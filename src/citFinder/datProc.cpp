//
//  datProc.cpp
//  citFinder
//
//  Created by Aaron Maurais on 12/10/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#include <citFinder/datProc.hpp>

bool CitFinder::calcFragments(std::vector<PeptideNamespace::Peptide>& peptides,
							  const std::vector<CitFinder::Scan>& scans,
							  CitFinder::Params& pars)
{
	std::map<std::string, ms2::Ms2File> ms2Map;
	std::string curSample;
	std::string curWD;
	for(auto it = scans.begin(); it != scans.end(); ++it)
	{
		//read ms2 file if it hasn't been done yet
		if(curSample != it->getSampleName())
		{
			//read ms2 files
			ms2Map.clear();
			//first get curent wd name
			curSample = it->getSampleName();
			curWD = pars.getInputModeIndependentParentDir() + "/" + curSample;
			
			//next get names of all ms2 files in dir
			std::vector<std::string> ms2FileNames;
			if(!utils::ls(curWD.c_str(), ms2FileNames, ".ms2"))
			{
				std::cerr << "Error reading ms2 files!" << NEW_LINE;
				return false;
			}
			
			//finally read each ms2 file into map
			for(auto it2 = ms2FileNames.begin(); it2 != ms2FileNames.end(); ++it2)
			{
				ms2Map[*it2] = ms2::Ms2File();
				if(!ms2Map[*it2].read(curWD + "/" + *it2))
				{
					std::cerr << "Error reading ms2 files!" << NEW_LINE;
					return false;
				}
			}
			
			//re-init Peptide::AminoAcidMasses for each sample
			std::string temp = pars.getWD() + "/sequest.params";
			pars.setSeqParFname(temp);
			PeptideNamespace::Peptide::initAminoAcidsMasses(pars);
		}//end if
		
		//initilize peptide object for current scan
		peptides.push_back(PeptideNamespace::Peptide (it->getSequence()));
		peptides.back().initalize(pars);
		
		//calculate neutral loss combinations
		int nMods = peptides.back().getNumMod();
		double nlMass = pars.getNeutralLossMass();
		std::vector<double> neutralLossIons;
		for(int i = 1; i <= nMods; i++)
			neutralLossIons.push_back(i * nlMass);
		
		//add neutral loss fragments to current peptide
		peptides.back().addNeutralLoss(neutralLossIons);
		
		//load spectrum
		ms2::Spectrum spectrum;
		if(!ms2Map[it->getParentFile()].getScan(it->getScanNum(), spectrum))
		{
			std::cout << "Error reading scan!" << NEW_LINE;
			return false;
		}
		spectrum.labelSpectrum(peptides.back(), pars);
	}
	return true;
}
