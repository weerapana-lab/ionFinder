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
	std::cout << "\nReading DTAFilter-files...";
	std::vector<Dtafilter::Scan> scans;
	if(!Dtafilter::readFilterFiles(pars, scans))
	{
		std::cerr << "Failed to read DTASelect-filter files!" << NEW_LINE;
		return -1;
	}
	std::cout << "Done!\n";
	
	//calculate and find fragments
	std::vector<PeptideNamespace::Peptide> peptides;
	if(!CitFinder::findFragmentsParallel(scans, peptides, pars)){
		std::cout << "Failed to annotate spectra!" << std::endl;
	}
	
	//analyze sequences
	std::cout << "\nAnalyzing peptide sequences...";
	std::vector<CitFinder::PeptideStats> peptideStats;
	if(!CitFinder::analyzeSequences(scans, peptides, peptideStats, pars))
		std::cout << NEW_LINE;
	std::cout << "Done!\n";
	
	//write data
	std::string ofname = pars.makeOfname();
	if(!CitFinder::printPeptideStats(peptideStats, ofname))
	{
		std::cerr << "Failed to write peptide stats!" << NEW_LINE;
		return -1;
	}
	std::cout << "\nResults written to: " << ofname << NEW_LINE;
	
	return 0;
}
