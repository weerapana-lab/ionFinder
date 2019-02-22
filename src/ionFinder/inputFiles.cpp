//
//  dtafilterFiles.cpp
//  citFinder
//
//  Created by Aaron Maurais on 1/2/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#include <ionFinder/inputFiles.hpp>

/**
 Read list of filter files supplied by \p params
 \param params initialized Params object
 \param scans empty list of scans to fill
 \returns true if all files were successfully read.
 */
bool Dtafilter::readFilterFiles(const IonFinder::Params& params,
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
