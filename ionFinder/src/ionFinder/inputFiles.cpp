//
// inputFiles.cpp
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
		if(!Dtafilter::readFilterFile(it->second, it->first, scans,
									  !params.getIncludeReverse(), params.getModFilter()))
			return false;
	}
	
	return true;
}

/*
 sampleName
 parentID
 parentProtein
 parentDescription
 matchDirection
 sequence
 fullSequence
 unique
 charge
 xcorr
 scanNum
 precursorMZ
 precursorScan
 precursorFile
 */

/**
 Read list of tsv formatted peptide lists supplied by \p pars <br>
 \p ifname must have at least columns with the headers in IonFinder::TSV_INPUT_REQUIRED_COLNAMES
 \param ifname path of .tsv file of peptides to search for
 \param scans empty list of scans to fill
 \param skipReverse Should reverse peptide matches be skipped?
 \param modFilter Which scans should be added to \p scans?
 0: only modified, 1: all peptides regardless of modification, 2: only unmodified pepeitde.
 
 \returns true if all files were successfully read.
 */
bool IonFinder::readInputTsv(const std::string& ifname,
							 std::vector<Dtafilter::Scan>& scans,
							 bool skipReverse, int modFilter)
{
	utils::TsvFile tsv(ifname);
	if(!tsv.read()) return false;
	
	//iterate through columns to make sure all required cols exist
	for(const auto & i : TSV_INPUT_REQUIRED_COLNAMES) {
		if(!tsv.colExists(i))
		{
			std::cerr << "\nError! Required column: " << i <<
			" not found in " << ifname << NEW_LINE;
			return false;
		}
	}
	
	//itterate through columns and search for optional columns
	std::map<std::string, bool> foundOptionalCols;
	for(int i = 0; i < TSV_INPUT_OPTIONAL_COLNAMES_LEN; i++)
	{
		if(tsv.colExists(TSV_INPUT_OPTIONAL_COLNAMES[i]))
			foundOptionalCols[TSV_INPUT_OPTIONAL_COLNAMES[i]] = true;
		else foundOptionalCols[TSV_INPUT_OPTIONAL_COLNAMES[i]] = false;
	}
	
	size_t nRow = tsv.getNrow();
	for(size_t i = 0; i < nRow; i++)
	{
		Dtafilter::Scan temp;
		temp.setMatchDirection(Dtafilter::Scan::MatchDirection::FORWARD);

		//required columns
		temp.setScanNum(tsv.getValSize(i, IonFinder::SCAN_NUM));
		temp.setSequence(tsv.getValStr(i, IonFinder::SEQUENCE));
        temp.setIsModified(temp.checkIsModified());
		temp.getPrecursor().setFile(tsv.getValStr(i, IonFinder::PRECURSOR_FILE));
		temp.setSampleName(tsv.getValStr(i, IonFinder::SAMPLE_NAME));

		//add optional columns which were found.
		if(foundOptionalCols[IonFinder::PARENT_ID])
			temp.setParentID(tsv.getValStr(i, IonFinder::PARENT_ID));
		if(foundOptionalCols[IonFinder::PARENT_PROTEIN])
			temp.setParentProtein(tsv.getValStr(i, IonFinder::PARENT_PROTEIN));
		if(foundOptionalCols[IonFinder::PARENT_DESCRIPTION])
			temp.setParentDescription(tsv.getValStr(i, IonFinder::PARENT_DESCRIPTION));
		if(foundOptionalCols[IonFinder::MATCH_DIRECTION])
			temp.setMatchDirection(Dtafilter::Scan::strToMatchDirection(tsv.getValStr(i, IonFinder::MATCH_DIRECTION)));
        if(foundOptionalCols[IonFinder::FORMULA])
            temp.setFormula(tsv.getValStr(i, IonFinder::FORMULA));
		if(foundOptionalCols[IonFinder::FULL_SEQUENCE])
			temp.setFullSequence(tsv.getValStr(i, IonFinder::FULL_SEQUENCE));
		if(foundOptionalCols[IonFinder::UNIQUE])
			temp.setUnique(tsv.getValBool(i, IonFinder::UNIQUE));
		if(foundOptionalCols[IonFinder::CHARGE])
			temp.getPrecursor().setCharge(tsv.getValInt(i, IonFinder::CHARGE));
		if(foundOptionalCols[IonFinder::SCORE])
			temp.setXcorr(tsv.getValStr(i, IonFinder::SCORE));
		if(foundOptionalCols[IonFinder::PRECURSOR_MZ])
			temp.getPrecursor().setMZ(tsv.getValStr(i, IonFinder::PRECURSOR_MZ));
		if(foundOptionalCols[IonFinder::PRECURSOR_SCAN])
            temp.getPrecursor().setScan(tsv.getValStr(i, IonFinder::PRECURSOR_SCAN));
		
		//reverse match filter
		if(skipReverse && temp.getMatchDirection() == Dtafilter::Scan::MatchDirection::REVERSE)
			continue;
		
		//mod filter
		if((modFilter == 0 && !temp.isModified()) ||
		   (modFilter == 2 && temp.isModified()))
			continue;
		
		scans.push_back(temp);
	}
	
	return true;
}
