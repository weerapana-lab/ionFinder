//
//  params.cpp
//  ionFinder
//
//  Created by Aaron Maurais on 12/9/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#include <ionFinder/params.hpp>

/**
 Calculates number of supported threads using std::thread::hardware_concurrency(). <br><br>
 By default returns result of thread::hardware_concurrency() / 2.<br>
 If thread::hardware_concurrency returns 0, indicating an error, 1 thread is used.
 \return number of supported threads
 */
unsigned int IonFinder::Params::computeThreads() const
{
	unsigned int ret = std::thread::hardware_concurrency() / 2;
	if(ret == 0){
		std::cerr << "\nError detecting hardware_concurrency. Only 1 thread being used." << NEW_LINE;
		return 1;
	}
	return ret;
}

/**
 Parses command line arguments and stores in Params object
 \pre current working directory exists
 \param argc argc from main
 \param argv argv from main
 \return true if args were successfully read.
 */
bool IonFinder::Params::getArgs(int argc, const char* const argv[])
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
		if(!strcmp(argv[i], "-i") || !strcmp(argv[i], "--inputMode"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			if(!(!strcmp(argv[i], DTAFILTER_INPUT_STR.c_str()) || !strcmp(argv[i], TSV_INPUT_STR.c_str())))
			{
				std::cerr << argv[i] << base::PARAM_ERROR_MESSAGE << "inputMode" << std::endl;
				return false;
			}
			_inputMode = argv[i];
			continue;
		}
		if(!strcmp(argv[i], "-o") || !strcmp(argv[i], "--ofname"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			ofname = argv[i];
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
		if(!strcmp(argv[i], "--fastaFile"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			_fastaFile = utils::absPath(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--printSpectra"))
		{
			_printSpectraFiles = true;
			continue;
		}
		if(!strcmp(argv[i], "--calcNL"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			if(!(!strcmp(argv[i], "0") || !strcmp(argv[i], "1")))
			{
				std::cerr << argv[i] << base::PARAM_ERROR_MESSAGE << "calcNL" << NEW_LINE;
				return false;
			}
			_calcNL = std::stoi(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--lossMass"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			//!TODO: Maybe add option to search for multiple loeese
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
		if(!strcmp(argv[i], "--matchType"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			_matchType = strToMatchType(std::string(argv[i]));
			if(_matchType == MatchType::UNKNOWN){
				std::cerr << argv[i] << " is an unknown MatchType!" << NEW_LINE;
				return false;
			}
			continue;
		}
		if(!strcmp(argv[i], "--citStats"))
		{
			_neutralLossMass = CIT_NL_MASS;
			_ambigiousResidues = CIT_AMB_RESIDUES;
			_calcNL = true;
			ofname = PEPTIDE_CIT_STATS_OFNAME;
			continue;
		}
		if(!strcmp(argv[i], "--isoAA"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			_ambigiousResidues = std::string(argv[i]);
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
		if(!strcmp(argv[i], "-y") || !strcmp(argv[i], "--height"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			plotHeight = std::stod(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-w") || !strcmp(argv[i], "--width"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			plotWidth = std::stod(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-mmComp"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			if(!(!strcmp(argv[i], "intensity") || !strcmp(argv[i], "mz")))
			{
				std::cerr << argv[i] << base::PARAM_ERROR_MESSAGE << "mmComp" << NEW_LINE;
				return false;
			}
			multipleMatchCompare = std::string(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "--incAllIons"))
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
		if(!strcmp(argv[i], "--nThread"))
		{
			if(!utils::isArg(argv[++i]))
			{
				usage();
				return false;
			}
			_numThread = std::stoi(argv[i]);
			continue;
		}
		if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose"))
		{
			verbose = true;
			continue;
		}
		if(!strcmp(argv[i], "--version"))
		{
			std::cout << "ionFinder " << BIN_VERSION << NEW_LINE;
			printGitVersion();
			return false;
		}
		else if(utils::isFlag(argv[i])){
			std::cerr << argv[i] << " is an invalid argument." << NEW_LINE;
			usage();
			return false;
		}
		else{ //we are in input dirs
			while(i < argc){
				if(utils::isFlag(argv[i])){
					usage();
					return false;
				}
				_inDirs.push_back(std::string(argv[i++]));
				_inDirSpecified = true;
			}
		}
		//end of else
	}//end of for i
	
	//fix options
	if(_wd[_wd.length() - 1] != '/')
		_wd += "/";
	if(_inputMode == DTAFILTER_INPUT_STR){
		if(!getFlist()){
			std::cerr << "Could not find DTAFilter-files!" << NEW_LINE;
			return false;
		}
	}
	
	return true;
}//end of getArgs

/**
 Searches all directories in _inDirs for DTAFilter files.
 If _inDirs is empty, current working directory is used.
 \return true if > 1 filter file was found, else false
 */
bool IonFinder::Params::getFlist()
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
