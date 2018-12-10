//
//  params.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef params_hpp
#define params_hpp

#include <iostream>
#include <string>

#include <paramsBase.hpp>
#include <utils.hpp>
#include <ms2_annotator/ms2_annotator.hpp>

namespace params{
	
	//program file locations
	std::string const PROG_HELP_FILE = base::PROG_DB + "/helpFile.man";
	std::string const PROG_USAGE_FNAME = base::PROG_DB + "/usage.txt";
	
	//std::string const DEFAULT_SMOD_NAME = "staticModifications.txt";
	
	//std::string const PARAM_ERROR_MESSAGE = " is an invalid argument for ";
	
	class InFile;
	class Params;
	
	class InFile{
		friend class Params;
	private:
		std::string infile;
		std::string sequence;
		int scan;
		
	public:
		InFile() {
			infile = "";
			sequence = "";
			scan = 0;
		}
		~InFile() {}
		
		std::string getInfile() const{
			return infile;
		}
		std::string getSeq() const{
			return sequence;
		}
		int getScan() const{
			return scan;
		}
	};
	
	class Params : public base::ParamsBase{
	private:
		//std::string wd;
		//std::string aaMassFile;
		//std::string smodFile;
		bool force;
		int inputMode;
		/*bool wdSpecified;
		std::string sequestParams;
		std::string ofname;*/
		
		//bool seqSpecified;
		//bool dtaSpecified, seqParSpecified;
		bool ms2Specified, scanSpecified;
		bool minMzSpecified;
		bool maxMzSpecified;
		bool minIntensitySpecified;
		
		//program paramaters
		//int minFragCharge;
		//int maxFragCharge;
		//double matchTolerance;
		//double minLabelIntensity;
		double minMZ;
		double maxMZ;
		double minIntensity;
		bool includeAllIons;
		std::string multipleMatchCompare;
		
		//void usage() const;
		bool checkParams() const;
		//bool writeSmod(std::string) const;
		
	public:
		InFile inFile;
		
		Params() : ParamsBase(PROG_USAGE_FNAME, PROG_HELP_FILE) {
			//wd = "";
			//smodFile = PROG_DEFAULT_SMOD_FILE;
			//aaMassFile = PROG_AA_MASS_LOCATION;
			force = false;
			inputMode = 0;
			wdSpecified = false;
			sequestParams = "";
			ofname = "";
			
			minMZ = 0;
			maxMZ = 0;
			minMzSpecified = false;
			maxMzSpecified = false;
			minIntensity = 0;
			minIntensitySpecified = false;
			includeAllIons = true;
			
			//seqSpecified = false;
			dtaSpecified = false;
			seqParSpecified = false;
			smodSpecified = false;
			scanSpecified = false;
			ms2Specified = false;
		}
		
		bool getArgs(int, const char* const[]);
		
		//properties
		/*std::string getWD() const{
			return wd;
		}
		int getMinFragCharge() const{
			return minFragCharge;
		}
		int getMaxFragCharge() const{
			return maxFragCharge;
		}
		std::string getSmodFileLoc() const{
			return smodFile;
		}
		std::string getAAMassFileLoc() const{
			return aaMassFile;
		}
		double getMatchTolerance() const{
			return matchTolerance;
		}*/
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
		/*double getMinLabelIntensity() const{
			return minLabelIntensity;
		}*/
		double getMinIntensity() const{
			return minIntensity;
		}
		std::string getMultipleMatchCompare() const{
			return multipleMatchCompare;
		}
		bool getForce() const{
			return force;
		}
		int getInputMode() const{
			return inputMode;
		}
		/*bool getWDSpecified() const{
			return wdSpecified;
		}
		bool getSeqParSpecified() const{
			return seqParSpecified;
		}
		std::string getSeqParFname() const{
			return sequestParams;
		}*/
	};
}

#endif /* params_hpp */
