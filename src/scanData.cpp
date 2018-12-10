//
//  scanData.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/23/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <scanData.hpp>

void scanData::Scan::clear()
{
	parentFile.clear();
	scanNum = 0;
	sequence.clear();
	charge = 0;
	fullSequence.clear();
}

std::string scanData::Scan::makeSequenceFromFullSequence(std::string fs) const
{
	fs = fs.substr(fs.find(".") + 1);
	fs = fs.substr(0, fs.find_last_of("."));
	return fs;
}

void scanData::Scan::initilizeFromLine(std::string line)
{
	std::vector<std::string> elems;
	utils::split(line, IN_DELIM, elems);
	fullSequence = elems[12];
	sequence = makeSequenceFromFullSequence(fullSequence);
	xcorr = elems[2];
	
	std::string scanLine = elems[1];
	utils::split(scanLine, '.', elems);
	
	parentFile = elems[0] + ".ms2";
	scanNum = std::stoi(elems[1]);
	charge = std::stoi(elems[3]);
}

std::string scanData::Scan::getOfname() const
{
	std::string ret;
	ret = utils::removeExtension(parentFile);
	ret += ("_" + sequence + "_" + std::to_string(scanNum));
	if(charge != 0)
		ret += ("_" + std::to_string(charge));
	ret += OF_EXT;
	
	return ret;
}
