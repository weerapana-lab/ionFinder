//
// inputFiles.hpp
// ionFinder
// -----------------------------------------------------------------------------
// MIT License
// Copyright 2020 Aaron Maurais
// -----------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -----------------------------------------------------------------------------
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
	const std::string TSV_INPUT_OPTIONAL_COLNAMES [] = {PARENT_ID, PARENT_PROTEIN, PARENT_DESCRIPTION, MATCH_DIRECTION,
                                                        FORMULA, FULL_SEQUENCE, UNIQUE, CHARGE, SCORE, PRECURSOR_MZ,
                                                        PRECURSOR_SCAN};
	int const TSV_INPUT_OPTIONAL_COLNAMES_LEN = 10;
	
	bool readInputTsv(const std::string& ifname, std::vector<Dtafilter::Scan>&scans,
					  bool skipReverse = false, int modFilter = 1);
}

#endif /* inputFiles_hpp */
