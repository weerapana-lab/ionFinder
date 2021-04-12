//
// scanData.cpp
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

#include <scanData.hpp>

void scanData::Scan::clear()
{
	_scanNum = 0;
	_sequence.clear();
	_fullSequence.clear();
	_modified = false;
	_spectralCounts = 0;
	_precursor.clear();
}

//! Check whether _sequence contains any element in MOD_CHAR
bool scanData::Scan::checkIsModified() const {
    return utils::strContains(scanData::MOD_CHAR, _sequence);
}

std::string scanData::Scan::makeSequenceFromFullSequence(std::string fs) const
{
	fs = fs.substr(fs.find(".") + 1);
	fs = fs.substr(0, fs.find_last_of("."));
	return fs;
}

/**
 \brief Remove static modification symbols from peptide sequence \p s.
 
 \param s peptide sequence
 \param lowercase Should modified residue be transformed to lowercase?
 
 \return \p with static modifications removed.
 */
std::string scanData::removeStaticMod(std::string s, bool lowercase)
{
	//if s does not contain any modifications, just return s
	if(!(utils::strContains('(', s) ||
		 utils::strContains(')', s)))
		return s;
	
	std::string ret = "";
	for(size_t i = 0; i < s.length(); i++)
	{
		if(s[i] == ')')
			throw std::runtime_error("Invalid sequence: " + s);
		
		if(s[i] == '('){
			//get end of paren
			size_t end = s.find(')', i);
			if(end == std::string::npos)
				throw std::runtime_error("Invalid sequence: " + s);
			
			//erase paren from s
			s.erase(s.begin() + i, s.begin() + end + 1);
			
			if(lowercase)
				ret[ret.length() - 1] = std::tolower(ret.back());
		}
		if(i < s.length())
            ret += s[i];
	}
	
	return ret;
}

/**
 \brief Remove dynamic modification symbols from peptide sequence \p s.
 
 \param s peptide sequence
 \param lowercase Should modified residue be transformed to lowercase?
 
 \return \p with dynamic modifications removed.
 */
std::string scanData::removeDynamicMod(std::string s, bool lowercase)
{
	//if s does not contain any modifications, just return s
	if(!utils::strContains(scanData::MOD_CHAR, s))
		return s;
	
	std::string ret = "";
	for(size_t i = 0; i < s.length(); i++)
	{
		if(s[i] == scanData::MOD_CHAR){
			if(lowercase)
				ret[ret.length() - 1] = std::tolower(ret.back());
		}
		else ret += s[i];
	}
	
	return ret;
}

std::string scanData::Scan::makeOfSequenceFromSequence(std::string s) const{
	s = removeStaticMod(s);
	s = removeDynamicMod(s);
	return s;
}

void scanData::Scan::initilizeFromLine(std::string line)
{
	std::vector<std::string> elems;
	utils::split(line, IN_DELIM, elems);
	_fullSequence = elems[12];
	_sequence = makeSequenceFromFullSequence(_fullSequence);
	_modified = utils::strContains(MOD_CHAR, _sequence);
	_xcorr = elems[2];
	_spectralCounts = std::stoi(elems[11]);
	
	std::string scanLine = elems[1];
	utils::split(scanLine, '.', elems);
	
	_precursor.setFile(elems[0] + ".ms2");
	_scanNum = std::stoi(elems[1]);
	_precursor.setCharge(std::stoi(elems[3]));
}

/**
 \brief Get unique output file name without extension.
 */
std::string scanData::Scan::getOfNameBase(std::string parentFile, std::string seq) const
{
	std::string ret;
	ret = utils::removeExtension(parentFile);
	ret += ("_" + makeOfSequenceFromSequence(seq) + "_" + std::to_string(_scanNum));
	if(_precursor.getCharge() != 0)
		ret += ("_" + std::to_string(_precursor.getCharge()));
	return ret;
}

/**
 \brief Calls getOfNameBase and adds OF_EXT to end.

 Only added for backwards compatibility.
 */
std::string scanData::Scan::getOfname() const{
	return getOfNameBase(_precursor.getFile(), _sequence) + OF_EXT;
}
