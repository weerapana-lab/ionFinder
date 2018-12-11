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
	
	double const DEFAULT_NEUTRAL_LOSS_MASS = 43.0058;
	
	class Params;
	
	class Params : public base::ParamsBase{
	public:
		enum class InputModes{SINGLE, RECURSIVE};
		typedef std::map<std::string, std::string> FilterFilesType;
		
	private:
		std::string _parentDir;
		InputModes _inputMode;
		FilterFilesType _filterFiles;
		std::string _dtaFilterBase;
		double _neutralLossMass;
		
		bool _includeReverse;
		
		InputModes intToInputModes(int) const;
		bool getFlist();
		
	public:
		
		Params() : ParamsBase(PROG_USAGE_FNAME, PROG_HELP_FILE){
			_parentDir = "";
			_inputMode = InputModes::SINGLE;
			_includeReverse = false;
			_dtaFilterBase = DEFAULT_FILTER_FILE_NAME;
			_neutralLossMass = DEFAULT_NEUTRAL_LOSS_MASS;
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
		std::string getInputModeIndependentParentDir() const;
		double getNeutralLossMass() const{
			return _neutralLossMass;
		}
	};
}

#endif /* params_hpp */
