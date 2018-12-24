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

namespace Ms2_annotator{
	
	//program file locations
	std::string const PROG_HELP_FILE = base::PROG_DB + "/ms2_annotator/helpFile.man";
	std::string const PROG_USAGE_FNAME = base::PROG_DB + "/ms2_annotator/usage.txt";
	
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
		bool force;
		int inputMode;
		
		bool seqSpecified;
		bool ms2Specified, scanSpecified;
		bool dtaSpecified;
		
		//program parameters
		
		bool checkParams() const;
		
	public:
		InFile inFile;
		
		Params() : ParamsBase(PROG_USAGE_FNAME, PROG_HELP_FILE) {
			force = false;
			inputMode = 0;
			wdSpecified = false;
			sequestParamsFname = "";
			ofname = "";
			
			seqSpecified = false;
			dtaSpecified = false;
			smodSpecified = false;
			scanSpecified = false;
			ms2Specified = false;
			verbose = true; //set verbose to true by default
		}
		
		bool getArgs(int, const char* const[]);
		
		//properties
		bool getForce() const{
			return force;
		}
		int getInputMode() const{
			return inputMode;
		}
	};
}

#endif /* params_hpp */
