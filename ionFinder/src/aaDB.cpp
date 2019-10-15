//
//  aaDB.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/6/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <aaDB.hpp>

aaDB::AminoAcid::AminoAcid(std::string line)
{
	size_t endOfLine = line.find(";");
	line = line.substr(0, endOfLine);
	line = utils::trim(line);
	
	std::vector<std::string> elems;
	utils::split(line, '\t', elems);
	
	symbol = elems[0];
	mass = std::stod(elems[1]);
	modification = 0;
}

bool aaDB::AADB::readInAADB(std::string _aaDBLoc)
{
	std::ifstream inF(_aaDBLoc);
	if(!inF){
		std::cerr << "Could not open file: " << _aaDBLoc << NEW_LINE;
		return false;
	}
	
	std::string line;
	while(utils::safeGetline(inF, line)){
		line = utils::trim(line);
		if(utils::isCommentLine(line) || line.empty())
			continue;
		aaDB::AminoAcid temp(line);
		aminoAcidsDB[temp.getSymbol()] = temp;
	}
	
	
	return true;
}

bool aaDB::AADB::readInModDB(std::string _modDBLoc, aaDB::aminoAcidsDBType& modsTemp)
{
	std::ifstream inF(_modDBLoc);
	if(!inF) return false;
	
	//std::cerr << _modDBLoc << std::endl;
	
	std::string line;
	
	while(inF && line != aaDB::SMOD_END_TAG)
	{
		utils::safeGetline(inF, line);
		line = utils::trim(line);
		if(utils::isCommentLine(line) || line.empty())
			continue;
		
		while(utils::safeGetline(inF, line))
		{
			line = utils::trim(line);
			if(utils::isCommentLine(line) || line.empty())
				continue;
			aaDB::AminoAcid temp (line);
			modsTemp[temp.getSymbol()] = temp;
		}//end of while
	}
	return true;
}//end of function

void aaDB::AADB::addStaticMod(const aaDB::aminoAcidsDBType& modsTemp, bool showWarnings)
{
	for(aaDB::AADB::itType it = modsTemp.begin(); it != modsTemp.end(); ++it)
	{
		std::string tempSymbol = it->second.getSymbol();
		//check that item from modsTemp exists in aadb
		if(aminoAcidsDB.find(tempSymbol) == aminoAcidsDB.end())
		{
			if(showWarnings)
				std::cerr << "Could not find " << tempSymbol << " in aaDB!" << NEW_LINE;
			continue;
		} else {
			aminoAcidsDB[tempSymbol] += it->second.getMass();
		}
	}//end for
}//end function

bool aaDB::AADB::initialize(std::string aaDBLoc, std::string modDBLoc, bool showWarnings)
{
	//read in files containing aa masses and modifications
	aaDB::aminoAcidsDBType modsTemp;
	if(!readInModDB(modDBLoc, modsTemp) || !readInAADB(aaDBLoc))
		return false;
	
	//add static mods to aadb
	addStaticMod(modsTemp, showWarnings);
	
	return true;
}

bool aaDB::AADB::initialize(std::string aaDBLoc, const aaDB::aminoAcidsDBType& mods, bool showWarnings)
{
	//read in files containing aa masses and modifications
	if(!readInAADB(aaDBLoc))
		return false;
	
	//add static mods to aadb
	addStaticMod(mods, showWarnings);
	
	return true;
}


double aaDB::AADB::getMW(std::string aa) const
{
	aaDB::AADB::itType it = aminoAcidsDB.find(aa);
	if(it == aminoAcidsDB.end())
		return -1;
	return it->second.getMass();
}

double aaDB::AADB::getMW(char aa) const
{
	return getMW(std::string(1, aa));
}

double aaDB::AADB::calcMW(std::string sequence, bool addNTerm, bool addCTerm) const
{
	double mass = 0;
	size_t len = sequence.length();
	double temp;
	
	if(addNTerm)
	{
		temp = getMW("N_term");
		if(temp == -1)
			return -1;
		mass += temp;
	}
	
	for(int i = 0; i < len; i++)
	{
		temp = getMW(sequence[i]);
		if(temp == -1)
			return -1;
		mass += temp;
	}
	if(addCTerm)
	{
		temp = getMW("C_term");
		if(temp == -1)
			return -1;
		mass += temp;
	}
	
	return mass;
}


