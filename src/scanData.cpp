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
	precursorFile.clear();
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

std::string scanData::Scan::makeOfSequenceFromSequence(std::string s) const
{
	//if s does not contain any modifications, just return s
	if(!(utils::strContains('*', s) ||
		 utils::strContains('(', s) ||
		 utils::strContains(')', s)))
		return s;
	
	std::string ret = "";
	for(int i = 0; i < s.length(); i++)
	{
		if(s[i] == ')')
			throw std::runtime_error("Invalid sequence: " + s);
		
		if(s[i] == '*'){
			ret[ret.length() - 1] = std::tolower(ret.back());
		}
		else if(s[i] == '('){
			//get end of paren
			size_t end = s.find(')', i);
			if(end == std::string::npos)
				throw std::runtime_error("Invalid sequence: " + s);
			
			//erase paren from s
			//sequence.erase(sequence.begin() + startIndex, sequence.begin() + end + 1);
			s.erase(s.begin() + i, s.begin() + end + 1);
			
			ret[ret.length() - 1] = std::tolower(ret.back());
		}
		else ret += s[i];
	}
	
	return ret;
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
	
	precursorFile = elems[0] + ".ms2";
	scanNum = std::stoi(elems[1]);
	charge = std::stoi(elems[3]);
}

/**
 \brief Get unique output file name without extension.
 */
std::string scanData::Scan::getOfNameBase(std::string parentFile, std::string seq) const
{
	std::string ret;
	ret = utils::removeExtension(parentFile);
	ret += ("_" + makeOfSequenceFromSequence(seq) + "_" + std::to_string(scanNum));
	if(charge != 0)
		ret += ("_" + std::to_string(charge));
	return ret;
}

/**
 \brief Calls getOfNameBase and adds OF_EXT to end.
 
 Only added for backwards compatibility.
 */
std::string scanData::Scan::getOfname() const{
	return getOfNameBase(precursorFile, sequence) + OF_EXT;
}
