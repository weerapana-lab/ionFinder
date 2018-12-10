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
	citFinder::Params pars;
	if(!pars.getArgs(argc, argv))
		return -1;
	
	std::cout << "citFinder v" << BIN_VERSION << NEW_LINE;
	
	//read filter files
	std::vector<citFinder::Scan> scans;
	if(!citFinder::readFilterFiles(pars, scans))
	{
		std::cerr << "Failed to read DTASelect-filter files!" << NEW_LINE;
		return -1;
	}
	
	//calculate fragments
	for(auto it = scans.begin(); it != scans.end(); ++it)
	{
		PeptideNamespace::Peptide pep(it->getSequence());
		pep.initalize(pars);
		
		ms2::Ms2File file(it->getParentFile());
		if(!file.read())
		{
			std::cout << "Failed to read: " << it->getParentFile() << NEW_LINE;
			return -1;
		}
	}
	
	//annotate spectra
	
	//write data
	
	return 0;
}
