//
//  ms2.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/6/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <ms2.hpp>

/**
 \brief Copy metadata values from rhs to *this
 */
void ms2::Ms2File::copyMetadata(const Ms2File& rhs)
{
	_parentMs2 = rhs._parentMs2;
	firstScan = rhs.firstScan;
	lastScan = rhs.lastScan;
	dataType = rhs.dataType;
	scanType = rhs.scanType;
}

/**
 \brief Initalize metadata values to empty variables
 */
void ms2::Ms2File::initMetadata()
{
	_parentMs2 = "";
	firstScan = 0;
	lastScan = 0;
	dataType = "";
	scanType = "";
}

bool ms2::Ms2File::read(std::string fname)
{
	_fname = fname;
	return Ms2File::read();
}

bool ms2::Ms2File::read()
{
	calcParentMs2(_fname);
	if(!BufferFile::read(_fname)) return false;
	return getMetaData();
}

bool ms2::Ms2File::getMetaData()
{
	//find header in buffer and put it into ss
	std::stringstream ss;
	std::string line;
	size_t end = utils::offset(_buffer, _size, "LastScan") + 100;
	for(size_t i = 0; i < end; i++)
		ss.put(*(_buffer + i));
	size_t sLen = ss.str().length();
	
	std::vector<std::string> elems;
	int mdCount = 0;
	
	//iterate through ss to find metadata
	while(ss.tellg() < sLen){
		utils::safeGetLine(ss, line);
		if(line[0] == 'H')
		{
			utils::split(line, IN_DELIM, elems);
			if(elems[1] == "ScanType")
				scanType = elems[2];
			else if(elems[1] == "DataType")
				dataType = elems[2];
			else if(elems[1] == "FirstScan")
				firstScan = std::stoi(elems[2]);
			else if(elems[1] == "LastScan")
				lastScan = std::stoi(elems[2]);
			else continue;
			mdCount++;
		}
	}
	
	//check that md is good
	if(scanType == "MS2" &&
	   dataType == "Centroid" &&
	   firstScan <= lastScan &&
	   mdCount == MD_NUM)
		return true;
	return false;
}

const char* ms2::Ms2File::makeOffsetQuery(std::string queryScan) const
{
	size_t qsInt = std::stoi(queryScan);
	return makeOffsetQuery(qsInt);
}

const char* ms2::Ms2File::makeOffsetQuery(size_t queryScan) const
{
	size_t numDigitsFinal = NUM_DIGITS_IN_SCAN;
	size_t numDigitsQuery = utils::numDigits(queryScan);
	int repeatNum = int(numDigitsFinal - numDigitsQuery);
	
	std::string ret = "S\t" +
		utils::repeat("0", repeatNum) + std::to_string(queryScan) +
		"\t" + utils::repeat("0", repeatNum) + std::to_string(queryScan);
	
	return ret.c_str();
}

/**
 \brief Overloaded function with \p queryScan as string
 */
bool ms2::Ms2File::getScan(std::string queryScan, Spectrum& scan) const{
	return getScan(std::stoi(queryScan), scan);
}

/**
 \brief Get parsed ms2::Spectrum from ms2 file.
 
 \param queryScan scan number to search for
 \param scan empty ms2::Spectrum to load scan into
 \return false if \p queryScan not found, true if sucessful
 */
bool ms2::Ms2File::getScan(size_t queryScan, ms2::Spectrum& scan) const
{
	scan.clear();
	scan._parentMs2 = _parentMs2;
	if(!((queryScan >= firstScan) && (queryScan <= lastScan)))
	{
		std::cout << "queryScan not in file scan range!" << NEW_LINE;
		return false;
	}
	
	const char* query = makeOffsetQuery(queryScan);
	size_t queryLen = strlen(query);
	size_t scanOffset = utils::offset(_buffer, _size, query);
	size_t endOfScan = utils::offset(_buffer + scanOffset + queryLen,
									 _size - (scanOffset + queryLen), "S\t") + queryLen;
	if(scanOffset == _size)
	{
		std::cout << "queryScan could not be found!" << NEW_LINE;
		return false;
	}
	
	std::vector<std::string> elems;
	std::string line;
	size_t numIons = 0;
	
	std::string temp (_buffer + scanOffset, _buffer + scanOffset + endOfScan);
	std::stringstream ss(temp);
	std::streampos oldPos = ss.tellg();
	bool z_found = false;
	
	while(utils::safeGetline(ss, line, oldPos))
	{
		if(line.empty()) continue;
		utils::split(line, IN_DELIM, elems);
		
		if(elems[0] == "S")
		{
			assert(elems.size() == 4);
			utils::split(line, IN_DELIM, elems);
			scan.scanNum = std::stoi(elems[2]);
			scan.precursorMZ = std::stod(elems[3]);
		}
		else if(elems[0] == "I")
		{
			assert(elems.size() == 3);
			utils::split(line, IN_DELIM, elems);
			if(elems[1] == "RetTime")
				scan.retTime = std::stod(elems[2]);
			else if(elems[1] == "PrecursorInt")
				scan.precursorInt = std::stod(elems[2]);
			else if(elems[1] == "PrecursorFile")
				scan.precursorFile = utils::removeExtension(elems[2]);
			else if(elems[1] == "PrecursorScan")
				scan.precursorScan = std::stoi(elems[2]);
		}
		else if(elems[0] == "Z"){
			if(!z_found){
				assert(elems.size() == 3);
				utils::split(line, IN_DELIM, elems);
				scan.charge = std::stoi(elems[1]);
				z_found = true;
			}
		}
		else if(utils::isInteger(std::string(1, elems[0][0])))
		{
			ss.seekg(oldPos);
			while(utils::safeGetLine(ss, line))
			{
				if(line.empty()) continue;
				
				utils::split(line, ' ', elems);
				assert(elems.size() >= 2);
				ms2::DataPoint tempIon (std::stod(elems[0]), std::stod(elems[1]));
				
				if(numIons == 0)
				{
					scan.minMZ = tempIon.getMZ();
					scan.minInt = tempIon.getIntensity();
				}
				
				scan.ions.push_back(tempIon);
				numIons++;
				
				//get min and max vals
				if(tempIon.getIntensity() > scan.maxInt)
					scan.maxInt = tempIon.getIntensity();
				if(tempIon.getIntensity() < scan.minInt)
					scan.minInt = tempIon.getIntensity();
				if(tempIon.getMZ() > scan.maxMZ)
					scan.maxMZ = tempIon.getMZ();
				if(tempIon.getMZ() < scan.minMZ)
					scan.minMZ = tempIon.getMZ();
			}//end of while
		}//end of else
	}//end of while
	
	scan.mzRange = scan.maxMZ - scan.minMZ;
	
	return true;
}

