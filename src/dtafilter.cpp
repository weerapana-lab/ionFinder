//
//  dtafilter.cpp
//  ionFinder
//
//  Created by Aaron Maurais on 12/9/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#include <dtafilter.hpp>

void Dtafilter::Scan::operator = (const Dtafilter::Scan& rhs)
{
	_parentProtein = rhs._parentProtein;
	_parentID = rhs._parentID;
	_matchDirection = rhs._matchDirection;
	_sampleName = rhs._sampleName;
	_precursorFile = rhs._precursorFile;
	_scanNum = rhs._scanNum;
	_sequence = rhs._sequence;
	_fullSequence = rhs._fullSequence;
	_charge = rhs._charge;
	_xcorr = rhs._xcorr;
}

/**
 \brief Get protein match direction. <br>
 
 Match direction is determined by checking  if \p str contains Dtafilter::REVERSE_MATCH.
 \param str \p db tag from fasta header line.
 See https://www.uniprot.org/help/fasta-headers for detials on fasta headers.
 \return match direction for fasta entry
 */
Dtafilter::Scan::MatchDirection Dtafilter::Scan::strToMatchDirection(std::string str)
{
	if(utils::strContains(REVERSE_MATCH, utils::toLower(str)))
		return MatchDirection::REVERSE;
	return MatchDirection::FORWARD;
}

bool Dtafilter::Scan::parse_matchDir_ID_Protein(std::string str)
{
	std::vector<std::string> elems;
	utils::split(str, '|', elems);
	
	try{
		_matchDirection = strToMatchDirection(elems.at(0));
		_parentID = elems.at(1);
		
		size_t underScoreI = elems.at(2).find_last_of("_");
		_parentProtein = elems.at(2).substr(0, underScoreI);
	}
	catch(std::out_of_range& e){
		std::cerr << "\n Error parsing protein id for " << str <<"\n Skipping...\n";
		return false;
	}
	
	if(_matchDirection == MatchDirection::REVERSE)
		_parentID = "reverse_" + _parentID;
	return true;
}

/**
 Read DTAFilter-file and populate peptides into \p scans.
 \p scans does not have to be empty. New scans are added with the std::vector::push_back method.
 \param fname File name
 \param sampleName Sample name to add to _sampleName member of each scan in \p scans
 \param scans Vector of scans to add to
 \param skipReverse Should reverse peptide matches be skipped?
 \param modFilter Which scans should be added to \p scans?
 	0: only modified, 1: all peptides regardless of modification, 2: only unmodified pepeitde.
 
 \return true if file I/O was sucessful.
 */
bool Dtafilter::readFilterFile(std::string fname,
							   std::string sampleName,
							   std::vector<Dtafilter::Scan>& scans,
							   bool skipReverse,
							   int modFilter)
{
	std::ifstream inF(fname);
	if(!inF) return false;
	
	//flow control flags
	bool foundHeader = false;
	std::streampos sp;
	
	std::string line;
	std::vector<std::string> elems;
	
	while(!inF.eof())
	{
		utils::safeGetline(inF, line, sp);
		line = utils::trimTraling(line);
		if(line.empty()) continue;
		
		if(utils::strContains('%', line)) //find protein header lines by percent symbol for percent coverage
		{
			if(!foundHeader)
			{
				if(utils::strContains("Conf%", line)) //skip if header line
				{
					foundHeader = true;
					continue;
				}
			}
			else{
				utils::split(line, IN_DELIM, elems);
				
				Scan baseScan;
				baseScan.parse_matchDir_ID_Protein(elems[0]);
				baseScan.setSampleName(sampleName);
				
				//extract shortened protein name and description
				size_t endOfDescription = elems[8].find(" [");
				baseScan.setParentDescription(elems[8].substr(0, endOfDescription));
				
				while(!inF.eof())
				{
					//if(getNewLine)
					utils::safeGetline(inF, line, sp);
					//getNewLine = true;
					line = utils::trimTraling(line);
					if(line.empty()) continue;
					
					//break if starting new protein or end of file
					if(utils::strContains('%', line) || line == "\tProteins\tPeptide IDs\tSpectra")
						break;
					
					Scan newScan = baseScan;
					newScan.initilizeFromLine(line);
					newScan.setUnique(line[0] == '*');
					newScan.setPrecursorFile(utils::dirName(fname) + "/" + newScan.getPrecursorFile());
					
					//reverse match filter
					if(skipReverse && newScan.getMatchDirection() == Dtafilter::Scan::MatchDirection::REVERSE)
						continue;
					
					//mod filter
					if((modFilter == 0 && !newScan.isModified()) ||
					   (modFilter == 2 && newScan.isModified()))
						continue;
					
					scans.push_back(newScan);
					
				}//end of while
				inF.seekg(sp); //reset streampos so line is not skipped in next itteration
			}
		}//end if
	}//end while
	
	return true;
}
