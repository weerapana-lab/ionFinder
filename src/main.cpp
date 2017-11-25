//
//  main.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 3/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "../include/ms2_annotator.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	//get args
	params::Params pars;
	if(!pars.getArgs(argc, argv))
		return -1;
	
	scanData::scansType scans;
	//get scan numbers from DTAFilter file if suplied by user
	if(pars.getInputMode() == 1)
	{
		dtafilter::DtaFilterFile filterFile(pars.inFile.getInfile());
		if(!filterFile.read())
		{
			cout << "Failed to read DTAFilter file" << endl;
			return -1;
		}
		if(!filterFile.getScan(pars.inFile.getSeq(), scans, pars.getForce()))
			cout << "Scan not found" << endl;
		
		for(scanData::scansType::iterator it = scans.begin(); it != scans.end(); ++it)
			it->setParentFile(pars.getWD() + it->getParentFile());
	}
	else if(pars.getInputMode() == 0)
	{
		scans.push_back(scanData::Scan(pars.inFile.getSeq(),
					   pars.inFile.getScan(), pars.inFile.getInfile()));
	}
	
	for(scanData::scansType::const_iterator it = scans.begin(); it != scans.end(); ++it)
	{
		//initalize peptide with user suplied sequence
		peptide::Peptide pep(it->getSequence());
		pep.initalize(pars);
		
		ms2::Ms2File file(it->getParentFile());
		if(!file.read())
			cout << "Failed to read!" << endl;
		
		ms2::Spectrum spectrum;
		if(!file.getScan(it->getScanNum(), spectrum))
			cout << "scan not found" << endl;
		else {
			spectrum.normalizeIonInts(100);
			spectrum.labelSpectrum(pep);
			spectrum.calcLabelPos();
			ofstream outF(it->getOfname());
			spectrum.printLabeledSpectrum(outF, true);
			cout << "Spectrum file written to ./" << utils::baseName(it->getOfname()) << endl;
		}
	}
	
	return 0;
}
