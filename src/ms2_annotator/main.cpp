//
//  main.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 3/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <ms2_annotator/main.hpp>

int main(int argc, char* argv[])
{
	//get args
	Ms2_annotator::Params pars;
	if(!pars.getArgs(argc, argv))
		return -1;
	
	std::cout << "ms2_annotator v" << BIN_VERSION << NEW_LINE;
	
	scanData::scansType scans;
	//get scan numbers from DTAFilter file if supplied by user
	if(pars.getInputMode() == 1)
	{
		Ms2_annotator::DtaFilterFile filterFile(pars.inFile.getInfile());
		if(!filterFile.read())
		{
			std::cout << "Failed to read DTAFilter file" << NEW_LINE;
			return -1;
		}
		if(!filterFile.getScan(pars.inFile.getSeq(), scans, pars.getForce()))
		{
			std::cout << "Scan not found" << NEW_LINE;
			return -1;
		}
	}
	else if(pars.getInputMode() == 0)
	{
		scans.push_back(scanData::Scan(pars.inFile.getSeq(),
									   pars.inFile.getScan(), pars.inFile.getInfile()));
	}
	
	aaDB::AADB aadb;
	for(scanData::scansType::iterator it = scans.begin(); it != scans.end(); ++it)
	{
		//initialize peptide with user supplied sequence
		PeptideNamespace::Peptide pep(it->getSequence());
		PeptideNamespace::initAminoAcidsMasses(pars, aadb);
		
		pep.initialize(pars, aadb);
		
		ms2::Ms2File file(it->getPrecursorFile());
		if(!file.read())
		{
			std::cout << "Failed to read: " << it->getPrecursorFile() << NEW_LINE;
			return -1;
		}
		
		ms2::Spectrum spectrum;
		if(!file.getScan(it->getScanNum(), spectrum))
		{
			std::cout << "scan not found!" << NEW_LINE;
			return -1;
		}
		else {
			if(pars.getInputMode() == 0)
				it->setCharge(spectrum.getCharge());
			spectrum.labelSpectrum(pep, pars);
			spectrum.normalizeIonInts(100);
			spectrum.calcLabelPos();
			std::ofstream outF(it->getOfname().c_str());
			spectrum.printLabeledSpectrum(outF, true);
			if(pars.getWDSpecified())
				std::cout << "Spectrum file written to " << it->getOfname() << NEW_LINE;
			else std::cout << "Spectrum file written to ./" << utils::baseName(it->getOfname()) << NEW_LINE;
		}
	}
	
	return 0;
}
