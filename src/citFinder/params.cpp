//
//  params.cpp
//  citFinder
//
//  Created by Aaron Maurais on 12/9/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#include <citFinder/params.hpp>

bool CitFinder::Params::getArgs(int argc, const char* const argv[])
{
	_wd = utils::pwd();
	assert(utils::dirExists(_wd));
	
	//!TODO: add all options to getArgs
	for(int i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
		{
			displayHelp();
			return false;
		}
		if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dir"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			_wd = utils::absPath(argv[i]);
			wdSpecified = true;
			if(!utils::dirExists(_wd))
			{
				std::cerr << "Specified direectory does not exist." << NEW_LINE;
				return false;
			}
			continue;
		}
		if(!strcmp(argv[i], "-dta"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			_dtaFilterBase = argv[i];
			continue;
		}
		if(!strcmp(argv[i], "-rev"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			if(!(!strcmp(argv[i], "0") || !strcmp(argv[i], "1")))
			{
				std::cerr << argv[i] << base::PARAM_ERROR_MESSAGE << "includeReverse" << std::endl;
				return false;
			}
			_includeReverse = std::stoi(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--printSpectra"))
		{
			_printSpectraFiles = true;
			continue;
		}
		if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--lossMass"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			_neutralLossMass = std::stod(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-mt") || !strcmp(argv[i], "--matchTolerance"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			matchTolerance = std::stod(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-minC"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			minFragCharge = std::stoi(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-maxC"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			maxFragCharge = std::stoi(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-minMZ"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			minMZ = std::stod(argv[i]);
			minMzSpecified = true;
			continue;
		}
		if(!strcmp(argv[i], "-maxMZ"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			maxMZ = std::stod(argv[i]);
			maxMzSpecified = true;
			continue;
		}
		if(!strcmp(argv[i], "-minLabInt"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			minLabelIntensity = std::stoi(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-minInt"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			minIntensity = std::stod(argv[i]);
			minIntensitySpecified = true;
			continue;
		}
		if(!strcmp(argv[i], "-incAllIons"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			if(!(!strcmp(argv[i], "0") || !strcmp(argv[i], "1")))
			{
				std::cerr << argv[i] << base::PARAM_ERROR_MESSAGE << "incAllIons" << NEW_LINE;
				return false;
			}
			includeAllIons = std::stoi(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose"))
		{
			verbose = true;
			continue;
		}
		if(!strcmp(argv[i], "--version"))
		{
			std::cout << "citFinder " << BIN_VERSION << NEW_LINE;
			return false;
		}
		else if(utils::isFlag(argv[i])){
			std::cerr << argv[i] << " is an invalid argument." << NEW_LINE;
			usage();
			return false;
		}
		else{ //we are in input dirs
			while(i < argc){
				_inDirs.push_back(std::string(argv[i++]));
				_inDirSpecified = true;
			}
		}
		//end of else
	}//end of for i
	
	//fix options
	if(_wd[_wd.length() - 1] != '/')
		_wd += "/";
	if(!getFlist()){
		std::cerr << "Could not find DTAFilter-files!" << NEW_LINE;
		return false;
	}
	
	return true;
}//end of getArgs

/**
 Searches all directories in _inDirs for DTAFilter files.
 If _inDirs is empty, current working directory is used.
 @return true if > 1 filter file was found, else false
 */
bool CitFinder::Params::getFlist()
{
	if(_inDirs.size() == 0){
		_inDirs.push_back(_wd);
		_wd = utils::parentDir(_wd);
	}
	for(auto it = _inDirs.begin(); it != _inDirs.end(); ++it)
	{
		std::string fname = (_inDirSpecified ? (_wd + *it) : *it) + ("/" + _dtaFilterBase);
		if(utils::fileExists(fname)){
			_filterFiles[utils::baseName(*it)] = fname;
		}
		else{
			std::cerr << "Warning: No filter file found in: " << *it << NEW_LINE;
		}
	}
	if(_filterFiles.size() == 0)
		return false;
	
	return true;
}
