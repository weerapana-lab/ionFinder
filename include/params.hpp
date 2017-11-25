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
	string const PROG_WD = string(getenv("HOME")) + "/Xcode_projects/ms2_anotator";
	string const PROG_DB = PROG_WD + "/db";
	string const PROG_AA_MASS_LOCATION = PROG_DB + "/aaMasses.txt";
	string const PROG_DEFAULT_SMOD_FILE = PROG_DB + "/staticModifications.txt";
	string const PROG_HELP_FILE = PROG_DB + "/help.man";
	string const PROG_USAGE_FNAME = PROG_DB + "/usage.txt";
	
	string const DEFAULT_SMOD_NAME = "staticModifications.txt";
	
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
		
		//program paramaters
		int minFragCharge;
		int maxFragCharge;
		double matchTolerance;
		double minLabelIntensity;
		
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
		size_t getMinFragCharge() const{
			return minFragCharge;
		}
		size_t getMaxFragCharge() const{
			return maxFragCharge;
		}
		string getSmodFileLoc() const{
			return smodFile;
		}
		string getAAMassFileLoc() const{
			return aaMassFile;
		}
		bool getForce() const{
			return force;
		}
		int getInputMode() const{
			return inputMode;
		}
	};
}

#endif /* params_hpp */
