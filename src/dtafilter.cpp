//
//  dtafilter.cpp
//  citFinder
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
	parentFile = rhs.parentFile;
	scanNum = rhs.scanNum;
	sequence = rhs.sequence;
	fullSequence = rhs.fullSequence;
	charge = rhs.charge;
	xcorr = rhs.xcorr;
}

Dtafilter::Scan::MatchDirection Dtafilter::Scan::strToMatchDirection(std::string str) const
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

bool Dtafilter::readFilterFile(std::string fname,
							   std::string sampleName,
							   std::vector<Dtafilter::Scan>& scans,
							   bool skipReverse)
{
	std::ifstream inF(fname);
	if(!inF) return false;
	
	//flow control flags
	bool foundHeader = false;
	bool getNewLine = true;
	
	std::string line;
	std::vector<std::string> elems;
	
	while(!inF.eof())
	{
		if(getNewLine)
			utils::safeGetline(inF, line);
		getNewLine = true;
		
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
				baseScan._sampleName = sampleName;
				
				//extract shortened protein name and description
				size_t endOfDescription = elems[8].find(" [");
				baseScan._parentDescription = elems[8].substr(0, endOfDescription);
				
				while(!inF.eof())
				{
					if(getNewLine)
						utils::safeGetline(inF, line);
					getNewLine = true;
					
					//break if starting new protein or end of file
					if(utils::strContains('%', line) || line == "\tProteins\tPeptide IDs\tSpectra")
						break;
					
					Scan newScan = baseScan;
					newScan.initilizeFromLine(line);
					newScan._unique = line[0] == '*';
					if(skipReverse && newScan._matchDirection == Dtafilter::Scan::MatchDirection::REVERSE)
						continue;
					else scans.push_back(newScan);
					
				}//end of while
				getNewLine = false;
			}
		}//end if
	}//end while
	
	return true;
}

bool Dtafilter::readFilterFiles(const CitFinder::Params& params,
								std::vector<Dtafilter::Scan>& scans)
{
	auto endIt = params.getFilterFiles().end();
	for(auto it = params.getFilterFiles().begin(); it != endIt; ++it)
	{
		if(!Dtafilter::readFilterFile(it->second, it->first, scans, !params.getIncludeReverse()))
			return false;
	}
	
	return true;
}
