//
//  datProc.cpp
//  citFinder
//
//  Created by Aaron Maurais on 12/10/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#include <citFinder/datProc.hpp>

/**
 Clear PeptideFragmentsMap.
 */
void CitFinder::PeptideFragmentsMap::clear()
{
	fragmentMap.clear();
	_sequence.clear();
}

/**
 Generate fragment map from sequence.
 From a full sequence given, the sequence of all b and y ions are calculated
 and stored in the object.
 @param sequence the peptide sequence to calculate fragments from
 */
void CitFinder::PeptideFragmentsMap::populateMap(std::string sequence)
{
	_sequence = sequence;
	size_t len = _sequence.length();
	for(int i = 0; i < len; i++)
	{
		std::string beg = _sequence.substr(0, i + 1);
		std::string end = _sequence.substr(i + 1);
		
		fragmentMap["b" + std::to_string(i+1)] = beg;
		fragmentMap["y" + std::to_string(len-i-1)] = end;
	}
}

std::string CitFinder::PeptideFragmentsMap::getIonSeq(std::string searchStr) const{
	return fragmentMap.at(searchStr);
}

std::string CitFinder::PeptideFragmentsMap::getIonSeq(char b_y, int num) const{
	std::string searchStr = std::string(1, b_y) + std::to_string(num);
	return getIonSeq(searchStr);
}

void CitFinder::RichFragmentIon::calcSequence(const PeptideFragmentsMap& pepMap){
	sequence = pepMap.getIonSeq(b_y, num);
}

/**
 Allign ref sequence to query sequence. To peptide sequences as strings are alligned.
 @param ref The reference sequence to search.
 @param query The sequence to search for.
 @param beg Begining of match. If no match, left unchanged.
 @param end End of match. If no match, left unchanged.
 @return bool representing if match was found.
 */
bool CitFinder::allignSeq(const std::string& ref, const std::string& query, size_t& beg, size_t& end)
{
	//find query in match
	size_t match = ref.find(query);
	if(match == std::string::npos) return false;
	
	beg = match;
	end = match + query.length() - 1;
	
	return true;
}

void CitFinder::PeptideStats::initStats()
{
	//fragment counts
	nFrag = 0;
	nAmbFrag = 0;
	nDetFrag = 0;
	nAmbNLFrag = 0;
	nDetNLFrag = 0;
	nArtNLFrag = 0;
	
	//fragment lists
	frag = "";
	ambFrag = "";
	detFrag = "";
	ambNLFrag = "";
	detNLFrag = "";
	artNLFrag = "";
}

void CitFinder::PeptideStats::initModLocs(const char* diffmods)
{
	size_t modLoc = std::string::npos;
	bool modFound = false;
	size_t len = fullSequence.length();
	std::string tempSeq = fullSequence;
	for(size_t i = 0; i < len; i++)
	{
		//check that current char is not a mod char
		for(const char* p = diffmods; *p; p++)
			if(tempSeq[i] == *p)
				throw std::runtime_error("Invalid peptide sequence!");
		
		//if not at second to last AA, search for diff mod
		if((i + 1) < tempSeq.length())
		{
			//iterate through diffmods
			for(const char* p = diffmods; *p; p++)
			{
				//check if next char in sequence is diff mod char
				if(tempSeq[i + 1] == *p)
				{
					modFound = true;
					modLoc = i;
					tempSeq.erase(i + 1, 1);
					break;
				}//end of if
			}//end of for
		}//end of if
		//PeptideIon temp(aminoAcidMasses->getMW(sequence[i]));
		if(modFound){
			//temp.setMod(mod, aminoAcidMasses->getMW(mod));
			modLocs.push_back(modLoc);
			//nMod++;
		}
		//aminoAcids.push_back(temp);
		modFound = false;
	}//end of for
}

void CitFinder::PeptideStats::addChar(std::string toAdd, std::string& s)
{
	if(s.empty())
		s = toAdd;
	else s += _fragDelim + toAdd;
}

/**
 Tests whether sequence contains ambigious residues.
 @param ambResidues ambigious residues to search for
 @return True if an ambigious residue is found.
*/
bool CitFinder::PeptideStats::containsAmbResidues(const std::string& ambResidues) const
{
	size_t len = sequence.length();
	for(int i = 0; i < len; i++)
		for(int j = 0; j < ambResidues.length(); j++)
			if(sequence[i] == ambResidues[j])
				return true;
	return false;
}

void CitFinder::PeptideStats::addSeq(const CitFinder::RichFragmentIon& seq,
									 const std::string& ambResidues)
{
	//TODO: problem with artNlFrag
	
	//first check if seq is found in *this sequence
	size_t beg, end;
	if(!CitFinder::allignSeq(sequence, seq.getSequence(), beg, end))
		return;
	
	//increment total fragment ions found
	nFrag++;
	std::string ionStr = seq.getIonStr(false);
	addChar(ionStr, frag);
	
	//check if seq spans any modified residues
	for(auto it = modLocs.begin(); it != modLocs.end(); ++it)
	{
		//check if in span
		if(utils::inSpan(beg, end, *it))
		{
			//check if NL
			if(seq.isNL())
			{
				if(containsAmbResidues(ambResidues)) //is amb NL frag
				{
					nAmbFrag++;
					addChar(ionStr, ambFrag);
				}
				else //is det NL frag
				{
					nDetNLFrag++;
					addChar(ionStr, detNLFrag);
				}
			}
			else //is determining fragment
			{
				nDetFrag++;
				addChar(ionStr, detFrag);
			}
		}
		else{
			if(seq.isNL()) //is artifact NL frag
			{
				nArtNLFrag++;
				addChar(ionStr, artNLFrag);
			}
			else //is amg frag
			{
				nAmbFrag++;
				addChar(ionStr, ambFrag);
			}//end of else
		}//end of else
	}//end of for
}//end of fxn

/**
 
 @param peptides vector of peptides to analyze
 */
void CitFinder::analyzeSequences(std::vector<Dtafilter::Scan>& scans,
								 const std::vector<PeptideNamespace::Peptide>& peptides,
								 std::vector<PeptideStats>& peptideStats,
								 const CitFinder::Params& pars)
{
	CitFinder::PeptideFragmentsMap fragmentMap;
	
	for(auto it = peptides.begin(); it != peptides.end(); ++it)
	{
		fragmentMap.clear();
		fragmentMap.populateMap(it->getSequence());
		//it->printFragments(std::cout);
		
		CitFinder::PeptideStats pepStat(*it); //init pepStat
		pepStat._scan = &scans[it-peptides.begin()]; //add pointer to scan
		size_t nFragments = it->getNumFragments();
		for(size_t i = 0; i < nFragments; i++)
		{
			if(it->getFragment(i).getFound())
			{
				CitFinder::RichFragmentIon fragTemp(it->getFragment(i));
				fragTemp.calcSequence(fragmentMap);
				pepStat.addSeq(fragTemp, pars.getAmbigiousResidues());
			} //end of if
		}//end of for i
		peptideStats.push_back(pepStat);
	}//end if for it
}//end of fxn

/**
 Find peptide fragment ions in ms2 files.
 @param peptides empty vector of peptides to be filled from data in scans.
 @param scans vector of scans from filter file.
 @param pars CitFinder params object.
 */
bool CitFinder::findFragments(const std::vector<Dtafilter::Scan>& scans,
							  std::vector<PeptideNamespace::Peptide>& peptides,
							  CitFinder::Params& pars)
{
	std::map<std::string, ms2::Ms2File> ms2Map;
	std::string curSample;
	std::string curWD;
	for(auto it = scans.begin(); it != scans.end(); ++it)
	{
		//read ms2 files if it hasn't been done yet
		if(curSample != it->getSampleName())
		{
			//read ms2 files
			ms2Map.clear();
			//first get curent wd name
			curSample = it->getSampleName();
			curWD = pars.getInputModeIndependentParentDir() + "/" + curSample;
			
			//next get names of all ms2 files in dir
			std::vector<std::string> ms2FileNames;
			if(!utils::ls(curWD.c_str(), ms2FileNames, ".ms2"))
			{
				std::cerr << "Error reading ms2 files!" << NEW_LINE;
				return false;
			}
			
			//finally read each ms2 file into map
			for(auto it2 = ms2FileNames.begin(); it2 != ms2FileNames.end(); ++it2)
			{
				ms2Map[*it2] = ms2::Ms2File();
				if(!ms2Map[*it2].read(curWD + "/" + *it2))
				{
					std::cerr << "Error reading ms2 files!" << NEW_LINE;
					return false;
				}
			}
			
			//re-init Peptide::AminoAcidMasses for each sample
			pars.setSeqParFname(pars.getWD() + "/sequest.params");
			PeptideNamespace::Peptide::initAminoAcidsMasses(pars);
		}//end if
		
		//initilize peptide object for current scan
		peptides.push_back(PeptideNamespace::Peptide (it->getSequence()));
		peptides.back().initalize(pars);
		
		//calculate neutral loss combinations
		int nMods = peptides.back().getNumMod();
		double nlMass = pars.getNeutralLossMass();
		std::vector<double> neutralLossIons;
		for(int i = 1; i <= nMods; i++)
			neutralLossIons.push_back(i * nlMass);
		
		//add neutral loss fragments to current peptide
		peptides.back().addNeutralLoss(neutralLossIons);
		
		//load spectrum
		ms2::Spectrum spectrum;
		if(!ms2Map[it->getParentFile()].getScan(it->getScanNum(), spectrum))
		{
			std::cout << "Error reading scan!" << NEW_LINE;
			return false;
		}
		spectrum.labelSpectrum(peptides.back(), pars);
		
		//temp for debuging
		spectrum.normalizeIonInts(100);
		spectrum.calcLabelPos();
		std::ofstream outF("/Users/Aaron/local/ms2_anotator/testFiles/nl_test.spectrum");
		spectrum.printLabeledSpectrum(outF, true);
	}
	return true;
}
