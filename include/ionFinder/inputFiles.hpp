//
//  inputFiles.hpp
//  ionFinder
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
	const std::string TSV_INPUT_REQUIRED_COLNAMES [] = {"sampleName", "sequence", "precursorFile", "scanNum"};
	int const TSV_INPUT_REQUIRED_COLNAMES_LEN = 4;
	const std::string TSV_INPUT_OPTIONAL_COLNAMES [] = {"parentID", "parentProtein", "parentDescription", "matchDirection", "fullSequence", "unique", "charge", "xcorr", "precursorMZ", "precursorScan"};
	int const TSV_INPUT_OPTIONAL_COLNAMES_LEN = 10;
	
	bool readInputTsv(std::string ifname, std::vector<Dtafilter::Scan>&);
}

#endif /* inputFiles_hpp */
