//
// main.cpp
// ionFinder
// -----------------------------------------------------------------------------
// MIT License
// Copyright 2020 Aaron Maurais
// -----------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -----------------------------------------------------------------------------
//

#include <ionFinder/main.hpp>

int main(int argc, const char** argv)
{
	IonFinder::Params pars;
	if(!pars.getArgs(argc, argv))
		return 1;
	
	pars.printVersion(std::cout);

	//read input files
	std::vector<Dtafilter::Scan> scans;
	if(pars.getInputMode() == IonFinder::DTAFILTER_INPUT_STR)
	{
		std::cout << "\nReading DTAFilter-files...";
		if(!Dtafilter::readFilterFiles(pars, scans))
		{
			std::cerr << "Failed to read DTASelect-filter files!" << NEW_LINE;
			return 1;
		}
		else std::cout << "Done!\n";
	}
	else{
		assert(pars.getInputMode() == IonFinder::TSV_INPUT_STR);
		std::cout << "\nReading input .tsv files...";
		for(auto file: pars.getInputDirs())
		{
            if(!IonFinder::readInputTsv(file,scans, !pars.getIncludeReverse(), pars.getModFilter())) {
                std::cerr << "Failed to read input .tsv files!" << NEW_LINE;
                return 1;
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

	/*
	assert(IonFinder::printFragmentIntensities(peptideStats,
			"fragment_intensities_" +
			std::to_string(int(pars.getArtifactNLIntFrac() * 100)) + ".tsv"));
    */
	
	//write data
	if(!IonFinder::printPeptideStats(peptideStats, pars))
	{
		std::cerr << "Failed to write peptide stats!" << NEW_LINE;
		return 1;
	}
	std::cout << "\nResults written to: " << pars.makeOfname() << NEW_LINE;
	
	return 0;
}
