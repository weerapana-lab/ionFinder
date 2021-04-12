//
// dtafilterFile.cpp
// ionFinder
// -----------------------------------------------------------------------------
// MIT License
// Copyright 2020 Aaron Maurais
// -----------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -----------------------------------------------------------------------------
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

void Ms2_annotator::DtaFilterFile::getScans(const std::string& _seq, scanData::scansType& scans) const
{	
	for(auto it = _scans.begin(); it != _scans.end(); ++it)
		if(it->getFullSequence().find(_seq) != std::string::npos)
			scans.push_back(*it);
}

bool Ms2_annotator::DtaFilterFile::getScan(const std::string& _seq,
										   scanData::scansType& scans,
										   bool force) const
{
	std::string outDelim = "\t";
	bool annotateAll = false;
	int annotate = -1;
	scanData::scansType scansTemp;
	
	getScans(_seq, scansTemp);
	if(scansTemp.size() == 0)
	{
		std::cout << "Sequence: " << _seq << " not found in dtafilter file." <<NEW_LINE;
		return false;
	}
	if(scansTemp.size() == 1){
		annotate = 0;
	}
	if(scansTemp.size() > 1 && !force)
	{
		std::cout << "Multiple matches found. Choose which scan you would like to annotate." << NEW_LINE;
		std::cout << NEW_LINE << "Match_number" << outDelim << "Parent_file" << utils::repeat(outDelim, 2) << "Scan"
		<< outDelim << "Xcorr" << outDelim << "Sequence" << utils::repeat(outDelim, 2) << "Charge" << NEW_LINE;
		
		size_t i = 0;
		size_t len = scansTemp.size();
		for(; i < len; i++)
		{
			std::cout << i << ")" << outDelim << scansTemp[i].getPrecursorFile()
			<< outDelim << scansTemp[i].getScanNum()
			<< outDelim << scansTemp[i].getXcorr()
			<< outDelim << scansTemp[i].getFullSequence()
			<< utils::repeat(outDelim, 2) << scansTemp[i].getCharge() << outDelim <<NEW_LINE;
		}
		std::cout << i << ")" << outDelim << "Annotate all." << NEW_LINE << "Enter choice: ";
		
		annotate = utils::readInt(0, int(scansTemp.size()));
		if(annotate == scansTemp.size())
			annotateAll = true;
	}
	
	if(!annotateAll)
		scans.push_back(scansTemp[annotate]);
	else scans = scansTemp;
	
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

