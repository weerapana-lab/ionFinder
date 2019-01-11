//
//  paramsBase.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 12/9/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#ifndef paramsBase_hpp
#define paramsBase_hpp

#include <iostream>
#include <string>
#include <cassert>

#include <config.hpp>
#include <utils.hpp>

namespace base{
	
	std::string const PROG_WD = CONFIG_PROG_WD_DIR;
	std::string const PROG_DB = PROG_WD + "/db";
	std::string const PROG_MAN = PROG_WD + "/man";
	std::string const PROG_AA_MASS_LOCATION = PROG_DB + "/aaMasses.txt";
	std::string const PROG_DEFAULT_SMOD_FILE = PROG_DB + "/staticModifications.txt";
	double const DEFAULT_MATCH_TOLERANCE = 0.5;
	
	std::string const DEFAULT_SMOD_NAME = "staticModifications.txt";
	
	std::string const PARAM_ERROR_MESSAGE = " is an invalid argument for ";
	
	class ParamsBase;
	
	class ParamsBase{
	protected:
		std::string _wd;
		std::string _aaMassFile;
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
		//!How shoud fragment ion tolerances be calulated?
		MatchType _matchType;
		
		double minMZ;
		double maxMZ;
		double minIntensity;
		bool includeAllIons;
		std::string multipleMatchCompare;
		
		bool smodSpecified;
		bool seqParSpecified;
		bool minMzSpecified;
		bool maxMzSpecified;
		bool minIntensitySpecified;
		
		bool verbose;
		
		bool writeSmod(std::string wd) const;
		void usage(std::ostream& out = std::cerr) const;
		void displayHelp() const;
		static MatchType strToMatchType(std::string);
		
	public:
		ParamsBase(std::string usageFile, std::string helpFile){
			//file locations
			_wd = "";
			_smodFile = PROG_DEFAULT_SMOD_FILE;
			_aaMassFile = PROG_AA_MASS_LOCATION;
			_usageFile = usageFile;
			_helpFile = helpFile;
			ofname = "";
			
			minFragCharge = 1;
			maxFragCharge = 1;
			matchTolerance = DEFAULT_MATCH_TOLERANCE;
			_matchType = MatchType::TH;
			minLabelIntensity = 0;
			multipleMatchCompare = "intensity";
			
			seqParSpecified = false;
			minMZ = 0;
			maxMZ = 0;
			minMzSpecified = false;
			maxMzSpecified = false;
			minIntensity = 0;
			minIntensitySpecified = false;
			includeAllIons = true;
			verbose = false;
		}
		
		//modifers
		bool getArgs(int, const char* const[]);
		
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
		std::string getAAMassFileLoc() const{
			return _aaMassFile;
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
