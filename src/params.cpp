//
//  params.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "../include/params.hpp"

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
			if(!utils::dirExists(wd))
			{
				cerr << "Specified direectory does not exist." << endl;
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
			inFile.sequence = string(argv[i]);
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
			inFile.scan = utils::toInt(argv[i]);
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
			inputMode = 1;
			continue;
		}
		if(!strcmp(argv[i], "--printSmod"))
		{
			if(!writeSmod(wd))
				cerr << "Could not write new smod file!" << endl;
			return false;
		}
		if(!strcmp(argv[i], "-mt") || !strcmp(argv[i], "--matchTolerance"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			matchTolerance = utils::toDouble(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-minC"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			minFragCharge = utils::toInt(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-maxC"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			maxFragCharge = utils::toInt(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-minLabInt"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			minLabelIntensity = utils::toInt(argv[i]);
			continue;
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
		cerr << endl << "Sequence must be specified" << endl;
		usage();
		return false;
	}
	if(inputMode == 0)
	{
		if(!(scanSpecified && ms2Specified))
		{
			cerr << endl << "Scan number and .ms2 file must be specified" << endl;
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
		cerr << file.getLine() << endl;
}

bool params::Params::writeSmod(string _wd) const
{
	if(_wd[_wd.length() - 1] != '/')
		_wd += "/";
	ofstream outF((_wd + params::DEFAULT_SMOD_NAME).c_str());
	utils::File staticMods(params::PROG_DEFAULT_SMOD_FILE);
	if(!outF || !staticMods.read())
		return false;
	
	if(wdSpecified)
		cerr << endl << "Generating " << _wd << DEFAULT_SMOD_NAME << endl;
	else cerr << endl <<"Generating ./" << DEFAULT_SMOD_NAME << endl;
	
	outF << utils::COMMENT_SYMBOL << " Static modifications for ms2_annotator" << endl
	<< utils::COMMENT_SYMBOL << " File generated on: " << utils::ascTime() << endl
	<< "<staticModifications>" << endl;
	
	while(!staticMods.end())
		outF << staticMods.getLine() << endl;
	
	outF << endl << "</staticModifications>" << endl;
	
	return true;
}





