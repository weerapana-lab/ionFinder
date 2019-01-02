//
//  dtafilterFile.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/21/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <ms2_annotator/dtafilterFile.hpp>

bool Ms2_annotator::DtaFilterFile::read(std::string fname)
{
	_fname = fname;
	return read();
}

bool Ms2_annotator::DtaFilterFile::read()
{
	if(!Dtafilter::readFilterFile(_fname, utils::dirName(_fname), _scans))
		return false;
	return true;
}

void Ms2_annotator::DtaFilterFile::getScans(const std::string& _seq, scanData::scansType& _scans) const
{
	/*_scans.clear();
	const char* query = _seq.c_str();
	size_t queryLen = strlen(query);
	char* instance = buffer;
	
	while(instance)
	{
		if(strstr(instance, query) == NULL){
			break;
		}
		instance = strstr(instance, query);
		if(instance){
			_scans.push_back(scanData::Scan(getScanLine(instance - buffer + 1)));
			instance += queryLen;
		}
	}*/
	
	
}

bool Ms2_annotator::DtaFilterFile::getScan(const std::string& _seq, scanData::scansType& _scans, bool force) const
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

/*bool Ms2_annotator::DtaFilterFile::getFirstScan(const std::string& _seq, scanData::Scan& _scan) const
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
	
	//initialize _scan
	std::string line = getScanLine(offset);
	_scan = scanData::Scan(line);
	//_scan.setSequence(_seq);
	
	return true;
}

std::string Ms2_annotator::DtaFilterFile::getScanLine(size_t offset) const
{
	//std::cout << "getScanLine" << NEW_LINE;
	size_t lineBegin = getBeginLine(offset);
	size_t lineEnd = getEndLine(offset);
	std::string line(buffer + lineBegin, (lineEnd - lineBegin));
	line = utils::trim(line);
	return line;
}

size_t Ms2_annotator::DtaFilterFile::getBeginLine(size_t offset) const
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

size_t Ms2_annotator::DtaFilterFile::getEndLine(size_t offset) const
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
}*/

