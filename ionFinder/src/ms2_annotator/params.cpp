//
//  params.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <ms2_annotator/params.hpp>

/**
 Parses command line arguments and stores in Params object
 \pre current working directory exists
 \param argc argc from main
 \param argv argv from main
 \return true if args were successfully read.
 */
bool Ms2_annotator::Params::getArgs(int argc, const char* const argv [])
{
	//get wd
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
			sequestParamsFname = utils::absPath(argv[i]);
			seqParSpecified = true;
			continue;
		}
		if(!strcmp(argv[i], "-printSmod"))
		{
			if(!writeSmod(_wd))
				std::cerr << "Could not write new smod file!" << NEW_LINE;
			return false;
		}
		if(!strcmp(argv[i], "-smod"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			_aaMassFile = utils::absPath(argv[i]);
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
		if(!strcmp(argv[i], "--version"))
		{
			std::cout << "annotate_ms2 " << BIN_VERSION << NEW_LINE;
			printGitVersion();
			return false;
		}
		else{
			std::cerr << argv[i] << " is an invalid argument." << NEW_LINE;
			usage();
			return false;
		}
		
	}
	
	//fix options
	if(_wd[_wd.length() - 1] != '/')
		_wd += "/";
	
	return checkParams();
}

bool Ms2_annotator::Params::checkParams() const
{
	if(!seqSpecified)
	{
		std::cerr << "Sequence must be specified" << NEW_LINE;
		usage();
		return false;
	}
	if(inputMode == 0)
	{
		if(!(scanSpecified && ms2Specified))
		{
			std::cerr << NEW_LINE << "Scan number and .ms2 file must be specified" << NEW_LINE;
			usage();//Ms2_annotator::PROG_USAGE_FNAME);
			return false;
		}
	}
	else if(inputMode == 1)
	{
		assert(dtaSpecified);
	}
	
	return true;
}


