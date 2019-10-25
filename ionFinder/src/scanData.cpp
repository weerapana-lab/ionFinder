//
//  scanData.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/23/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <scanData.hpp>

void scanData::PrecursorScan::clear()
{
    _mz.clear();
    _scan.clear();
    _rt = 0;
    _file.clear();
    _charge = 0;
    _intensity = 0;
}

void scanData::Scan::clear()
{
	_scanNum = 0;
	_sequence.clear();
	_fullSequence.clear();
	_modified = false;
	_spectralCounts = 0;
	_precursor.clear();
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
 
 \return \s with static modifications removed.
 */
std::string scanData::removeStaticMod(std::string s, bool lowercase)
{
	//if s does not contain any modifications, just return s
	if(!(utils::strContains('(', s) ||
		 utils::strContains(')', s)))
		return s;
	
	std::string ret = "";
	for(int i = 0; i < s.length(); i++)
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
		ret += s[i];
	}
	
	return ret;
}

/**
 \brief Remove dynamic modification symbols from peptide sequence \p s.
 
 \param s peptide sequence
 \param lowercase Should modified residue be transformed to lowercase?
 
 \return \s with dynamic modifications removed.
 */
std::string scanData::removeDynamicMod(std::string s, bool lowercase)
{
	//if s does not contain any modifications, just return s
	if(!utils::strContains(scanData::MOD_CHAR, s))
		return s;
	
	std::string ret = "";
	for(int i = 0; i < s.length(); i++)
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

const scanData::PrecursorScan& scanData::Scan::getPrecursor() const {
    return _precursor;
}

scanData::PrecursorScan& scanData::Scan::getPrecursor() {
    return _precursor;
}

const std::string &scanData::PrecursorScan::getMZ() const {
    return _mz;
}

void scanData::PrecursorScan::setMZ(const std::string &mz) {
    _mz = mz;
}

const std::string &scanData::PrecursorScan::getScan() const {
    return _scan;
}

void scanData::PrecursorScan::setScan(const std::string &scan) {
    _scan = scan;
}

double scanData::PrecursorScan::getRT() const {
    return _rt;
}

void scanData::PrecursorScan::setRT(const double rt) {
    _rt = rt;
}

const std::string &scanData::PrecursorScan::getFile() const {
    return _file;
}

void scanData::PrecursorScan::setFile(const std::string &file) {
    _file = file;
}

void scanData::PrecursorScan::setCharge(int rhs) {
    _charge = rhs;
}

int scanData::PrecursorScan::getCharge() const {
    return _charge;
}

double scanData::PrecursorScan::getIntensity() const {
    return 0;
}

void scanData::PrecursorScan::setIntensity(double rhs) {
    _intensity = rhs;
}
