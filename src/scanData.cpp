//
//  scanData.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/23/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "../include/scanData.hpp"

void scanData::Scan::clear()
{
	parentFile.clear();
	scanNum = 0;
	sequence.clear();
	charge = 0;
	fullSequence.clear();
}

string scanData::Scan::makeSequenceFromFullSequence(string fs) const
{
	fs = fs.substr(fs.find(".") + 1);
	fs = fs.substr(0, fs.find_last_of("."));
	return fs;
}

scanData::Scan::Scan(string line)
{
	vector<string> elems;
	utils::split(line, IN_DELIM, elems);
	fullSequence = elems[12];
	sequence = makeSequenceFromFullSequence(fullSequence);
	xcorr = elems[2];
	
	string scanLine = elems[1];
	utils::split(scanLine, '.', elems);
	
	parentFile = elems[0] + ".ms2";
	scanNum = utils::toInt(elems[1]);
	charge = utils::toInt(elems[3]);
}

string scanData::Scan::getOfname() const
{
	string ret;
	ret = utils::removeExtension(parentFile);
	ret += ("_" + sequence + "_" + utils::toString(scanNum));
	if(charge != 0)
		ret += ("_" + utils::toString(charge));
	ret += OF_EXT;
	
	return ret;
}
