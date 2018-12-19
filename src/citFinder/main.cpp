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
	std::vector<Dtafilter::Scan> scans;
	if(!Dtafilter::readFilterFiles(pars, scans))
	{
		std::cerr << "Failed to read DTASelect-filter files!" << NEW_LINE;
		return -1;
	}
	
	//calculate and find fragments
	std::vector<PeptideNamespace::Peptide> peptides;
	CitFinder::findFragments(scans, peptides, pars);
	
	//analyze sequences
	std::vector<CitFinder::PeptideStats> peptideStats;
	CitFinder::analyzeSequences(scans, peptides, peptideStats, pars);
	
	//write data
	std::ofstream outF(pars.makeOfname());
	if(!outF)
	{
		std::cerr << "Failed to write peptide stats!" << NEW_LINE;
		return -1;
	}
	CitFinder::printPeptideStats(peptideStats, outF);
	
	return 0;
}