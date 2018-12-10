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
		bool wdSpecified;
		std::string sequestParams;
		std::string ofname;
		
		bool seqSpecified;
		bool dtaSpecified, seqParSpecified;
		bool smodSpecified;
		
		bool writeSmod(std::string wd) const{
			if(_wd[_wd.length() - 1] != '/')
				wd = _wd + "/";
			std::ofstream outF((wd + base::DEFAULT_SMOD_NAME).c_str());
			utils::File staticMods(base::PROG_DEFAULT_SMOD_FILE);
			if(!outF || !staticMods.read())
				return false;
			
			if(wdSpecified)
				std::cerr << std::endl << "Generating " << wd << base::DEFAULT_SMOD_NAME << std::endl;
			else std::cerr << std::endl <<"Generating ./" << base::DEFAULT_SMOD_NAME << std::endl;
			
			outF << utils::COMMENT_SYMBOL << " Static modifications for ms2_annotator" << std::endl
			<< utils::COMMENT_SYMBOL << " File generated on: " << utils::ascTime() << std::endl
			<< "<staticModifications>" << std::endl;
			
			while(!staticMods.end())
				outF << staticMods.getLine() << std::endl;
			
			outF << std::endl << "</staticModifications>" << std::endl;
			
			return true;
		}
		
		void usage() const{
			utils::File file(_usageFile);
			assert(file.read());
			
			while(!file.end())
				std::cerr << file.getLine() << std::endl;
		}
		
	public:
		ParamsBase(std::string usageFile){
			//file locations
			_wd = "";
			_smodFile = PROG_DEFAULT_SMOD_FILE;
			_aaMassFile = PROG_AA_MASS_LOCATION;
			_usageFile = usageFile;
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
		bool getSeqParSpecified() const{
			return seqParSpecified;
		}
		std::string getSeqParFname() const{
			return sequestParams;
		}
	};
	
}

#endif /* paramsBase_hpp */
