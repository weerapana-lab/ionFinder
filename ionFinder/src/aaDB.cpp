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

void aaDB::AADB::initAADB()
{
    aminoAcidsDB["C_term"]= aaDB::AminoAcid("C_term", 17.00325);
    aminoAcidsDB["N_term"] = aaDB::AminoAcid("N_term", 1.00732);
    aminoAcidsDB["A"] = aaDB::AminoAcid("A", 71.03712);
    aminoAcidsDB["C"] = aaDB::AminoAcid("C", 103.00918);
    aminoAcidsDB["D"] = aaDB::AminoAcid("D", 115.02694);
    aminoAcidsDB["E"] = aaDB::AminoAcid("E", 129.0426);
    aminoAcidsDB["F"] = aaDB::AminoAcid("F", 147.06841);
    aminoAcidsDB["G"] = aaDB::AminoAcid("G", 57.02146);
    aminoAcidsDB["H"] = aaDB::AminoAcid("H", 137.05891);
    aminoAcidsDB["I"] = aaDB::AminoAcid("I", 113.08407);
    aminoAcidsDB["K"] = aaDB::AminoAcid("K", 128.09496);
    aminoAcidsDB["L"] = aaDB::AminoAcid("L", 113.08406);
    aminoAcidsDB["M"] = aaDB::AminoAcid("M", 131.04049);
    aminoAcidsDB["N"] = aaDB::AminoAcid("N", 114.04293);
    aminoAcidsDB["O"] = aaDB::AminoAcid("O", 114.07931);
    aminoAcidsDB["P"] = aaDB::AminoAcid("P", 97.05276);
    aminoAcidsDB["Q"] = aaDB::AminoAcid("Q", 128.05858);
    aminoAcidsDB["R"] = aaDB::AminoAcid("R", 156.10111);
    aminoAcidsDB["S"] = aaDB::AminoAcid("S", 87.03203);
    aminoAcidsDB["T"] = aaDB::AminoAcid("T", 101.04768);
    aminoAcidsDB["V"] = aaDB::AminoAcid("V", 99.06841);
    aminoAcidsDB["W"] = aaDB::AminoAcid("W", 186.07931);
    aminoAcidsDB["Y"] = aaDB::AminoAcid("Y", 163.06333);
    aminoAcidsDB["U"] = aaDB::AminoAcid("U", 150.95309);
    aminoAcidsDB["*"] = aaDB::AminoAcid("*", 0);
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

bool aaDB::AADB::initialize(std::string modDBLoc, bool showWarnings)
{
	//read in files containing aa masses and modifications
	aaDB::aminoAcidsDBType modsTemp;
	if(!readInModDB(modDBLoc, modsTemp))
		return false;
    initAADB();

	//add static mods to aadb
	addStaticMod(modsTemp, showWarnings);
	
	return true;
}

bool aaDB::AADB::initialize(const aaDB::aminoAcidsDBType& mods, bool showWarnings)
{
    initAADB(); // init aaDB
    addStaticMod(mods, showWarnings); //add static mods to aadb
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


