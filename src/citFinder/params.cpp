//
//  params.cpp
//  citFinder
//
//  Created by Aaron Maurais on 12/9/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#include <citFinder/params.hpp>

CitFinder::Params::inputModes CitFinder::Params::intToInputModes(int val) const
{
	switch(val){
		case 0 : return inputModes::SINGLE;
			break;
		case 1 : return inputModes::RECURSIVE;
			break;
		default : throw std::runtime_error("Invalid type!");
	}
}

bool CitFinder::Params::getArgs(int argc, const char* const argv[])
{
	_wd = utils::pwd();
	assert(utils::dirExists(_wd));
	
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
		if(!strcmp(argv[i], "-i") || !strcmp(argv[i], "--inputMode"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			if(!(!strcmp(argv[i], "0") || !strcmp(argv[i], "1")))
			{
				std::cerr << argv[i] << base::PARAM_ERROR_MESSAGE << "inputMode" << NEW_LINE;
				return false;
			}
			_inputMode = intToInputModes(atoi(argv[i]));
			continue;
		}
		if(!strcmp(argv[i], "-dta"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			_dtaFilterBase = utils::absPath(argv[i]);
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
		if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version"))
		{
			std::cout << "citFinder " << BIN_VERSION << NEW_LINE;
			return false;
		}
		else{
			std::cerr << argv[i] << " is an invalid argument." << NEW_LINE;
			usage();
			return false;
		}//end of else
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

bool CitFinder::Params::getFlist()
{
	if(_inputMode == inputModes::SINGLE)
	{
		std::string baseName = utils::baseName(_wd);
		_filterFiles[baseName] = _wd + _dtaFilterBase;
	}
	else if(_inputMode == inputModes::RECURSIVE)
	{
		std::vector<std::string> files;
		if(!utils::ls(_wd.c_str(), files))
			return false;
		
		for(auto it = files.begin(); it != files.end(); ++it)
		{
			if(utils::isDir(_wd + *it))
			{
				std::string fname = _wd + *it + "/" + _dtaFilterBase;
				std::cout << fname << NEW_LINE;
				if(utils::fileExists(fname))
					_filterFiles[utils::baseName(*it)] = fname;
			}
		}
		if(_filterFiles.size() == 0)
			return false;
	}
	return true;
}
