//
//  dtafilter.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/21/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "../include/dtafilter.hpp"

bool dtafilter::DtaFilterFile::read(string _fname)
{
	fname = _fname;
	return read();
}

bool dtafilter::DtaFilterFile::read()
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
	if(!inF.read(buffer, size))
		return false;
	return true;
}

void dtafilter::DtaFilterFile::getScans(const string& _seq, scanData::scansType& _scans) const
{
	_scans.clear();
	const char* query = _seq.c_str();
	char* instance;
	instance = strstr(buffer, query);
	
	while(instance != nullptr)
	{
		_scans.push_back(scanData::Scan(getScanLine(instance - buffer + 1)));
		//_scans.back().setSequence(_seq);
		instance = strstr(instance + 1, query);
	}
}

bool dtafilter::DtaFilterFile::getScan(const string& _seq, scanData::scansType& _scans, bool force) const
{
	string outDelim = "\t";
	bool annotateAll = false;
	int annotate = -1;
	scanData::scansType scans;
	
	getScans(_seq, scans);
	if(scans.size() == 0)
	{
		cout << "Sequence: " << _seq << " not found in dtafilter file." << endl;
		return false;
	}
	
	if(scans.size() > 1 && !force)
	{
		cout << "Multiple matches found. Choose which scan you would like to annotate." << endl;
		cout << endl << "Match_number" << outDelim << "Parent_file" << utils::repeat(outDelim, 2) << "Scan"
		<< outDelim << "Xcorr" << outDelim << "Sequence" << outDelim << "Charge" << endl;
		
		size_t i = 0;
		size_t len = scans.size();
		for(; i < len; i++)
			cout << i << ")" << outDelim << scans[i].getParentFile()
			<< outDelim << scans[i].getScanNum()
			<< outDelim << scans[i].getXcorr()
			<< outDelim << scans[i].getFullSequence()
			<< utils::repeat(outDelim, 2) << scans[i].getCharge() << outDelim << endl;
		cout << i << ")" << outDelim << "Annotate all." << endl << "Enter choice: ";
		
		string choice;
		bool good;
		do{
			choice.clear();
			cin.sync();
			std::getline(std::cin, choice);
			choice = utils::trim(choice);
			if(utils::isInteger(choice))
			{
				annotate = utils::toInt(choice);
				if(annotate >= 0 && annotate <= scans.size())
				{
					if(annotate == scans.size())
						annotateAll = true;
					good = true;
				}
				else{
					good = false;
					cout << "Invalid choice!" << endl << "Enter choice: ";
				}
			}
			else{
				good = false;
				cout << "Invalid choice!" << endl << "Enter choice: ";
			}
		} while(!good);
	}
	
	if(!annotateAll)
		_scans.push_back(scans[annotate]);
	else _scans = scans;
	
	return true;
}

bool dtafilter::DtaFilterFile::getFirstScan(const string& _seq, scanData::Scan& _scan) const
{
	_scan.clear();
	
	//get offset for _seq in file buffer
	const char* query = _seq.c_str();
	size_t offset = utils::offset(buffer, size, query);
	if(offset == size)
	{
		cerr << "Sequence: " << _seq << " not found in dtafilter file." << endl;
		return false;
	}
	
	//initalize _scan
	string line = getScanLine(offset);
	_scan = scanData::Scan(line);
	//_scan.setSequence(_seq);
	
	return true;
}

string dtafilter::DtaFilterFile::getScanLine(size_t offset) const
{
	size_t lineBegin = getBeginLine(offset);
	size_t lineEnd = getEndLine(offset);
	string line(buffer + lineBegin, (lineEnd - lineBegin));
	line = utils::trim(line);
	return line;
}

size_t dtafilter::DtaFilterFile::getBeginLine(size_t offset) const
{
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
	char _delim = delim[0];
	while(*(buffer + offset) != _delim)
	{
		if(offset > size)
			return false;
		offset++;
	}
	return offset;
}



