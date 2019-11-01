//
//  peptide.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 3/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <peptide.hpp>

unsigned int PeptideNamespace::Peptide::obj_count = 0;

PeptideNamespace::Species::Species(const PeptideNamespace::Species& rhs){
    mass = rhs.mass;
}

PeptideNamespace::Ion::Ion(const PeptideNamespace::Ion & rhs) : PeptideNamespace::Species::Species(rhs) {
    charge = rhs.charge;
}

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
 \return neutral loss
 */
std::string PeptideNamespace::FragmentIon::getNLStr() const{
	return std::string((_nlMass < 1 ? "" : "+")) + std::to_string((int)round(_nlMass));
}

/**
 \brief Given a peptide sequence, initialize the _beg, _end, and _sequence members.
 
 \param sequence Full peptide sequence.
 */
void PeptideNamespace::FragmentIon::_initFragSpan(const std::string& sequence)
{
	int len = int(sequence.length());
	
	if(_b_y == 'b')
	{
		_beg = 0;
		_end = _num - 1;
	}
	else if(_b_y == 'y')
	{
		_beg = len - _num;
		_end = len - 1;
	}
	else if(_b_y == 'M' || _b_y == 'm')
	{
		_beg = 0;
		_end = len - 1;
	}
	else throw std::runtime_error("Unknown IonType!");
	_sequence = std::string(sequence.begin() + _beg, sequence.begin() + _end + 1);
	//std::cout << _sequence << std::endl;
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

/**
 Get charge label based off sign of Ion::charge.
 */
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

/**
 Add \p modMass to AminoAcid::_modMass
 \param modMass mass to add
 */
void PeptideNamespace::AminoAcid::_addMod(double modMass){
	_modMass += modMass;
}

/**
 Set dynamic modification for AminoAcid.
 \param mod symbol for modification to add.
 \param modMass mass to add
 */
void PeptideNamespace::AminoAcid::setDynamicMod(char mod, double modMass){
	_mod = mod;
	_dynamicMod = true;
	_addMod(modMass);
}

/**
 Add statid modification to AminoAcid.
 \param modMass mass to add
 */
void PeptideNamespace::AminoAcid::addStaticMod(double modMass)
{
	_staticMod = true;
	_addMod(modMass);
}

PeptideNamespace::AminoAcid::AminoAcid(double mass) : Ion() {
    initalizeFromMass(mass);
    _staticMod = false;
    _dynamicMod = false;
    _modMass = 0;
    _mod = '\0';
}

/**
 Get ion label.
 \return unformatted ion label
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
 \return formatted ion label
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
	
//	if(sequence == "GPDFGYVTR")
//		std::cout << "Found!" << std::endl;
	
	double nTerm = aminoAcidsMasses.getMW("N_term");
	double cTerm = aminoAcidsMasses.getMW("C_term");
	
	const PeptideNamespace::PepIonIt endIt = aminoAcids.end();
	const PeptideNamespace::PepIonIt begIt = aminoAcids.begin();
	size_t beg_end, end_beg;
	
	size_t len = aminoAcids.size();
	for(size_t i = 0; i < len; i++)
	{
		beg_end = i + 1;
		end_beg = i;
		
		std::string modsB = PeptideNamespace::concatMods(begIt, begIt + beg_end);
		std::string modsY = PeptideNamespace::concatMods(begIt + end_beg, endIt);
		
		std::string ySeq = "";
		std::string bSeq = "";
		
		for(int j = minCharge; j <= maxCharge; j++)
		{
			//add b ion
			fragments.push_back(FragmentIon('b', //b_y
											i + 1, //num
											j, //charge
											PeptideNamespace::calcMass(begIt, begIt + beg_end) + nTerm, //mass
											modsB, //mod
											sequence) //pepSequence
								);
			
			//add y ion
			if(i == 0){
				fragments.push_back(FragmentIon('M', //b_y
												0, //num
												j, //charge
												PeptideNamespace::calcMass(begIt + end_beg, endIt) + PeptideNamespace::H_MASS + cTerm, //mass
												modsY, //mod
												sequence) //pepSequence
									);
			}
			else{
				fragments.push_back(FragmentIon('y', //b_y
												int(sequence.length() - i), //num
												j, //charge
												PeptideNamespace::calcMass(begIt + end_beg, endIt) + PeptideNamespace::H_MASS + cTerm, //mass
												modsY ,//mod
												sequence) //pepSequence
									);
			}//end of else
		}//end of for j
	}//enf of for i
	
	/*if(sequence == "DLAAFDKSHDQAVRTYQEHK"){
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

size_t PeptideNamespace::Peptide::nModsInSpan(size_t beg, size_t end) const
{
	size_t ret = 0;
	for(auto it = modLocs.begin(); it != modLocs.end(); ++it)
		if(utils::inSpan(beg, end, *it))
			ret++;
	return ret;
}

/**
 Create a new FragmentIon object with the corresponding \p lossMass neutral loss.
 
 \param lossMass Mass of neutral loss given as a positive number.
 \param numNL Multlipicity of neutral loss.
 \return New PeptideNamespace::FragmentIon with specified \p lossMass.
 */
PeptideNamespace::FragmentIon PeptideNamespace::FragmentIon::makeNLFrag(double lossMass,
																		size_t numNL) const
{
	PeptideNamespace::FragmentIon ret = FragmentIon(*this);
	
	//get new fragment type
	PeptideNamespace::FragmentIon::IonType ionType;
	if(getBY() == 'b')
		ionType = PeptideNamespace::FragmentIon::IonType::B_NL;
	else if(getBY() == 'y')
		ionType = PeptideNamespace::FragmentIon::IonType::Y_NL;
	else if(getBY() == 'M' || getBY() == 'm')
		ionType = PeptideNamespace::FragmentIon::IonType::M_NL;
	else throw std::runtime_error("Unknown ion type!");
	ret.setIonType(ionType);
	
	//add mass and nlMass
	ret.mass = mass - (lossMass / charge);
	ret._nlMass = -1 * lossMass;
	ret._numNl = numNL;

	return ret;
}

PeptideNamespace::FragmentIon::FragmentIon(char b_y, int num, int charge, double mass, std::string mod,
                                           const std::string &pepSequence) : Ion() {
    _b_y = b_y;
    _num = num;
    _mod = mod;
    _nlMass = 0;
    _numNl = 0;
    initalizeFromMass(mass, charge);
    _found = false;
    _ionType = strToIonType(b_y);
    _initFragSpan(pepSequence);
    _includeLabel = true;
    _foundMZ = 0;
    _foundIntensity = 0;
}

//!copy constructor
PeptideNamespace::FragmentIon::FragmentIon(const PeptideNamespace::FragmentIon &rhs) : Ion(rhs) {
    _b_y = rhs._b_y;
    _num = rhs._num;
    _mod = rhs._mod;
    _found = rhs._found;
    _ionType = rhs._ionType;
    _nlMass = rhs._nlMass;
    _numNl = rhs._numNl;
    charge = rhs.charge;
    mass = rhs.mass;
    _sequence = rhs._sequence;
    _beg = rhs._beg;
    _end = rhs._end;
    _includeLabel = rhs._includeLabel;
    _foundMZ = rhs._foundMZ;
    _foundIntensity = rhs._foundIntensity;
}

/**
 \brief Add neutral loss fragment ions to Peptide <br>
 Neutral loss ions for each b and y ion are added to Peptide for
 each mass in \p losses.
 
 \param lossMass mass of neutral loss to add
 \param labelDecoyNL Should artifact neutral loss ions be labeled in spectra?
 */
void PeptideNamespace::Peptide::addNeutralLoss(double lossMass, bool labelDecoyNL)
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
			fragments.push_back(fragments[i].makeNLFrag(neutralLossIons[j], j + 1));
			
			//calc forceLabel
			if(!labelDecoyNL){
				size_t modCount_temp = nModsInSpan(fragments.back().getBegin(), fragments.back().getEnd());
				bool forceLabel = modCount_temp == fragments.back().getNumNl();
				fragments.back().setForceLabel(forceLabel);
			}//end if
		}//end for j
	}//end for i
}//end function

/**
 \brief Parse explicit static modification from AminoAcid::sequence. <br>
 
 Modification enclosed with parentheses is removed from sequence.
 
 \param startIndex index at which mod begins.
 \return mass of modification.
 */
double PeptideNamespace::Peptide::parseStaticMod(size_t startIndex)
{
	//get index of parentheses close
	size_t end = sequence.find(')', startIndex);
	if(end == std::string::npos)
		throw std::runtime_error("Invalid peptide sequence: " + sequence);
	
	//get mass of modification
	double ret = std::stod(sequence.substr(startIndex + 1, end - 1));
	//erase modification from sequence
	sequence.erase(sequence.begin() + startIndex, sequence.begin() + end + 1);
	
	return ret;
}

/**
 \brief Remove explicit static amino acid modifications from sequence. <br>
 
 Sequences in the form AAC(+57.0)AAR*AAK will be parsed AACAARAAK to remove
 the explicit (+57.0) and * add modifications. Modifications will be preserved
 in the Peptide::aminoAcids member.
 */
void PeptideNamespace::Peptide::fixDiffMod(const aaDB::AADB& aminoAcidsMasses,
										   const char* diffmods)
{
	//check if n term mod
	double nTermMod = 0;
	if(sequence[0] == '('){
		nTermMod = parseStaticMod(0);
	}
	
	//check that n term is not a diff mod
	for(const char* p = diffmods; *p; p++)
		if(sequence[0] == *p)
			throw std::runtime_error("Invalid peptide sequence: " + sequence);
	
	//Check that first char is now a letter
	if(!isalpha(sequence[0]))
		throw std::runtime_error("Invalid peptide sequence!");
	
	for(size_t i = 0; i < sequence.length(); i++)
	{
		//first check if current index is a modification.
		if(sequence[i] == '(')
		{
			double m = parseStaticMod(i);
			aminoAcids.back().addStaticMod(m);
		}
		for(const char* p = diffmods; *p; p++){
			if(sequence[i] == *p){
				aminoAcids.back().setDynamicMod(sequence[i],
												aminoAcidsMasses.getMW(sequence[i]));
				sequence.erase(i, 1);
				modLocs.push_back(int(aminoAcids.size() - 1)); //add location of mod to modLocs
				nMod++; //increment nMod
			}
		}
		
		//exit loop if at end of sequence
		if(i >= sequence.length())
			break;
		
		//Check that current char is letter
		if(!isalpha(sequence[i]))
			throw std::runtime_error("Invalid peptide sequence: " + sequence);
		
		//add new amino acid to
		aminoAcids.emplace_back(aminoAcidsMasses.getMW(sequence[i]));
	}//end for
	
	//add n-terminal modification
	aminoAcids.begin()->addStaticMod(nTermMod);
}

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
 Prints peptide fragments and calculated MZs to out.
 For debugging
 \param out stream to print to.
 \param printHeader Should a header be printed?
 \param printFoundIntensity Should the intensity for the fragment be printed?
 */
void PeptideNamespace::Peptide::printFragments(std::ostream& out,
        bool printHeader, bool printFoundIntensity) const
{
	assert(out);

	if(printHeader) {
        out << "index" << OUT_DELIM << "label" << OUT_DELIM << "mz";
        if(printFoundIntensity)
            out << OUT_DELIM << "foundIntensity";
        out << NEW_LINE;
    }
	size_t len = fragments.size();
	for(size_t i = 0; i < len; i++) {
        out << i << OUT_DELIM <<
            fragments[i].getLabel() <<
            OUT_DELIM << fragments[i].getMZ();
            if(printFoundIntensity)
                out << OUT_DELIM << fragments[i].getFoundIntensity();
        out << NEW_LINE;
    }
}

/**
 Removes all unlabeled FragmentIon(s) from fragments.
 For debugging.
 */
void PeptideNamespace::Peptide::removeUnlabeledFrags()
{
	for(auto it = fragments.begin(); it != fragments.end();)
	{
		if(!it->getFound())
			fragments.erase(it);
		else ++it;
	}
}

/**
 * Remove FragmentIon (s) which are below a specified intensity threshold.
 * \param min_int Minium ion intensity to allow.
 * \param require_nl Should only neutral loss fragment ions be removed?
 */
void PeptideNamespace::Peptide::removeLabelIntensityBelow(double min_int, bool require_nl, bool remove)
{
    for(auto it = fragments.begin(); it != fragments.end();)
    {
        if(it->getFoundIntensity() <= min_int)
        {
            if(require_nl && !it->isNL()) {
                ++it;
                continue;
            }

            if(remove) {
                fragments.erase(it);
                continue;
            }
            else {
                it->setFound(false);
            }
        }
        ++it;
    }
}

/**
 * Divide all fragment ions by \p den.
 * \param den Denominator do divide ion intensities by.
 */
void PeptideNamespace::Peptide::normalizeLabelIntensity(double den)
{
    for(auto & fragment : fragments)
        if(fragment.getFound())
            fragment.setFoundIntensity(fragment.getFoundIntensity() / den);
}

double PeptideNamespace::calcMass(double mz, int charge){
	return mz * charge - (charge * PeptideNamespace::H_MASS);
}
double PeptideNamespace::calcMZ(double mass, int charge){
	return (mass + (charge * PeptideNamespace::H_MASS)) / charge;
}

/**
 Returns sum of masss of amino acids in vec.

 \param begin iterator to start of vec
 \param end iterator to end of vec
 
 \return mass of of peptide
*/
double PeptideNamespace::calcMass(PeptideNamespace::PepIonIt begin, PeptideNamespace::PepIonIt end)
{
	double ret = 0;
	for(auto it = begin; it != end; ++it)
		ret += it->getTotalMass();
	return ret;
}

/**
 Returns a string containing all the modifications if interest in vec.
 
 \param begin iterator to start of vec
 \param end iterator to end of vec
 
 \return all modifications concated into a single string
 */
std::string PeptideNamespace::concatMods(PeptideNamespace::PepIonIt begin, PeptideNamespace::PepIonIt end)
{
	std::string ret;
	for(auto it = begin; it != end; ++it)
		if(it->hasDynamicMod())
			ret += it->getMod();
	return ret;
}

