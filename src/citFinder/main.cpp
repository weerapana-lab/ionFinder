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
	
	std::vector<citFinder::Scan> scans;
	if(!citFinder::readFilterFiles(pars.getFilterFiles(), scans))
	{
		std::cerr << "Failed!" << NEW_LINE;
		return -1;
	}
	
	return 0;
}
