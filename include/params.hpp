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
#include "../include/utils.hpp"

using namespace std;

namespace params{
	
	//program file locations
	string const PROG_WD = string(getenv("HOME")) + "/scripts/ms2_anotator";
	string const PROG_DB = PROG_WD + "/db";
	string const PROG_AA_MASS_LOCATION = PROG_DB + "/aaMasses.txt";
	string const PROG_DEFAULT_SMOD_FILE = PROG_DB + "/staticModifications.txt";
	string const PROG_HELP_FILE = PROG_DB + "/help.man";
	string const PROG_USAGE_FNAME = PROG_DB + "/usage.txt";
	
	string const DEFAULT_SMOD_NAME = "staticModifications.txt";
	
	string const PARAM_ERROR_MESSAGE = " is an invalid argument for ";
	
	class InFile;
	class Params;
	
	class InFile{
		friend class Params;
	private:
		string infile;
		string sequence;
		int scan;
		
	public:
		InFile() {
			infile = "";
			sequence = "";
			scan = 0;
		}
		~InFile() {}
		
		string getInfile() const{
			return infile;
		}
		string getSeq() const{
			return sequence;
		}
		int getScan() const{
			return scan;
		}
	};
	
	class Params{
	private:
		string wd;
		string aaMassFile;
		string smodFile;
		bool force;
		int inputMode;
		bool wdSpecified;
		string sequestParams;
		string ofname;
		
		bool seqSpecified;
		bool dtaSpecified, seqParSpecified;
		bool ms2Specified, scanSpecified;
		bool smodSpecified;
		bool mzSpecified;
		bool minIntensitySpecified;
		
		//program paramaters
		int minFragCharge;
		int maxFragCharge;
		double matchTolerance;
		double minLabelIntensity;
		double minMZ;
		double maxMZ;
		double minIntensity;
		bool includeAllIons;
		
		void displayHelp() const{
			utils::systemCommand("man " + PROG_HELP_FILE);
		}
		void usage() const;
		bool checkParams() const;
		bool writeSmod(string) const;
		
	public:
		InFile inFile;
		
		Params() {
			wd = "";
			smodFile = PROG_DEFAULT_SMOD_FILE;
			aaMassFile = PROG_AA_MASS_LOCATION;
			force = false;
			inputMode = 0;
			wdSpecified = false;
			sequestParams = "";
			ofname = "";
			
			minFragCharge = 1;
			maxFragCharge = 1;
			matchTolerance = 0.25;
			minLabelIntensity = 0;
			minMZ = 0;
			maxMZ = 0;
			mzSpecified = false;
			minIntensity = 0;
			minIntensitySpecified = false;
			includeAllIons = true;
			
			seqSpecified = false;
			dtaSpecified = false;
			seqParSpecified = false;
			smodSpecified = false;
			scanSpecified = false;
			ms2Specified = false;
		}
		
		bool getArgs(int, const char* const[]);
		
		string getWD() const{
			return wd;
		}
		int getMinFragCharge() const{
			return minFragCharge;
		}
		int getMaxFragCharge() const{
			return maxFragCharge;
		}
		string getSmodFileLoc() const{
			return smodFile;
		}
		string getAAMassFileLoc() const{
			return aaMassFile;
		}
		double getMatchTolerance() const{
			return matchTolerance;
		}
		double getMinMZ() const{
			return minMZ;
		}
		double getMaxMZ() const{
			return maxMZ;
		}
		bool getMZSpecified() const{
			return mzSpecified;
		}
		bool getMinIntensitySpecified() const{
			return minIntensitySpecified;
		}
		bool getIncludeAllIons() const{
			return includeAllIons;
		}
		double getMinLabelIntensity() const{
			return minLabelIntensity;
		}
		double getMinIntensity() const{
			return minIntensity;
		}
		bool getForce() const{
			return force;
		}
		int getInputMode() const{
			return inputMode;
		}
		bool getWDSpecified() const{
			return wdSpecified;
		}
	};
}

#endif /* params_hpp */
