//
// paramsBase.cpp
// ionFinder
// -----------------------------------------------------------------------------
// MIT License
// Copyright 2020 Aaron Maurais
// -----------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -----------------------------------------------------------------------------
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
void base::ParamsBase::usage(std::string message, std::ostream& out) const
{
    std::cerr << message << NEW_LINE;
	std::ifstream inF(_usageFile);
	if(!inF) std::cerr << "Could not open usage file at:\n\t" << _usageFile << NEW_LINE;
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
 Also see ParamsBase::getMatchTolerance(double ms) const.
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

/**
 Print git version and date and time of last commit to \p out
 \param out stream to print to
 */
void base::ParamsBase::printGitVersion(std::ostream& out) const{
	if(GIT_RETRIEVED_STATE) {
        out << "Last git commit: " << GIT_HEAD_SHA1 << NEW_LINE
            << "Git revision: " << GIT_LAST_COMMIT_DATE << NEW_LINE;
        if(GIT_IS_DIRTY)
            out << "WARN: there were uncommitted changes." << NEW_LINE;
    }
	else {
	    out << "WARN: failed to get the current git state. Is this a git repo?" << NEW_LINE;
    }
}

bool base::ParamsBase::getArgs(int, const char *const *) {
    return false;
}
