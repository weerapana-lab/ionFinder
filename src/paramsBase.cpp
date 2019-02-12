//
//  paramsBase.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 1/7/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#include <paramsBase.hpp>

/**
 \brief Convert string to ParamsBase::MatchType.
 
 "mz" and "th" will be return MatchType::TH.
 "ppm" will return MatchType::PPM.
 \param mt str to convert.
 \return match type
 */
base::ParamsBase::MatchType base::ParamsBase::strToMatchType(std::string mt)
{
	if(mt == "mz" || mt == "th")
		return MatchType::TH;
	else if(mt == "ppm")
		return MatchType::PPM;
	else return MatchType::UNKNOWN;
}

/**
 \brief Write empty static modifications file to \p wd
 
 The file is written to the path \p wd/base::PROG_DEFAULT_SMOD_FILE.
 \param wd directory to write static modifications file to.
 \return true is successful
 */
bool base::ParamsBase::writeSmod(std::string wd) const
{
	if(_wd[_wd.length() - 1] != '/')
		wd = _wd + "/";
	std::ofstream outF((wd + base::DEFAULT_SMOD_NAME).c_str());
	std::ifstream staticMods(base::PROG_DEFAULT_SMOD_FILE);
	if(!outF || !staticMods)
		return false;
	
	if(wdSpecified)
		std::cerr << NEW_LINE << "Generating " << wd << base::DEFAULT_SMOD_NAME << NEW_LINE;
	else std::cerr << NEW_LINE <<"Generating ./" << base::DEFAULT_SMOD_NAME << NEW_LINE;
	
	outF << utils::COMMENT_SYMBOL << " Static modifications for ms2_annotator" << NEW_LINE
	<< utils::COMMENT_SYMBOL << " File generated on: " << utils::ascTime() << NEW_LINE;
	
	std::string line;
	while(utils::safeGetline(staticMods, line))
		outF << line << NEW_LINE;
	
	return true;
}

/**
 \brief Print usage to \p out
 
 Prints contents of ParamsBase::_usageFile to \p out
 \param out ostream to print to.
 */
void base::ParamsBase::usage(std::ostream& out) const
{
	std::ifstream inF(_usageFile);
	std::string line;
	while(utils::safeGetline(inF, line))
		out << line << NEW_LINE;
}

/**
 \brief Display ParamsBase::_helpFile in terminal
 */
void base::ParamsBase::displayHelp() const{
	utils::systemCommand("man " + _helpFile);
}

/**
 \brief Get fragment ion tolerance.
 
 Only works if MatchType is MatchType::TH.
 If MatchType::PPM the mz would be needed to calculate the tolerance. <br>
 Also see double ParamsBase::getMatchTolerance(double ms) const.
 \pre ParamsBase::_matchType == MatchType::TH
 \return fragment match tolerance
 */
double base::ParamsBase::getMatchTolerance() const{
	assert(_matchType == MatchType::TH);
	return matchTolerance;
}

/**
 \brief Get fragment ion tolerance.
 
 Calculates match tolerance for \p mz.
 Uses ParamsBase::_matchType to determine whether the match tolerance should be in ppm or Th.
 \pre ParamsBase::_matchType != MatchType::UNKNOWN
 \param mz ion mz to calculate tolerance for.
 \return fragment match tolerance
 */
double base::ParamsBase::getMatchTolerance(double mz) const
{
	if(_matchType == MatchType::TH)
		return matchTolerance;
	else if(_matchType == MatchType::PPM){
		return mz * (matchTolerance / 1e6);
	}
	else{
		throw std::runtime_error("Unknown match type!");
	}
}
