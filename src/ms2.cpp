//
//  ms2.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/6/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <ms2.hpp>

int ms2::Ms2File::mdNum = 4;

bool ms2::Ms2File::read(std::string _fname)
{
	fname = _fname;
	return read();
}

bool ms2::Ms2File::read()
{
	if(fname.empty())
		throw std::runtime_error("File must be specified!");
	std::ifstream inF(fname.c_str());
	if(!inF)
		return false;
	
	delimType = utils::detectLineEnding(inF);
	if(delimType == utils::unknown)
		throw std::runtime_error("Invalid delimiter in file: " + fname + "!");
	delim = utils::getDelimStr(delimType);
	if(delimType == utils::crlf)
		beginLine = 1;
	else beginLine = 0;
	
	inF.seekg(0, inF.end);
	size = inF.tellg();
	inF.seekg(0, inF.beg);
	buffer = new char [size];
	if(inF.read(buffer, size))
		return getMetaData();
	return false;
}

bool ms2::Ms2File::getMetaData()
{
	//find header in buffer and put it into ss
	std::stringstream ss;
	std::string line;
	size_t end = utils::offset(buffer, size, "LastScan") + 100;
	for(size_t i = 0; i < end; i++)
		ss.put(*(buffer + i));
	size_t sLen = ss.str().length();
	
	std::vector<std::string> elems;
	int mdCount = 0;
	char _delim = utils::getDelim(delimType);
	//iterate through ss to find metadata
	while(ss.tellg() < sLen){
		utils::getLine(ss, line, _delim, beginLine);
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
	   mdCount == mdNum)
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

bool ms2::Ms2File::getScan(std::string queryScan, Spectrum& scan) const
{
	return getScan(std::stoi(queryScan), scan);
}

bool ms2::Ms2File::getScan(size_t queryScan, Spectrum& scan) const
{
	if(!((queryScan >= firstScan) && (queryScan <= lastScan)))
	{
		std::cout << "queryScan not in file scan range!" << NEW_LINE;
		return false;
	}
	
	scan.clear();
	const char* query = makeOffsetQuery(queryScan);
	size_t scanOffset = utils::offset(buffer, size, query);
	if(scanOffset == size)
	{
		std::cout << "queryScan could not be found!" << NEW_LINE;
		return false;
	}
	
	const char* _delim = delim.c_str();
	char* _scan = strtok(strdup(buffer + scanOffset), _delim);
	std::vector<std::string> elems;
	size_t numIons = 0;
	
	do{
		std::string line = std::string(_scan);
		utils::split(line, IN_DELIM, elems);
		if(_scan[0] == 'S')
		{
			assert(elems.size() == 4);
			utils::split(line, IN_DELIM, elems);
			scan.scanNumber = std::stoi(elems[2]);
			scan.precursorMZ = std::stod(elems[3]);
		}
		else if(_scan[0] == 'I')
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
		else if(_scan[0] == 'Z'){
			assert(elems.size() == 3);
			utils::split(line, IN_DELIM, elems);
			scan.precursorCharge = std::stoi(elems[1]);
		}
		else if(utils::isInteger(std::string(1, _scan[0]))){
			
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
		}
		_scan = strtok(nullptr, _delim);
	} while(_scan != nullptr && _scan[0] != 'S');
	
	scan.mzRange = scan.maxMZ - scan.minMZ;
	
	return true;
}




