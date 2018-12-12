//
//  peptide.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 3/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <peptide.hpp>

aaDB::AADB* PeptideNamespace::Peptide::aminoAcidMasses = nullptr;
bool PeptideNamespace::Peptide::aminoAcidMassesInitilized = false;

PeptideNamespace::FragmentIon::IonType PeptideNamespace::FragmentIon::strToIonType(std::string s)
{
	if(s == "b")
		return IonType::B;
	else if(s == "y")
		return IonType::Y;
	else if(utils::startsWith(s, "b-"))
		return IonType::B_NL;
	else if(utils::startsWith(s, "y-"))
		return IonType::Y_NL;
	else throw std::runtime_error("Unknown IonType!");
}

std::string PeptideNamespace::FragmentIon::ionTypeToStr() const
{
	switch(_ionType){
		case IonType::B : return "b";
			break;
		case IonType::Y : return "y";
			break;
		case IonType::B_NL : return "b-";
			break;
		case IonType::Y_NL : return "y-";
			break;
	}
}

std::string PeptideNamespace::Ion::makeChargeLable() const
{
	if(charge > 0)
		return std::to_string(charge) + "+";
	else if(charge < 0)
		return std::to_string(charge) + "-";
	else if(charge == 1)
		return " +";
	else return std::to_string(charge);
}

std::string PeptideNamespace::PeptideIon::makeModLable() const
{
	if(modified)
		return std::string(1, mod);
	else return "";
}

std::string PeptideNamespace::FragmentIon::getIonStr() const
{
	std::string str;
	str =std::string(1, b_y) + std::to_string(num) + mod;
	if(charge > 1)
		str += " " + makeChargeLable();
	return str;
}

std::string PeptideNamespace::FragmentIon::getFormatedLabel() const
{
	std::string str =std::string(1, b_y) + "[" + std::to_string(num) + "]";
	
	if(!mod.empty())
		str += " *\"" + mod + "\"";
	
	if(charge > 1)
		str += "^\"" + makeChargeLable() + "\"";
	
	return str;
}

void PeptideNamespace::Peptide::calcFragments(int minCharge, int maxCharge)
{
	fragments.clear();
	
	double nTerm = aminoAcidMasses->getMW("N_term");
	double cTerm = aminoAcidMasses->getMW("C_term");
	double hMass = aminoAcidMasses->getMW("H_mass");
	
	size_t len = aminoAcids.size();
	for(int i = 0; i < len; i++)
	{
		std::string beg = sequence.substr(0, i + 1);
		std::string end = sequence.substr(i + 1);
		
		PeptideNamespace::PepIonIt beg_beg = aminoAcids.begin();
		PeptideNamespace::PepIonIt beg_end = beg_beg + i + 1;
		PeptideNamespace::PepIonIt end_beg = beg_beg + i;
		PeptideNamespace::PepIonIt end_end = aminoAcids.end();
		
		std::string modsB = PeptideNamespace::concatMods(aminoAcids, beg_beg, beg_end);
		std::string modsY = PeptideNamespace::concatMods(aminoAcids, end_beg, end_end);
		
		for(int j = minCharge; j <= maxCharge; j++)
		{
			//TODO: add if statement here to include un-mod fragments
			//maybe some other time
			
			fragments.push_back(FragmentIon('b', i + 1, j,
				PeptideNamespace::calcMass(aminoAcids, beg_beg, beg_end) + nTerm, 0.0,
				modsB));
			fragments.push_back(FragmentIon('y', int(sequence.length() - i), j,
				PeptideNamespace::calcMass(aminoAcids, end_beg, end_end) + hMass * 2 + cTerm, 0.0,
				modsY));
		}
	}
	/*for(auto it = fragments.begin(); it != fragments.end(); ++it)
	{
		std::cout << it->getFormatedLabel() << '\t' << it->getMass() << '\t';
		++it;
		std::cout << it->getFormatedLabel() << '\t' << it->getMass() << '\n';
	}*/
}

void PeptideNamespace::Peptide::addNeutralLoss(const std::vector<double>& losses)
{
	size_t len = fragments.size();
	for(size_t i = 0; i < len; i++)
	{
		for(auto it2 = losses.begin(); it2 != losses.end(); ++it2)
		{
			int tempCharge = fragments[i].getCharge();
			
			//get new fragment type
			PeptideNamespace::FragmentIon::IonType ionType;
			if(fragments[i].getBY() == 'b')
				ionType = PeptideNamespace::FragmentIon::IonType::B_NL;
			else if(fragments[i].getBY() == 'y')
				ionType = PeptideNamespace::FragmentIon::IonType::Y_NL;
			else throw std::runtime_error("Unknown ion type!");
				
			fragments.push_back(FragmentIon(ionType, fragments[i].getNum(), tempCharge,
											fragments[i].getMass() - (*it2 / tempCharge),
											*it2 * -1, //losses are given positive. Convert to negative num here.
											fragments[i].getMod()));
		}
	}
}

void PeptideNamespace::Peptide::fixDiffMod(const char* diffmods)
{
	char mod = '\0';
	bool modFound = false;
	for(size_t i = 0; i < sequence.length(); i++)
	{
		//check that current char is not a mod char
		for(const char* p = diffmods; *p; p++)
			if(sequence[i] == *p)
				throw std::runtime_error("Invalid peptide sequence!");
		
		//if not at second to last AA, search for diff mod
		if((i + 1) < sequence.length())
		{
			//iterate through diffmods
			for(const char* p = diffmods; *p; p++)
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
		PeptideIon temp(aminoAcidMasses->getMW(sequence[i]));
		if(modFound){
			temp.setMod(mod, aminoAcidMasses->getMW(mod));
			nMod++;
		}
		aminoAcids.push_back(temp);
		modFound = false;
	}//end of for
}//end of function

double PeptideNamespace::Peptide::calcMass()
{
	if(!initalized)
		throw std::runtime_error("Peptide must be initalized to calc mass!");
	
	initalizeFromMass(aminoAcidMasses->calcMW(sequence));
	return getMass();
}

void PeptideNamespace::Peptide::initAminoAcidsMasses(const base::ParamsBase& pars)
{
	if(pars.getSeqParSpecified())
	{
		seqpar::SequestParamsFile spFile(pars.getSeqParFname());
		
		if(!aminoAcidMassesInitilized)
		{
			if(!spFile.read())
				throw std::runtime_error("Could not read sequest params file!");
			
			aminoAcidMasses = new aaDB::AADB;
			if(!aminoAcidMasses->initalize(pars.getAAMassFileLoc(), spFile.getAAMap()))
				throw std::runtime_error("Error initalzing peptide::Peptide::aminoAcidMasses");
		}
	}
	else {
		if(!aminoAcidMassesInitilized)
		{
			aminoAcidMasses = new aaDB::AADB;
			if(!aminoAcidMasses->initalize(pars.getAAMassFileLoc(), pars.getSmodFileLoc()))
				throw std::runtime_error("Error initalzing peptide::Peptide::aminoAcidMasses");
		}
	}
	aminoAcidMassesInitilized = true;
}

void PeptideNamespace::Peptide::initalize(const base::ParamsBase& pars, bool _calcFragments)
{
	initalized = true;
	
	//initilize aminoAcidMasses if necissary
	if(!aminoAcidMassesInitilized)
		initAminoAcidsMasses(pars);
	
	calcMass();
	fixDiffMod();
	if(_calcFragments)
		calcFragments(pars.getMinFragCharge(), pars.getMaxFragCharge());
}

void PeptideNamespace::Peptide::printFragments(std::ostream& out) const
{
	assert(out);
	for(FragmentIonItType it = fragments.begin(); it != fragments.end(); ++it)
		out << it->getIonStr() << ": " << it->getMZ() <<NEW_LINE;
}

double PeptideNamespace::calcMass(double mz, int charge){
	return mz * charge - charge;
}
double PeptideNamespace::calcMZ(double mass, int charge){
	return mass / charge + charge;
}

double PeptideNamespace::calcMass(const PeptideNamespace::PepIonVecType& vec)
{
	return PeptideNamespace::calcMass(vec, vec.begin(), vec.end());
}

double PeptideNamespace::calcMass(const PeptideNamespace::PepIonVecType& vec,
								  PeptideNamespace::PepIonIt begin, PeptideNamespace::PepIonIt end)
{
	double ret = 0;
	for(PeptideNamespace::PepIonIt it = begin; it != end; ++it)
		ret += it->getTotalMass();
	return ret;
}

/**
 Returns a string containing all the modifications in vec.
 @param vec vector of PeptideIon(s)
 @param begin iterator to start of vec
 @param end iterator to end of vec
 */
std::string PeptideNamespace::concatMods(const PeptideNamespace::PepIonVecType& vec,
								PeptideNamespace::PepIonIt begin, PeptideNamespace::PepIonIt end)
{
	std::string ret = "";
	for(PeptideNamespace::PepIonIt it = begin; it != end; ++it)
		if(it->isModified())
			ret += it->getMod();
	return ret;
}

