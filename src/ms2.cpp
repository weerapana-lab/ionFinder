//
//  ms2.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/6/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "../include/ms2.hpp"

bool ms2::Ms2File::read(string _fname)
{
	fname = _fname;
	return read();
}

bool ms2::Ms2File::read()
{
	if(fname.empty())
		throw runtime_error("File must be specified!");
	ifstream inF(fname.c_str());
	if(!inF)
		return false;
	
	delimType = utils::detectLineEnding(inF);
	if(delimType == utils::unknown)
		throw runtime_error("Invalid delimiter in file: " + fname + "!");
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
	stringstream ss;
	string line;
	size_t end = utils::offset(buffer, size, "LastScan") + 100;
	for(size_t i = 0; i < end; i++)
		ss.put(*(buffer + i));
	size_t sLen = ss.str().length();
	
	vector<string> elems;
	int mdCount = 0;
	char _delim = utils::getDelim(delimType);
	//iterate throuth ss to find metadata
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
				firstScan = elems[2];
			else if(elems[1] == "LastScan")
				lastScan = elems[2];
			else continue;
			mdCount++;
		}
	}
	
	//check that md is good
	if(scanType == "MS2" &&
	   dataType == "Centroid" &&
	   utils::toInt(firstScan) <= utils::toInt(lastScan) &&
	   mdCount == mdNum)
		return true;
	return false;
}

const char* ms2::Ms2File::makeOffsetQuery(string queryScan) const
{
	size_t qsInt = utils::toInt(queryScan);
	return makeOffsetQuery(qsInt);
}

const char* ms2::Ms2File::makeOffsetQuery(size_t queryScan) const
{
	size_t numDigitsFinal = NUM_DIGITS_IN_SCAN;
	size_t numDigitsQuery = utils::numDigits(queryScan);
	int repeatNum = int(numDigitsFinal - numDigitsQuery);
	
	string ret = "S\t" +
		utils::repeat("0", repeatNum) + utils::toString(queryScan) +
		"\t" + utils::repeat("0", repeatNum) + utils::toString(queryScan);
	
	return ret.c_str();
}

bool ms2::Ms2File::getScan(string queryScan, Spectrum& scan) const
{
	return getScan(utils::toInt(queryScan), scan);
}

bool ms2::Ms2File::getScan(size_t queryScan, Spectrum& scan) const
{
	scan.clear();
	const char* query = makeOffsetQuery(queryScan);
	size_t scanOffset = utils::offset(buffer, size, query);
	if(scanOffset == size)
		return false;
	
	const char* _delim = delim.c_str();
	char* _scan = strtok(strdup(buffer + scanOffset), _delim);
	vector<string> elems;
	size_t numIons = 0;
	
	do{
		string line = string(_scan);
		utils::split(line, IN_DELIM, elems);
		if(_scan[0] == 'S')
		{
			assert(elems.size() == 4);
			utils::split(line, IN_DELIM, elems);
			scan.scanNumber = utils::toInt(elems[2]);
			scan.precursorMZ = utils::toDouble(elems[3]);
		}
		else if(_scan[0] == 'I')
		{
			assert(elems.size() == 3);
			utils::split(line, IN_DELIM, elems);
			if(elems[1] == "RetTime")
				scan.retTime = utils::toDouble(elems[2]);
			else if(elems[1] == "PrecursorInt")
				scan.precursorInt = utils::toDouble(elems[2]);
			else if(elems[1] == "PrecursorFile")
				scan.precursorFile = utils::removeExtension(elems[2]);
			else if(elems[1] == "PrecursorScan")
				scan.precursorScan = utils::toInt(elems[2]);
		}
		else if(_scan[0] == 'Z'){
			assert(elems.size() == 3);
			utils::split(line, IN_DELIM, elems);
			scan.precursorCharge = utils::toInt(elems[1]);
		}
		else if(utils::isInteger(string(1, _scan[0]))){
			
			utils::split(line, ' ', elems);
			assert(elems.size() == 2);
			DataPoint tempIon (utils::toDouble(elems[0]), utils::toDouble(elems[1]));
			
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




