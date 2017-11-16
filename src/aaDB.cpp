//
//  aaDB.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/6/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "aaDB.hpp"

aaDB::AminoAcid::AminoAcid(string line)
{
	size_t endOfLine = line.find(";");
	line = line.substr(0, endOfLine);
	line = utils::trim(line);
	
	vector<string> elems;
	utils::split(line, '\t', elems);
	
	symbol = elems[0];
	mass = utils::toDouble(elems[1]);
	modification = 0;
}

bool aaDB::AADB::readInAADB(string _aaDBLoc)
{
	utils::File file (_aaDBLoc);
	if(!file.read())
		return false;
	
	string line;
	do{
		line = file.getLine_skip_trim();
		aaDB::AminoAcid temp(line);
		aminoAcidsDB[temp.getSymbol()] = temp;
		
	} while(!file.end());
	
	
	return true;
}

bool aaDB::AADB::readInModDB(string _modDBLoc, aaDB::AADB::aminoAcidsDBType& modsTemp)
{
	utils::File file(_modDBLoc);
	if(!file.read())
		return false;
	
	string line;
	int i = 0;
	
	do{
		line = file.getLine_skip_trim();
		if(line == aaDB::SMOD_BEGIN_TAG)
		{
			do{
				line = file.getLine_skip_trim();
				if(line != aaDB::SMOD_END_TAG)
				{
					aaDB::AminoAcid temp (line);
					modsTemp[temp.getSymbol()] = temp;
					if(++i > aaDB::MAX_PARAM_ITERATIONS)
						return false;
				}
			} while(line != aaDB::SMOD_END_TAG);
		}
	} while(!file.end() && line != aaDB::SMOD_END_TAG);
	return true;
}//end of function

void aaDB::AADB::addStaticMod(const aaDB::AADB::aminoAcidsDBType& modsTemp, bool showWarnings)
{
	for(aaDB::AADB::itType it = modsTemp.begin(); it != modsTemp.end(); ++it)
	{
		string tempSymbol = it->second.getSymbol();
		//check that item from modsTemp exists in aadb
		if(aminoAcidsDB.find(tempSymbol) == aminoAcidsDB.end())
		{
			if(showWarnings)
				cout << "Could not find " << tempSymbol << " in aaDB!" << endl;
			continue;
		} else {
			aminoAcidsDB[tempSymbol] += it->second.getMass();
		}
	}//end for
}//end function

bool aaDB::AADB::initalize(string aaDBLoc, string modDBLoc, bool showWarnings)
{
	//read in files containing aa masses and modifications
	aaDB::AADB::aminoAcidsDBType modsTemp;
	if(!readInModDB(modDBLoc, modsTemp) || !readInAADB(aaDBLoc))
		return false;
	
	//add static mods to aadb
	addStaticMod(modsTemp, showWarnings);
	
	return true;
}

double aaDB::AADB::getMW(string aa) const
{
	aaDB::AADB::itType it = aminoAcidsDB.find(aa);
	if(it == aminoAcidsDB.end())
		return -1;
	return it->second.getMass();
}

double aaDB::AADB::getMW(char aa) const
{
	return getMW(string(1, aa));
}

double aaDB::AADB::calcMW(string sequence, bool addNTerm, bool addCTerm) const
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


