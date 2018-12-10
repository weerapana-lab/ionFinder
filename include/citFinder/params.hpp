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
#include <map>
#include <vector>

#include <citFinder/citFinder.hpp>
#include <paramsBase.hpp>
#include <utils.hpp>

namespace CitFinder{
	
	//program file locations
	std::string const PROG_USAGE_FNAME = base::PROG_DB + "/citFinder/usage.txt";
	std::string const PROG_HELP_FILE = base::PROG_DB + "/citFinder/helpFile.txt";
	std::string const DEFAULT_FILTER_FILE_NAME = "DTASelect-filter.txt";
	
	class Params;
	
	class Params : public base::ParamsBase{
	public:
		enum class inputModes{SINGLE, RECURSIVE};
		typedef std::map<std::string, std::string> FilterFilesType;
		
	private:
		std::string _parentDir;
		inputModes _inputMode;
		FilterFilesType _filterFiles;
		
		bool _includeReverse;
		
		inputModes intToInputModes(int) const;
		bool getFlist();
		
	public:
		
		Params() : ParamsBase(PROG_USAGE_FNAME, PROG_HELP_FILE){
			_parentDir = "";
			_inputMode = inputModes::SINGLE;
			_includeReverse = false;
		}
		
		//modifers
		bool getArgs(int, const char* const[]);
		
		//properties
		const FilterFilesType& getFilterFiles() const{
			return _filterFiles;
		}
		bool getIncludeReverse() const{
			return _includeReverse;
		}
	};
}

#endif /* params_hpp */
