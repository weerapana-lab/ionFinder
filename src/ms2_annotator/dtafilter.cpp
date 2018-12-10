//
//  dtafilter.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/21/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <ms2_annotator/dtafilter.hpp>

bool dtafilter::DtaFilterFile::read(std::string _fname)
{
	fname = _fname;
	return read();
}

bool dtafilter::DtaFilterFile::read()
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
	if(!inF.read(buffer, size))
		return false;
	return true;
}

void dtafilter::DtaFilterFile::getScans(const std::string& _seq, scanData::scansType& _scans) const
{
	//std::cout << "getScans" << NEW_LINE;
	//int iterations = 0;
	
	_scans.clear();
	const char* query = _seq.c_str();
	size_t queryLen = strlen(query);
	char* instance = buffer;
	
	while(instance)
	{
		//std::cout << "it num " << iterations << NEW_LINE;
		//iterations++;
		//std::cout << "pre inc" << NEW_LINE;
		//printf("%.*s\n", 100, instance);
		if(strstr(instance, query) == NULL)
		{
			//std::cout << "breaking" << NEW_LINE;
			break;
		}
		instance = strstr(instance, query);
		//std::cout << "post inc" << NEW_LINE;
		//printf("%.*s\n", 100, instance);
		if(instance){
			//std::cout << "push_back" << NEW_LINE;
			//std::cout << "offset is " << instance - buffer + 1 << NEW_LINE;
			_scans.push_back(scanData::Scan(getScanLine(instance - buffer + 1)));
			instance += queryLen;
		}
	}
	
	/*while(instance != nullptr)
	{
		_scans.push_back(scanData::Scan(getScanLine(instance - buffer + 1)));
		//_scans.back().setSequence(_seq);
		std::cout << "getScans" << NEW_LINE;
		instance = strstr(instance + 1, query);
	}*/
}

bool dtafilter::DtaFilterFile::getScan(const std::string& _seq, scanData::scansType& _scans, bool force) const
{
	std::string outDelim = "\t";
	bool annotateAll = false;
	int annotate = -1;
	scanData::scansType scans;
	
	getScans(_seq, scans);
	if(scans.size() == 0)
	{
		std::cout << "Sequence: " << _seq << " not found in dtafilter file." <<NEW_LINE;
		return false;
	}
	if(scans.size() == 1){
		annotate = 0;
	}
	if(scans.size() > 1 && !force)
	{
		std::cout << "Multiple matches found. Choose which scan you would like to annotate." << NEW_LINE;
		std::cout << NEW_LINE << "Match_number" << outDelim << "Parent_file" << utils::repeat(outDelim, 2) << "Scan"
		<< outDelim << "Xcorr" << outDelim << "Sequence" << utils::repeat(outDelim, 2) << "Charge" << NEW_LINE;
		
		size_t i = 0;
		size_t len = scans.size();
		for(; i < len; i++)
		{
			std::cout << i << ")" << outDelim << scans[i].getParentFile()
			<< outDelim << scans[i].getScanNum()
			<< outDelim << scans[i].getXcorr()
			<< outDelim << scans[i].getFullSequence()
			<< utils::repeat(outDelim, 2) << scans[i].getCharge() << outDelim <<NEW_LINE;
		}
		std::cout << i << ")" << outDelim << "Annotate all." << NEW_LINE << "Enter choice: ";
		
		annotate = utils::getInt(0, int(scans.size()));
		if(annotate == scans.size())
			annotateAll = true;
	}
	
	if(!annotateAll)
		_scans.push_back(scans[annotate]);
	else _scans = scans;
	
	return true;
}

bool dtafilter::DtaFilterFile::getFirstScan(const std::string& _seq, scanData::Scan& _scan) const
{
	_scan.clear();
	
	//get offset for _seq in file buffer
	const char* query = _seq.c_str();
	size_t offset = utils::offset(buffer, size, query);
	if(offset == size)
	{
		std::cerr << "Sequence: " << _seq << " not found in dtafilter file." <<NEW_LINE;
		return false;
	}
	
	//initalize _scan
	std::string line = getScanLine(offset);
	_scan = scanData::Scan(line);
	//_scan.setSequence(_seq);
	
	return true;
}

std::string dtafilter::DtaFilterFile::getScanLine(size_t offset) const
{
	//std::cout << "getScanLine" << NEW_LINE;
	size_t lineBegin = getBeginLine(offset);
	size_t lineEnd = getEndLine(offset);
	std::string line(buffer + lineBegin, (lineEnd - lineBegin));
	line = utils::trim(line);
	return line;
}

size_t dtafilter::DtaFilterFile::getBeginLine(size_t offset) const
{
	//std::cout << "begin" << NEW_LINE;
	char _delim = delim[0];
	while(*(buffer + offset) != _delim)
	{
		if(offset <= 0)
			return false;
		offset--;
	}
	return offset;
}

size_t dtafilter::DtaFilterFile::getEndLine(size_t offset) const
{
	//std::cout << "end" << NEW_LINE;
	char _delim = delim[0];
	while(*(buffer + offset) != _delim)
	{
		if(offset > size)
			return false;
		offset++;
	}
	return offset;
}

