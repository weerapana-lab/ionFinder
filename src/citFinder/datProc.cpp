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
 Align ref sequence to query sequence. To peptide sequences as strings are aligned.
 @param ref The reference sequence to search.
 @param query The sequence to search for.
 @param beg Beginning of match. If no match, left unchanged.
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
	
	containsCit = "false";
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
 Tests whether sequence contains ambiguous residues.
 @param ambResidues ambiguous residues to search for
 @param fragSeq sequence of fragment to search for ambiguous residues.
 @return True if an ambiguous residue is found.
*/
bool CitFinder::PeptideStats::containsAmbResidues(const std::string& ambResidues,
												  std::string fragSeq) const
{
	size_t len = fragSeq.length();
	for(int i = 0; i < len; i++)
		for(int j = 0; j < ambResidues.length(); j++)
			if(fragSeq[i] == ambResidues[j])
				return true;
	return false;
}

/**
 Add fragment sequence to PeptideStats.
 @param seq fragment ion to add
 @param ambResidues ambigious residues to search for.
 */
void CitFinder::PeptideStats::addSeq(const CitFinder::RichFragmentIon& seq,
									 const std::string& ambResidues)
{
	//first check if seq is found in *this sequence
	size_t beg, end;
	if(!CitFinder::allignSeq(sequence, seq.getSequence(), beg, end))
		return;
	
	//increment total fragment ions found
	nFrag++;
	std::string ionStr = seq.getIonStr(true);
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
				if(containsAmbResidues(ambResidues, seq.getSequence())) //is amb NL frag
				{
					nAmbNLFrag++;
					addChar(ionStr, ambNLFrag);
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
		/*if(it->getSequence() == "RHLETVDGAKVVVLVNR")
			std::cout << "Found!" << NEW_LINE;*/
		
		fragmentMap.clear();
		fragmentMap.populateMap(it->getSequence());
		//it->printFragments(std::cout);
		
		CitFinder::PeptideStats pepStat(*it); //init pepStat
		pepStat._scan = &scans[it-peptides.begin()]; //add pointer to scan
		size_t nFragments = it->getNumFragments();
		for(size_t i = 0; i < nFragments; i++)
		{
			/*if(it->getFragment(i).getIonStr(false) == "b14-43")
				std::cout << "Found!" << NEW_LINE;*/
			
			if(it->getFragment(i).getFound())
			{
				CitFinder::RichFragmentIon fragTemp(it->getFragment(i));
				fragTemp.calcSequence(fragmentMap);
				pepStat.addSeq(fragTemp, pars.getAmbigiousResidues());
			} //end of if
		}//end of for i
		pepStat.calcContainsCit();
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
			//first get current wd name
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
			std::string spFname = pars.getWD() + "/";
			if(pars.getInputMode() == CitFinder::Params::InputModes::SINGLE)
				spFname += "/sequest.params";
			else
				spFname += it->getSampleName() + "/sequest.params";
				
			pars.setSeqParFname(spFname);
			PeptideNamespace::Peptide::initAminoAcidsMasses(pars);
		}//end if
		
		//initialize peptide object for current scan
		peptides.push_back(PeptideNamespace::Peptide (it->getSequence()));
		peptides.back().initialize(pars);
		
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
		
		//print spectra file
		if(pars.getPrintSpectraFiles())
		{
			std::string dirNameTemp = pars.getWD() + "/spectraFiles";
			if(!utils::dirExists(dirNameTemp))
				if(!utils::mkdir(dirNameTemp.c_str()))
					return false;
			spectrum.normalizeIonInts(100);
			spectrum.calcLabelPos();
			//spectrm.makeOf
			std::string temp = it->getOfname();
			std::ofstream outF((dirNameTemp + "/" + temp).c_str());
			if(!outF) return false;
			spectrum.printLabeledSpectrum(outF, true);
		}
	}
	return true;
}

void CitFinder::PeptideStats::calcContainsCit()
{
	containsCit = "false";
	
	//is the peptide modified?
	if(modLocs.size() == 0) return;
	
	//is c terminal most cit modification on the c terminus?
	//modLocs.back() works because modLocs are added in the order
	//they appear in the sequence
	if(modLocs.back() == sequence.length() - 1) return;
	
	//is there a determining NL
	if(nDetNLFrag > 1 && nDetFrag >= 1){
		containsCit = "true";
		return;
	}
	
	//is there at least one detNLFrag and more than 1 detFrag
	if(nDetNLFrag == 1 && nDetFrag >= 1){
		containsCit = "likely";
		return;
	}
	
	if(nAmbNLFrag >= 1)
		containsCit = "ambiguous";
}

/**
 Prints peptide stats to out.
 @param stats Peptide stats to print.
 @param out stream to print to.
 */
void CitFinder::printPeptideStats(const std::vector<PeptideStats>& stats, std::ostream& out)
{
	assert(out);
	
	//build stat names vector
	std::string statNames = "containsCit nFrag nDetFrag nDetNLFrag nAmbFrag nAmbNLFrag nArtNLFrag";
	std::string listNames = " frag detFrag detNLFrag ambFrag ambNLFrag artNLFrag";
	std::vector<std::string> allNames;
	utils::split(statNames + listNames, ' ', allNames);
	
	std::string otherHeaders = "protein_ID parent_protein protein_description full_sequence sequence charge unique xCorr scan parent_file sample_name";
	std::vector<std::string> oHeaders;
	utils::split(otherHeaders, ' ', oHeaders);
	std::vector<std::string> headers;
	headers.reserve(statNames.size() + oHeaders.size());
	headers.insert(headers.end(), oHeaders.begin(), oHeaders.end());
	headers.insert(headers.end(), allNames.begin(), allNames.end());
	
	//print headers
	size_t len = headers.size();
	for(size_t i = 0; i < len; i++){
		if(i == 0)
			out << headers[i];
		else out << OUT_DELIM << headers[i];
	}
	out << NEW_LINE;
	
	//print data
	for(auto it = stats.begin(); it != stats.end(); ++it)
	{
		//scan data
		out << it->_scan->getParentID() << OUT_DELIM <<
		it->_scan->getParentProtein() << OUT_DELIM <<
		it->_scan->getParentDescription() << OUT_DELIM <<
		it->_scan->getFullSequence() << OUT_DELIM <<
		it->_scan->getSequence() << OUT_DELIM <<
		it->_scan->getCharge() << OUT_DELIM <<
		it->_scan->getUnique() << OUT_DELIM <<
		it->_scan->getXcorr() << OUT_DELIM <<
		it->_scan->getScanNum() << OUT_DELIM <<
		it->_scan->getParentFile() << OUT_DELIM <<
		it->_scan->getSampleName() << OUT_DELIM;
		
		//peptid analysis data
		out << it->containsCit << OUT_DELIM <<
		it->nFrag << OUT_DELIM <<
		it->nDetFrag << OUT_DELIM <<
		it->nDetNLFrag << OUT_DELIM <<
		it->nAmbFrag << OUT_DELIM <<
		it->nAmbNLFrag << OUT_DELIM <<
		it->nArtNLFrag << OUT_DELIM <<
		it->frag << OUT_DELIM <<
		it->detFrag << OUT_DELIM <<
		it->detNLFrag << OUT_DELIM <<
		it->ambFrag << OUT_DELIM <<
		it->ambNLFrag << OUT_DELIM <<
		it->artNLFrag << OUT_DELIM <<
		NEW_LINE;
	}
}
