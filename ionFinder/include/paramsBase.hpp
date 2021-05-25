//
// paramsBase.hpp
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

#ifndef paramsBase_hpp
#define paramsBase_hpp

#include <iostream>
#include <string>
#include <cassert>

#include <utils.hpp>
#include <config.h>
#include <git_info.h>

namespace base{
	
	std::string const PROG_DB = PROG_SHARE_DIR;
	std::string const PROG_MAN = PROG_MAN_DIR;
	std::string const PROG_DEFAULT_SMOD_FILE = PROG_DB + "/staticModifications.txt";
	double const DEFAULT_MATCH_TOLERANCE = 10;
	double const DEFAULT_PLOT_HEIGHT = 4;
	double const DEFAULT_PLOT_WIDTH = 12;
	
	std::string const DEFAULT_SMOD_NAME = "staticModifications.txt";
	
	std::string const PARAM_ERROR_MESSAGE = " is an invalid argument for ";
	
	class ParamsBase;
	
	class ParamsBase{
	protected:
		std::string _wd;
		std::string _smodFile;
		std::string _usageFile;
		std::string _helpFile;
		bool wdSpecified;
		std::string sequestParamsFname;
		std::string ofname;
		
		int minFragCharge;
		int maxFragCharge;
		double minLabelIntensity;
		
		//match tolerance stuff
		//!match tolerance for fragment ions in either ppm or Th
		double matchTolerance;
		enum class MatchType{
			//! match in ppm
			PPM,
			//! match in Th
			TH,
			//!unknown type
			UNKNOWN
		};
		//!How should fragment ion tolerances be calculated?
		MatchType _matchType;
		
		double minMZ;
		double maxMZ;
		double minIntensity;
		bool includeAllIons;
		std::string multipleMatchCompare;
		double plotWidth;
		double plotHeight;
		
		bool smodSpecified;
		bool seqParSpecified;
		bool minMzSpecified;
		bool maxMzSpecified;
		bool minIntensitySpecified;
		
		bool verbose;
		
		bool writeSmod(std::string wd) const;
		void usage(std::string message = "", std::ostream& out = std::cerr) const;
		void printGitVersion(std::ostream& out = std::cout) const;
		void displayHelp() const;
		static MatchType strToMatchType(std::string);
		
	public:
		ParamsBase(std::string usageFile, std::string helpFile){
			//file locations
			_wd = "";
			_smodFile = PROG_DEFAULT_SMOD_FILE;
			_usageFile = usageFile;
			_helpFile = helpFile;
			ofname = "";
			wdSpecified = false;
			smodSpecified = false;
			
			minFragCharge = 1;
			maxFragCharge = 1;
			matchTolerance = DEFAULT_MATCH_TOLERANCE;
			_matchType = MatchType::PPM;
			minLabelIntensity = 0;
			multipleMatchCompare = "intensity";
			
			seqParSpecified = false;
			minMZ = 0;
			maxMZ = 0;
			plotWidth = DEFAULT_PLOT_WIDTH;
			plotHeight = DEFAULT_PLOT_HEIGHT;
			minMzSpecified = false;
			maxMzSpecified = false;
			minIntensity = 0;
			minIntensitySpecified = false;
			includeAllIons = true;
			verbose = false;
			smodSpecified = false;
		}

        //modifiers
        virtual bool getArgs(int, const char* const[]);
		
		void setSeqParFname(std::string path){
			sequestParamsFname = path;
			seqParSpecified = true;
		}
		
		//properties
		std::string getWD() const{
			return _wd;
		}
		std::string getSmodFileLoc() const{
			return _smodFile;
		}
		bool getSmodFileSpecified() const{
			return smodSpecified;
		}

		bool getWDSpecified() const{
			return wdSpecified;
		}
		int getMinFragCharge() const{
			return minFragCharge;
		}
		int getMaxFragCharge() const{
			return maxFragCharge;
		}
		double getMatchTolerance() const;
		double getMatchTolerance(double mz) const;
		double getMinLabelIntensity() const{
			return minLabelIntensity;
		}
		double getMinMZ() const{
			return minMZ;
		}
		double getMaxMZ() const{
			return maxMZ;
		}
		double getPlotHeight() const{
			return plotHeight;
		}
		double getPlotWidth() const{
			return plotWidth;
		}
		bool getMZSpecified() const{
			return maxMzSpecified || minMzSpecified;
		}
		bool getMinMZSpecified() const{
			return minMzSpecified;
		}
		bool getMaxMZSpecified() const{
			return maxMzSpecified;
		}
		bool getMinIntensitySpecified() const{
			return minIntensitySpecified;
		}
		bool getIncludeAllIons() const{
			return includeAllIons;
		}
		double getMinIntensity() const{
			return minIntensity;
		}
		std::string getMultipleMatchCompare() const{
			return multipleMatchCompare;
		}
		bool getSeqParSpecified() const{
			return seqParSpecified;
		}
		std::string getSeqParFname() const{
			return sequestParamsFname;
		}
		bool getVerbose() const{
			return verbose;
		}
	};
}

#endif /* paramsBase_hpp */
