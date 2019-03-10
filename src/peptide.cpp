//
//  peptide.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 3/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <peptide.hpp>

/**
 \brief Convert string to IonType <br>
 Accepted options are "b", "y", "m", "M", "^y-", "^b-", and "^M-".
 All others will cause an error.
 \return IonType
 */
PeptideNamespace::FragmentIon::IonType PeptideNamespace::FragmentIon::strToIonType(std::string s)
{
	if(s == "b")
		return IonType::B;
	else if(s == "y")
		return IonType::Y;
	else if(s == "m" || s == "M")
		return IonType::M;
	else if(utils::startsWith(s, "b-"))
		return IonType::B_NL;
	else if(utils::startsWith(s, "y-"))
		return IonType::Y_NL;
	else if(utils::startsWith(s, "m-") || utils::startsWith(s, "M-"))
		return IonType::M_NL;
	else throw std::runtime_error("Unknown IonType!");
}

/**
 Get neutral loss as string rounded to nearest integer.
 @return neutral loss
 */
std::string PeptideNamespace::FragmentIon::getNLStr() const{
	return std::string((_nlMass < 1 ? "" : "+")) + std::to_string((int)round(_nlMass));
}

std::string PeptideNamespace::FragmentIon::ionTypeToStr() const
{
	switch(_ionType){
		case IonType::B : return "b";
			break;
		case IonType::Y : return "y";
			break;
		case IonType::M : return "M";
			break;
		case IonType::B_NL : return "b_nl"; // + getNLStr();
			break;
		case IonType::Y_NL : return "y_nl"; // + getNLStr();
			break;
		case IonType::M_NL : return "M_nl";
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

std::string PeptideNamespace::AminoAcid::makeModLable() const
{
	if(modified)
		return mod;
	else return "";
}

/**
 Get ion label.
 @return unformatted ion label
 */
std::string PeptideNamespace::FragmentIon::getLabel(bool includeMod, std::string chargeSep) const
{
	std::string str = std::string(1, _b_y);
	str += isM() ? "" : std::to_string(_num); //add ion number if not M ion
	str += includeMod ? _mod : ""; //add modification
	
	if(charge > 1)
		str += chargeSep + makeChargeLable();
	if(isNL())
		str += getNLStr();
	return str;
}

/**
 Format label with markup for ggplot ms2 spectrum.
 @return formatted ion label
 */
std::string PeptideNamespace::FragmentIon::getFormatedLabel() const
{
	std::string str = std::string(1, _b_y) + (isM() ? "" : "[" + std::to_string(_num) + "]");
	
	if(!_mod.empty())
		str += " *\"" + _mod + "\"";
	
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
			//add b ion
			fragments.push_back(FragmentIon('b', i + 1, j,
				PeptideNamespace::calcMass(aminoAcids, beg_beg, beg_end) + nTerm, 0.0, 0,
				modsB));
			
			//add y ion
			if(i == 0){
				fragments.push_back(FragmentIon('M', 0, j,
					PeptideNamespace::calcMass(aminoAcids, end_beg, end_end) + PeptideNamespace::H_MASS + cTerm, 0.0, 0,
					modsY));
			}
			else{
				fragments.push_back(FragmentIon('y', int(sequence.length() - i), j,
					PeptideNamespace::calcMass(aminoAcids, end_beg, end_end) + PeptideNamespace::H_MASS + cTerm, 0.0, 0,
					modsY));
			}//end of else
		}//end of for j
	}//enf of for i
	
	/*if(sequence == "SSGIHYGVITCEGCK"){
		std::cout << NEW_LINE << NEW_LINE;
		std::streamsize ss = std::cout.precision();
		std::cout.precision(5);
		for(int i = minCharge; i <= maxCharge; i++)
		{
			for(auto it = fragments.begin(); it != fragments.end(); ++it)
			{
				if(it->getCharge() == i)
				{
					std::cout << it->getLabel(false, "_") <<
					'\t' << std::fixed << it->getMZ() << '\t';
					++it;
					std::cout << std::fixed << it->getLabel(false, "_") <<
					'\t' << it->getMZ() << '\n';
				}
			}
		}
		std::cout.precision(ss);
		std::cout << "POOP" << NEW_LINE;
	}*/
}

/**
 \brief Add neutral loss fragment ions to Peptide <br>
 Neutral loss ions for each b and y ion are added to Peptide for
 each mass in \p losses.
 
 \param lossMass mass of neutral loss to add
 */
void PeptideNamespace::Peptide::addNeutralLoss(double lossMass)
{
	//calculate neutral loss combinations
	std::vector<double> neutralLossIons;
	for(int i = 1; i <= nMod; i++)
		neutralLossIons.push_back(i * lossMass);
	
	size_t len = fragments.size();
	size_t nLosses = neutralLossIons.size();
	for(size_t i = 0; i < len; i++)
	{
		for(size_t j = 0; j < nLosses; j++)
		{
			int tempCharge = fragments[i].getCharge();
			
			//get new fragment type
			PeptideNamespace::FragmentIon::IonType ionType;
			if(fragments[i].getBY() == 'b')
				ionType = PeptideNamespace::FragmentIon::IonType::B_NL;
			else if(fragments[i].getBY() == 'y')
				ionType = PeptideNamespace::FragmentIon::IonType::Y_NL;
			else if(fragments[i].getBY() == 'M' || fragments[i].getBY() == 'm')
				ionType = PeptideNamespace::FragmentIon::IonType::M_NL;
			else throw std::runtime_error("Unknown ion type!");
			
			fragments.push_back(FragmentIon(ionType, fragments[i].getNum(), tempCharge,
									fragments[i].getMass() - (neutralLossIons[j] / tempCharge),
									neutralLossIons[j] * -1, //losses are given positive.
															 //Convert to negative num here
									j + 1, fragments[i].getMod()));
		}
	}
}

void PeptideNamespace::Peptide::fixDiffMod(const aaDB::AADB& aminoAcidsMasses,
										   const char* diffmods)
{
	std::string mod = "";
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
		AminoAcid temp(aminoAcidsMasses.getMW(sequence[i]));
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
		throw std::runtime_error("Attempting to initialize Peptide with an empty AADB!");
	
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
	size_t len = fragments.size();
	for(size_t i = 0; i < len; i++)
		out << i << ") " << fragments[i].getLabel() << ": " << fragments[i].getMZ() << NEW_LINE;
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
	return mz * charge - (charge * PeptideNamespace::H_MASS);
}
double PeptideNamespace::calcMZ(double mass, int charge){
	return (mass + (charge * PeptideNamespace::H_MASS)) / charge;
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
 Returns a string containing all the modifications if interest in vec.
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

