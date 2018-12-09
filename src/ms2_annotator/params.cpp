//
//  params.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <ms2_annotator/params.hpp>

bool params::Params::getArgs(int argc, const char* const argv [])
{
	//get wd
	wd = utils::pwd();
	assert(utils::dirExists(wd));
	
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
			wd = utils::absPath(argv[i]);
			wdSpecified = true;
			if(!utils::dirExists(wd))
			{
				std::cerr << "Specified direectory does not exist." << std::endl;
				return false;
			}
			continue;
		}
		if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--seq"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			inFile.sequence =std::string(argv[i]);
			seqSpecified = true;
			
			continue;
		}
		if(!strcmp(argv[i], "-m") || !strcmp(argv[i], "--ms2"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			inFile.infile = utils::absPath(argv[i]);
			inputMode = 0;
			ms2Specified = true;
			continue;
		}
		if(!strcmp(argv[i], "-s") || !strcmp(argv[i], "--scan"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			inFile.scan = std::stoi(argv[i]);
			inputMode = 0;
			scanSpecified = true;
			continue;
		}
		if(!strcmp(argv[i], "-dta"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			inFile.infile = utils::absPath(argv[i]);
			dtaSpecified = true;
			inputMode = 1;
			continue;
		}
		if(!strcmp(argv[i], "-sp"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			sequestParams = utils::absPath(argv[i]);
			seqParSpecified = true;
			continue;
		}
		if(!strcmp(argv[i], "-printSmod"))
		{
			if(!writeSmod(wd))
				std::cerr << "Could not write new smod file!" << std::endl;
			return false;
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
				std::cerr << argv[i] << params::PARAM_ERROR_MESSAGE << "incAllIons" << std::endl;
				return false;
			}
			includeAllIons = std::stoi(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version"))
		{
			std::cout << "annotate_ms2 " << BIN_VERSION << std::endl;
			//std::cout << "Last git commit: " << GIT_DATE << std::endl;
			//std::cout << "git revision: " << GIT_COMMIT << std::endl;
			return false;
		}
		else{
			std::cerr << argv[i] << " is an invalid argument." << std::endl;
			usage();
			return false;
		}
		
	}
	
	//fix options
	if(wd[wd.length() - 1] != '/')
		wd += "/";
	
	return checkParams();
}

bool params::Params::checkParams() const
{
	if(!seqSpecified)
	{
		std::cerr << "Sequence must be specified" << std::endl;
		usage();
		return false;
	}
	if(inputMode == 0)
	{
		if(!(scanSpecified && ms2Specified))
		{
			std::cerr << std::endl << "Scan number and .ms2 file must be specified" << std::endl;
			usage();
			return false;
		}
	}
	else if(inputMode == 1)
	{
		assert(dtaSpecified);
	}
	
	return true;
}

//print program usage information located in PROG_USAGE_FNAME
void params::Params::usage() const
{
	utils::File file(PROG_USAGE_FNAME);
	assert(file.read());
	
	while(!file.end())
		std::cerr << file.getLine() << std::endl;
}

bool params::Params::writeSmod(std::string _wd) const
{
	if(_wd[_wd.length() - 1] != '/')
		_wd += "/";
	std::ofstream outF((_wd + params::DEFAULT_SMOD_NAME).c_str());
	utils::File staticMods(params::PROG_DEFAULT_SMOD_FILE);
	if(!outF || !staticMods.read())
		return false;
	
	if(wdSpecified)
		std::cerr << std::endl << "Generating " << _wd << DEFAULT_SMOD_NAME << std::endl;
	else std::cerr << std::endl <<"Generating ./" << DEFAULT_SMOD_NAME << std::endl;
	
	outF << utils::COMMENT_SYMBOL << " Static modifications for ms2_annotator" << std::endl
	<< utils::COMMENT_SYMBOL << " File generated on: " << utils::ascTime() << std::endl
	<< "<staticModifications>" << std::endl;
	
	while(!staticMods.end())
		outF << staticMods.getLine() << std::endl;
	
	outF << std::endl << "</staticModifications>" << std::endl;
	
	return true;
}





