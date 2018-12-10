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
	ms2::Ms2File ms2File;
	std::string curFname;
	for(auto it = scans.begin(); it != scans.end(); ++it)
	{
		PeptideNamespace::Peptide pep(it->getSequence());
		pep.initalize(pars);
		
		//read ms2 file if it hasn't been done yet
		if(curFname != it->getParentFile())
		{
			curFname = it->getParentFile();
			if(!ms2File.read(curFname))
			{
				std::cout << "Failed to read: " << it->getParentFile() << NEW_LINE;
				return -1;
			}
		}//end if
		
		
	}
	
	//annotate spectra
	
	//write data
	
	return 0;
}
