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
 Read list of tsv formated peptide lists supplied by \p pars <br>
 \p ifname must have atleast columns with the headers in IonFinder::TSV_INPUT_REQUIRED_COLNAMES
 \param ifname path of .tsv file of peptides to search for
 \param scans empty list of scans to fill
 \returns true if all files were successfully read.
 */
bool IonFinder::readInputTsv(std::string ifname,
							 std::vector<Dtafilter::Scan>& scans)
{
	utils::TsvFile tsv(ifname);
	if(!tsv.read()) return false;
	
	//iterate through columns to make sure all required cols exist
	for(int i = 0; i < TSV_INPUT_REQUIRED_COLNAMES_LEN; i++)
	{
		if(!tsv.colExists(TSV_INPUT_REQUIRED_COLNAMES[i]))
		{
			std::cerr << "\nError! Required column: " << TSV_INPUT_REQUIRED_COLNAMES[i] <<
			" not found in " << ifname << NEW_LINE;
			return false;
		}
	}
	
	//itterate through columns and search for optional colums
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
		
		//requied columns
		temp.setScanNum(tsv.getValSize(i, "scanNum"));
		temp.setSequence(tsv.getValStr(i, "sequence"));
		temp.setPrecursorFile(tsv.getValStr(i, "precursorFile"));
		temp.setSampleName(tsv.getValStr(i, "sampleName"));
		
		//add optional columns which were found.
		if(foundOptionalCols["parentID"])
			temp.setParentID(tsv.getValStr(i, "parentID"));
		if(foundOptionalCols["parentProtein"])
			temp.setParentProtein(tsv.getValStr(i, "parentProtein"));
		if(foundOptionalCols["parentDescription"])
			temp.setParentDescription(tsv.getValStr(i, "parentDescription"));
		if(foundOptionalCols["matchDirection"])
			temp.setMatchDirection(Dtafilter::Scan::strToMatchDirection(tsv.getValStr(i, "matchDirection")));
		if(foundOptionalCols["fullSequence"])
			temp.setFullSequence(tsv.getValStr(i, "fullSequence"));
		if(foundOptionalCols["unique"])
			temp.setUnique(tsv.getValBool(i, "unique"));
		if(foundOptionalCols["charge"])
			temp.setCharge(tsv.getValInt(i, "charge"));
		if(foundOptionalCols["xcorr"])
			temp.setXcorr(tsv.getValStr(i, "xcorr"));
		if(foundOptionalCols["precursorMZ"])
			temp.setPrecursorMZ(tsv.getValStr(i, "precursorMZ"));
		if(foundOptionalCols["precursorScan"])
			temp.setPrecursorScan(tsv.getValStr(i, "precursorScan"));
		
		scans.push_back(temp);
	}
	
	return true;
}
