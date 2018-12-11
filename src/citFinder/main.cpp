//
//  main.cpp
//  citFinder
//
//  Created by Aaron Maurais on 12/9/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#include <citFinder/main.hpp>

int main(int argc, const char** argv)
{
	CitFinder::Params pars;
	if(!pars.getArgs(argc, argv))
		return -1;
	
	std::cout << "citFinder v" << BIN_VERSION << NEW_LINE;
	
	//read filter files
	std::vector<CitFinder::Scan> scans;
	if(!CitFinder::readFilterFiles(pars, scans))
	{
		std::cerr << "Failed to read DTASelect-filter files!" << NEW_LINE;
		return -1;
	}
	
	//calculate fragments
	std::vector<PeptideNamespace::Peptide> peptides;
	CitFinder::calcFragments(peptides, scans, pars);
	
	//annotate spectra
	
	//write data
	
	return 0;
}
