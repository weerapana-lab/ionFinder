//
//  inputFiles.hpp
//  citFinder
//
//  Created by Aaron Maurais on 1/2/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#ifndef inputFiles_hpp
#define inputFiles_hpp

#include <dtafilter.hpp>
#include <ionFinder/params.hpp>
#include <scanData.hpp>
#include <utils.hpp>
#include <tsvFile.hpp>

namespace Dtafilter{
	bool readFilterFiles(const IonFinder::Params&, std::vector<Dtafilter::Scan>&);
}

namespace IonFinder{
	const std::string TSV_INPUT_REQUIRED_COLNAMES [] = {"sampleName", "parentID", "parentProtein", "parentDescription", "matchDirection", "sequence", "fullSequence", "unique", "charge", "xcorr", "precursorMZ", "precursorScan", "precursorFile", "scanNum", "sampleName"};
	int const TSV_INPUT_REQUIRED_COLNAMES_LEN = 15;
	
	bool readInputTsv(std::string ifname, std::vector<Dtafilter::Scan>&);
}

#endif /* inputFiles_hpp */
