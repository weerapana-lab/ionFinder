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
#include <tsv_constants.hpp>

namespace Dtafilter{
	bool readFilterFiles(const IonFinder::Params&, std::vector<Dtafilter::Scan>&);
}

namespace IonFinder{
	const std::string TSV_INPUT_REQUIRED_COLNAMES [] = {SAMPLE_NAME, SEQUENCE, PRECURSOR_FILE, SCAN_NUM};
	int const TSV_INPUT_REQUIRED_COLNAMES_LEN = 4;
	const std::string TSV_INPUT_OPTIONAL_COLNAMES [] = {PARENT_ID, PARENT_PROTEIN, PARENT_DESCRIPTION, MATCH_DIRECTION, FULL_SEQUENCE, UNIQUE, CHARGE, XCORR, PRECURSOR_MZ, PRECURSOR_SCAN};
	int const TSV_INPUT_OPTIONAL_COLNAMES_LEN = 10;
	
	bool readInputTsv(std::string ifname, std::vector<Dtafilter::Scan>&scans,
					  bool skipReverse = false, int modFilter = 1);
}

#endif /* inputFiles_hpp */
