//
//  params.hpp
//  citFinder
//
//  Created by Aaron Maurais on 12/9/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#ifndef params_hpp
#define params_hpp

#include <string>
#include <iostream>

#include <citFinder/citFinder.hpp>
#include <utilt.hpp>

namespace citFinder{
	
	//program file locations
	std::string const PROG_WD =std::string(getenv("HOME")) + "/local/ms2_anotator";
	std::string const PROG_DB = PROG_WD + "/db";
	std::string const PROG_AA_MASS_LOCATION = PROG_DB + "/aaMasses.txt";
	std::string const PROG_DEFAULT_SMOD_FILE = PROG_DB + "/staticModifications.txt";
	//std::string const PROG_HELP_FILE = PROG_DB + "/helpFile.man";
	std::string const PROG_USAGE_FNAME = PROG_DB + "/citFinder/usage.txt";
	
	std::string const DEFAULT_SMOD_NAME = "staticModifications.txt";
	
	std::string const PARAM_ERROR_MESSAGE = " is an invalid argument for ";
	
	class Params;
	
	class Params{
	private:
		std::string wd;
		std::string aaMassFile;
		std::string smodFile;
		
		
	public:
		Param(){
			//file locations
			wd = "";
			smodFile = PROG_DEFAULT_SMOD_FILE;
			aaMassFile = PROG_AA_MASS_LOCATION;
			
		}
		
		//modifers
		bool getArgs(int, const char* const[]);
		
		//properties
		std::string getWD() const{
			return wd;
		}
		std::string getSmodFileLoc() const{
			return smodFile;
		}
		std::string getAAMassFileLoc() const{
			return aaMassFile;
		}
	}
}

#endif /* params_hpp */
