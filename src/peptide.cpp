//
//  peptide.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 3/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <peptide.hpp>

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

std::string PeptideNamespace::FragmentIon::getNLStr() const{
	return std::string((nlMass < 1 ? "" : "+")) + std::to_string((int)round(nlMass));
}

std::string PeptideNamespace::FragmentIon::ionTypeToStr() const
{
	switch(_ionType){
		case IonType::B : return "b";
			break;
		case IonType::Y : return "y";
			break;
		case IonType::B_NL : return "b_nl"; // + getNLStr();
			break;
		case IonType::Y_NL : return "y_nl"; // + getNLStr();
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

std::string PeptideNamespace::FragmentIon::getIonStr(bool includeMod) const
{
	std::string str = std::string(1, b_y) + std::to_string(num) + (includeMod ? mod : "");
	if(charge > 1)
		str += " " + makeChargeLable();
	if(isNL())
		str += getNLStr();
	return str;
}

std::string PeptideNamespace::FragmentIon::getFormatedLabel() const
{
	std::string str =std::string(1, b_y) + "[" + std::to_string(num) + "]";
	
	if(!mod.empty())
		str += " *\"" + mod + "\"";
	
	if(charge > 1)
		str += "^\"" + makeChargeLable() + "\"";
	
	if(isNL())
		str += getNLStr();
	
	return str;
}

void PeptideNamespace::Peptide::calcFragments(int minCharge, int maxCharge,
											  const aaDB::AADB& aminoAcidsMasses)
{
	fragments.clear();
	
	double nTerm = aminoAcidsMasses.getMW("N_term");
	double cTerm = aminoAcidsMasses.getMW("C_term");
	double hMass = aminoAcidsMasses.getMW("H_mass");
	
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
				PeptideNamespace::calcMass(aminoAcids, end_beg, end_end) + hMass + cTerm, 0.0,
				modsY));
		}
	}
	/*for(auto it = fragments.begin(); it != fragments.end(); ++it)
	{
		std::cout << it->getIonStr() << '\t' << it->getMZ() << '\t';
		++it;
		std::cout << it->getIonStr() << '\t' << it->getMZ() << '\n';
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

void PeptideNamespace::Peptide::fixDiffMod(const aaDB::AADB& aminoAcidsMasses,
										   const char* diffmods)
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
		PeptideIon temp(aminoAcidsMasses.getMW(sequence[i]));
		if(modFound){
			temp.setMod(mod, aminoAcidsMasses.getMW(mod));
			nMod++;
		}
		aminoAcids.push_back(temp);
		modFound = false;
	}//end of for
}//end of function

double PeptideNamespace::Peptide::calcMass(const aaDB::AADB& aadb)
{
	if(!initialized)
		throw std::runtime_error("Peptide must be initialized to calc mass!");
	
	initalizeFromMass(aadb.calcMW(sequence));
	return getMass();
}

void PeptideNamespace::initAminoAcidsMasses(const base::ParamsBase& pars,
										    std::string seqParFname, aaDB::AADB& aadb)
{
	seqpar::SequestParamsFile spFile(seqParFname);
	
	if(!spFile.read())
		throw std::runtime_error("Could not read sequest params file!");
	
	if(!aadb.initialize(pars.getAAMassFileLoc(), spFile.getAAMap()))
		throw std::runtime_error("Error initalzing peptide::Peptide::aminoAcidMasses");
}

void PeptideNamespace::initAminoAcidsMasses(const base::ParamsBase& pars, aaDB::AADB& aadb)
{
	if(pars.getSeqParSpecified())
	{
		seqpar::SequestParamsFile spFile(pars.getSeqParFname());
		
		if(!spFile.read())
			throw std::runtime_error("Could not read sequest params file!");
		
		if(!aadb.initialize(pars.getAAMassFileLoc(), spFile.getAAMap()))
			throw std::runtime_error("Error initalzing peptide::Peptide::aminoAcidMasses");
	}
	else {
		if(!aadb.initialize(pars.getAAMassFileLoc(), pars.getSmodFileLoc()))
			throw std::runtime_error("Error initalzing peptide::Peptide::aminoAcidMasses");
	}
}

void PeptideNamespace::Peptide::initialize(const base::ParamsBase& pars,
										   const aaDB::AADB& aadb,
										   bool _calcFragments)
{
	if(aadb.empty())
		throw std::runtime_error("Atempting to initalize Peptide with an empty AADB!");
	
	initialized = true;
	calcMass(aadb);
	fixDiffMod(aadb);
	if(_calcFragments)
		calcFragments(pars.getMinFragCharge(), pars.getMaxFragCharge(), aadb);
}

/**
 Prints peptide fragmetns and calculated MZs to out.
 For debugging
 @param out stream to print to.
 */
void PeptideNamespace::Peptide::printFragments(std::ostream& out) const
{
	assert(out);
	for(FragmentIonItType it = fragments.begin(); it != fragments.end(); ++it)
		out << it->getIonStr() << ": " << it->getMZ() << NEW_LINE;
}

/**
 Removes all unlabeled fragment ions from fragments.
 For debugging.
 */
void PeptideNamespace::Peptide::removeUnlabeledFrags()
{
	for(FragmentIonItType it = fragments.begin(); it != fragments.end();)
	{
		if(!it->getFound())
			fragments.erase(it);
		else ++it;
	}
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

/**
 Returns sum of masss of amino acids in vec.
 @param vec vector of PeptideIon(s)
 @param begin iterator to start of vec
 @param end iterator to end of vec
 */
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

