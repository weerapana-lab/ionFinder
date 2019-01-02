//
//  dtafilterFiles.cpp
//  citFinder
//
//  Created by Aaron Maurais on 1/2/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#include <citFinder/dtafilterFiles.hpp>

bool Dtafilter::readFilterFiles(const CitFinder::Params& params,
								std::vector<Dtafilter::Scan>& scans)
{
	auto endIt = params.getFilterFiles().end();
	for(auto it = params.getFilterFiles().begin(); it != endIt; ++it)
	{
		if(!Dtafilter::readFilterFile(it->second, it->first, scans, !params.getIncludeReverse()))
			return false;
	}
	
	return true;
}
