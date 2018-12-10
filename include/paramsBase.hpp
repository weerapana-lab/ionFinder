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

#include <utils.hpp>

namespace base{
	
	std::string const PROG_WD =std::string(getenv("HOME")) + "/local/ms2_anotator";
	std::string const PROG_DB = PROG_WD + "/db";
	std::string const PROG_AA_MASS_LOCATION = PROG_DB + "/aaMasses.txt";
	std::string const PROG_DEFAULT_SMOD_FILE = PROG_DB + "/staticModifications.txt";
	
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
		std::string sequestParams;
		std::string ofname;
		
		int minFragCharge;
		int maxFragCharge;
		double matchTolerance;
		double minLabelIntensity;
		
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
		
		bool writeSmod(std::string wd) const{
			if(_wd[_wd.length() - 1] != '/')
				wd = _wd + "/";
			std::ofstream outF((wd + base::DEFAULT_SMOD_NAME).c_str());
			utils::File staticMods(base::PROG_DEFAULT_SMOD_FILE);
			if(!outF || !staticMods.read())
				return false;
			
			if(wdSpecified)
				std::cerr << NEW_LINE << "Generating " << wd << base::DEFAULT_SMOD_NAME << NEW_LINE;
			else std::cerr << NEW_LINE <<"Generating ./" << base::DEFAULT_SMOD_NAME << NEW_LINE;
			
			outF << utils::COMMENT_SYMBOL << " Static modifications for ms2_annotator" << NEW_LINE
			<< utils::COMMENT_SYMBOL << " File generated on: " << utils::ascTime() << NEW_LINE
			<< "<staticModifications>" << NEW_LINE;
			
			while(!staticMods.end())
				outF << staticMods.getLine() << NEW_LINE;
			
			outF << NEW_LINE << "</staticModifications>" << NEW_LINE;
			
			return true;
		}
		
		void usage() const{
			utils::File file(_usageFile);
			assert(file.read());
			
			while(!file.end())
				std::cerr << file.getLine() << NEW_LINE;
		}
		
		void displayHelp() const{
			utils::systemCommand("man " + _helpFile);
		}
		
	public:
		ParamsBase(std::string usageFile, std::string helpFile){
			//file locations
			_wd = "";
			_smodFile = PROG_DEFAULT_SMOD_FILE;
			_aaMassFile = PROG_AA_MASS_LOCATION;
			_usageFile = usageFile;
			_helpFile = helpFile;
			
			minFragCharge = 1;
			maxFragCharge = 1;
			matchTolerance = 0.25;
			minLabelIntensity = 0;
			
			seqParSpecified = false;
			minMZ = 0;
			maxMZ = 0;
			minMzSpecified = false;
			maxMzSpecified = false;
			minIntensity = 0;
			minIntensitySpecified = false;
			includeAllIons = true;
		}
		
		//modifers
		bool getArgs(int, const char* const[]);
		
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
		double getMatchTolerance() const{
			return matchTolerance;
		}
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
			return sequestParams;
		}
	};
	
}

#endif /* paramsBase_hpp */
