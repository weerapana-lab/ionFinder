//
//  peptide.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 3/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "../include/peptide.hpp"

string peptide::Ion::makeChargeLable() const
{
	if(charge > 0)
		return utils::toString(charge) + "+";
	else if(charge < 0)
		return utils::toString(charge) + "-";
	else if(charge == 1)
		return " +";
	else return utils::toString(charge);
}

string peptide::PeptideIon::makeModLable() const
{
	if(modified)
		return string(1, mod);
	else return "";
}

string peptide::FragmentIon::getIonStr() const
{
	string str;
	str = string(1, b_y) + utils::toString(num) + mod;
	if(charge > 1)
		str += " " + makeChargeLable();
	return str;
}

string peptide::FragmentIon::getFormatedLabel() const
{
	string str = string(1, b_y) + "[" + utils::toString(num) + "]" + mod;
	
	if(charge > 1)
		str += "^\"" + makeChargeLable() + "\"";
	
	return str;
}

template <typename _Tp>
string peptide::FragmentIon::getFormatedLabel(_Tp _num) const
{
	string str = "atop(" + string(1, b_y) + "[" + utils::toString(num) + "]" + mod;
	
	if(charge > 1)
		str += "^\"" + makeChargeLable() + "\"";
	
	str += "," + utils::toString(_num) + ")";
	
	return str;
}

void peptide::Peptide::calcFragments(int minCharge, int maxCharge)
{
	fragments.clear();
	
	size_t len = aminoAcids.size();
	for(int i = 0; i < len; i++)
	{
		for(int j = minCharge; j <= maxCharge; j++)
		{
			string beg = sequence.substr(0, i + 1);
			string end = sequence.substr(i + 1);
			
			peptide::PepIonIt beg_beg = aminoAcids.begin();
			peptide::PepIonIt beg_end = beg_beg + i + 1;
			peptide::PepIonIt end_beg = beg_beg + i;
			peptide::PepIonIt end_end = aminoAcids.end();
			
			//add if statement here to include un-mod fragments
			//maybe some other time
			
			fragments.push_back(FragmentIon('b', i + 1, j,
				peptide::calcMass(aminoAcids, beg_beg, beg_end),
				aminoAcids[i].makeModLable()));
			fragments.push_back(FragmentIon('y', int(sequence.length() - i), j,
				peptide::calcMass(aminoAcids, end_beg, end_end) +
				aminoAcidMasses.getMW("N_term") + aminoAcidMasses.getMW("C_term"),
				aminoAcids[i].makeModLable()));
		}
	}
}

void peptide::Peptide::fixDiffMod()
{
	char mod = '\0';
	bool modFound = false;
	for(size_t i = 0; i < sequence.length(); i++)
	{
		//check that current char is not a mod char
		for(const char* p = DIFFMODS; *p; p++)
			if(sequence[i] == *p)
				throw runtime_error("Invalid peptide sequence!");
		
		//if not at second to last AA search for diff mod
		if((i + 1) < sequence.length())
		{
			//iterate through diffmods
			for(const char* p = DIFFMODS; *p; p++)
			{
				//check if next char in sequence is diff mod char
				if(sequence[i + 1] == *p)
				{
					modFound = true;
					mod = sequence[i + 1];
					sequence.erase(i + 1, 1);
					break;
				}//end of if
			}//end of for
		}//end of if
		PeptideIon temp(aminoAcidMasses.getMW(sequence[i]));
		if(modFound)
			temp.setMod(mod, aminoAcidMasses.getMW(mod));
		aminoAcids.push_back(temp);
		modFound = false;
	}//end of for
}//end of function

double peptide::Peptide::calcMass()
{
	if(!initalized)
		throw runtime_error("Peptide must be initalized to calc mass!");
	
	initalizeFromMass(aminoAcidMasses.calcMW(sequence));
	return getMass();
}

void peptide::Peptide::initalize(const params::Params& pars, bool _calcFragments)
{
	initalized = true;
	if(!aminoAcidMasses.initalize(pars.getAAMassFileLoc(), pars.getSmodFileLoc()))
		throw runtime_error("Error initalzing peptide::Peptide::aminoAcidMasses");
	calcMass();
	fixDiffMod();
	if(_calcFragments)
		calcFragments(pars.getMinFragCharge(), pars.getMaxFragCharge());
}

void peptide::Peptide::printFragments(ostream& out) const
{
	assert(out);
	for(FragmentIonItType it = fragments.begin(); it != fragments.end(); ++it)
		out << it->getIonStr() << ": " << it->getMZ() << endl;
}

double peptide::calcMass(double mz, int charge){
	return mz * charge - charge;
}
double peptide::calcMZ(double mass, int charge){
	return mass / charge + charge;
}

double peptide::calcMass(const peptide::PepIonVecType& vec)
{
	return peptide::calcMass(vec, vec.begin(), vec.end());
}

double peptide::calcMass(const peptide::PepIonVecType& vec, peptide::PepIonIt begin, peptide::PepIonIt end)
{
	double ret = 0;
	for(peptide::PepIonIt it = begin; it != end; ++it)
		ret += it->getTotalMass();
	return ret;
}





