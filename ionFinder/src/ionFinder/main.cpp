//
//  main.cpp
//  ionFinder
//
//  Created by Aaron Maurais on 12/9/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#include <ionFinder/main.hpp>

int main(int argc, const char** argv)
{
	IonFinder::Params pars;
	if(!pars.getArgs(argc, argv))
		return -1;
	
	pars.printVersion(std::cout);
	
	//read input files
	std::vector<Dtafilter::Scan> scans;
	if(pars.getInputMode() == IonFinder::DTAFILTER_INPUT_STR)
	{
		std::cout << "\nReading DTAFilter-files...";
		if(!Dtafilter::readFilterFiles(pars, scans))
		{
			std::cerr << "Failed to read DTASelect-filter files!" << NEW_LINE;
			return -1;
		}
		else std::cout << "Done!\n";
	}
	else{
		assert(pars.getInputMode() == IonFinder::TSV_INPUT_STR);
		std::cout << "\nReading input .tsv files...";
		for(auto file: pars.getInputDirs())
		{
            if(!IonFinder::readInputTsv(file,scans,
                                         !pars.getIncludeReverse(), pars.getModFilter())) {
                std::cerr << "Failed to read input .tsv files!" << NEW_LINE;
                return -1;
            }
        }
		std::cout << "Done!\n";
	}
	
	//calculate and find fragments
	std::vector<PeptideNamespace::Peptide> peptides;
	peptides.reserve(scans.size());
	if(!IonFinder::findFragmentsParallel(scans, peptides, pars)){
		std::cout << "Failed to annotate spectra!" << std::endl;
	}

	/*
	std::ofstream outF("/Users/Aaron/local/ionFinder/testFiles/int_co_test.tsv");
	for(int i = 0; i < peptides.size(); i++){
	    peptides[i].printFragments(outF, (i == 0), true);
	}
	*/

	//analyze sequences
	std::cout << "\nAnalyzing peptide sequences...";
	std::vector<IonFinder::PeptideStats> peptideStats;
	if(!IonFinder::analyzeSequences(scans, peptides, peptideStats, pars))
		std::cout << NEW_LINE;
	std::cout << "Done!\n";
	
	//write data
	if(!IonFinder::printPeptideStats(peptideStats, pars))
	{
		std::cerr << "Failed to write peptide stats!" << NEW_LINE;
		return -1;
	}
	std::cout << "\nResults written to: " << pars.makeOfname() << NEW_LINE;
	
	return 0;
}
