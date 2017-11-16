//
//  ms2.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/6/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "ms2.hpp"

void ms2::Spectrum::printSpectrum(ostream& out, bool includeMetaData) const
{
	assert(out);
	if(includeMetaData)
	{
		out << "scanNumber" << OUT_DELIM << scanNumber << endl
			<< "retTime" << OUT_DELIM << retTime << endl << std::scientific
			<< "precursorInt" << OUT_DELIM << precursorInt << endl;
		out.unsetf(std::ios::scientific);
		out << "precursorFile" << OUT_DELIM << precursorFile << endl
			<< "precursorScan" << OUT_DELIM << precursorScan << endl;
	}
	
	for(size_t i = 0; i < NUM_SPECTRUM_COL_HEADERS_SHORT; i++)
	{
		if(i == 0)
		out << SPECTRUM_COL_HEADERS[i];
		else out << OUT_DELIM << SPECTRUM_COL_HEADERS[i];
	}
	out << endl;
	
	for(ionsTypeIt it1 = ions.begin(); it1 != ions.end(); ++it1)
		for(ionVecType::const_iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
			out << it2->getMZ() << OUT_DELIM << it2->getIntensity() << endl;
}

void ms2::Spectrum::printLabeledSpectrum(ostream& out, bool includeMetaData) const
{
	assert(out);
	if(includeMetaData)
	{
		out << "scanNumber" << OUT_DELIM << scanNumber << endl
			<< "retTime" << OUT_DELIM << retTime << endl << std::scientific
			<< "precursorInt" << OUT_DELIM << precursorInt << endl;
		out.unsetf(std::ios::scientific);
		out << "precursorFile" << OUT_DELIM << precursorFile << endl
			<< "precursorScan" << OUT_DELIM << precursorScan << endl
			<< "ionPercent" << OUT_DELIM << ionPercent << endl;
	}
	
	for(size_t i = 0; i < NUM_SPECTRUM_COL_HEADERS_LONG; i++)
	{
		if(i == 0)
			out << SPECTRUM_COL_HEADERS[i];
		else out << OUT_DELIM << SPECTRUM_COL_HEADERS[i];
	}
	out << endl;
	
	for(ionsTypeIt it1 = ions.begin(); it1 != ions.end(); ++it1)
		for(ionVecType::const_iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
			out << it2->getMZ() << OUT_DELIM
				<< it2->getIntensity() << OUT_DELIM
				<< it2->getLabel() << OUT_DELIM
				<< it2->getFormatedLabel() << OUT_DELIM
				<< it2->label.labelLoc.getX() << OUT_DELIM
				<< it2->label.labelLoc.getY() << endl;
}

void ms2::Spectrum::clear()
{
	scanNumber = 0;
	scanNumInt = 0;
	retTime = 0;
	precursorInt = 0;
	precursorFile.clear();
	precursorScan = 0;
	precursorCharge = 0;
	precursorMZ = 0;
	maxInt = 0;
	ions.clear();
}

template<typename _Tp>
void ms2::Spectrum::normalizeIonInts(_Tp _den)
{
	double den = getMaxIntensity() / _den;
	for(ionsType::iterator it1 = ions.begin(); it1 != ions.end(); ++it1)
		for(ionVecType::iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
			it2->normalizeIntensity(den);
	maxInt = _den;
	minInt = 0;
}

void ms2::Spectrum::setLabelTop(size_t labelTop)
{
	typedef list<ms2::DataPoint*> listType;
	listType pointList;
	
	for(ionsType::iterator it1 = ions.begin(); it1 != ions.end(); ++it1)
		for(ionVecType::iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
			pointList.push_back(&(*it2));
	
	if(pointList.size() > labelTop)
	{
		pointList.sort(ms2::DataPointComparison());
		pointList.resize(labelTop);
	}
	
	for(listType::iterator it = pointList.begin(); it != pointList.end(); ++it)
		(*it)->setTopAbundant(true);
}

void ms2::Spectrum::labelSpectrum(const peptide::Peptide& peptide, bool calcLables, size_t labelTop)
{
	size_t len = peptide.getNumFragments();
	size_t labledCount = 0;
	setLabelTop(labelTop);
	for(size_t i = 0; i < len; i++)
	{
		Ion::mz_intType tempInt = Ion::mz_intType(peptide.getFragmentMZ_int(i));
		if(ions.find(tempInt) == ions.end()) //check if ion with mz exists in spectrum
			continue;
		else //if it does exist do stuff
		{
			labledCount++;
			size_t vecLen = ions[tempInt].size();
			assert(vecLen > 0);
			if(vecLen == 1) //check if mz bin contains more than 1 ion
			{
				if(ions[tempInt][0].getTopAbundant())
				{
					ions[tempInt][0].setLabel(peptide.getFragmentLabel(i));
					ions[tempInt][0].setFormatedLabel(peptide.getFormatedLabel(i));
					ions[tempInt][0].setIncludeLabel(true);
				}
			}
			else{ //if not iterate through bin to find most intense ion
				ionVecType::iterator it, label;
				it = ions[tempInt].begin();
				label = it;
				//double diff = abs(it->getIntensity() - peptide.getFragmentMZ(i));
				double maxInt = it->getIntensity();
				for(; it != ions[tempInt].begin(); ++it)
				{
					//double temp = abs(it->getMZ() - peptide.getFragmentMZ(i));
					//if(temp > diff)
					if(it->getIntensity() > maxInt)
					{
						//diff = temp;
						label = it;
					}//end of if
				}//end of for
				if(label->getTopAbundant())
				{
					label->setLabel(peptide.getFragmentLabel(i));
					label->setFormatedLabel(peptide.getFormatedLabel(i));
					label->setIncludeLabel(true);
				}
			}//end of else
		}//end of else
	}//end of for
	ionPercent = (double(labledCount) / double(len)) * 100;
	
	if(calcLables)
		calcLabelPos();
}//end of function

void ms2::Spectrum::makePoints(labels::Labels& labs, double maxPerc,
							   double offset_x, double offset_y,
							   double x_padding, double y_padding)
{
	for(ionsType::iterator it = ions.begin(); it != ions.end(); ++it)
		for(ionVecType::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			if(it2->getIncludeLabel() || it2->getIntensity() >= maxPerc)
			{
				it2->label.labelLoc = geometry::Rect(it2->getMZ() + offset_x,
										 it2->getIntensity() + offset_y, y_padding, x_padding);
				labs.push_back(&(it2->label));
			}
}

void ms2::Spectrum::calcLabelPos()
{
	calcLabelPos(DEFAULT_MAX_PERC,
				 DEFAULT_X_OFFSET, DEFAULT_Y_OFFSET,
				 DEFAULT_X_PADDING, DEFAULT_Y_PADDING);
}

void ms2::Spectrum::calcLabelPos(double maxPerc,
								 double offset_x, double offset_y,
								 double x_padding, double y_padding)
{
	//initalize points
	labels::Labels labs(minMZ, maxMZ, minInt, maxInt);
	
	makePoints(labs, maxPerc, offset_x, offset_y, x_padding, y_padding);
	//labs.spaceOutAlg1();
	
	//copy corrected points to spectrum
	//copyPointsToSpectrum(points);
}

bool ms2::Ms2File::read(string _fname)
{
	fname = _fname;
	return read();
}

bool ms2::Ms2File::read()
{
	if(fname.empty())
		throw runtime_error("File must be specified!");
	ifstream inF(fname.c_str());
	if(!inF)
		return false;
	
	delimType = utils::detectLineEnding(inF);
	if(delimType == utils::unknown)
		throw runtime_error("Invalid delimiter in file: " + fname + "!");
	delim = utils::getDelimStr(delimType);
	if(delimType == utils::crlf)
		beginLine = 1;
	else beginLine = 0;
	
	inF.seekg(0, inF.end);
	size = inF.tellg();
	inF.seekg(0, inF.beg);
	buffer = new char [size];
	if(inF.read(buffer, size))
		return getMetaData();
	return false;
}

bool ms2::Ms2File::getMetaData()
{
	//find header in buffer and put it into ss
	stringstream ss;
	string line;
	size_t end = utils::offset(buffer, size, "LastScan") + 100;
	for(size_t i = 0; i < end; i++)
		ss.put(*(buffer + i));
	size_t sLen = ss.str().length();
	
	vector<string> elems;
	int mdCount = 0;
	char _delim = utils::getDelim(delimType);
	//iterate throuth ss to find metadata
	while(ss.tellg() < sLen){
		utils::getLine(ss, line, _delim, beginLine);
		if(line[0] == 'H')
		{
			utils::split(line, IN_DELIM, elems);
			if(elems[1] == "ScanType")
				scanType = elems[2];
			else if(elems[1] == "DataType")
				dataType = elems[2];
			else if(elems[1] == "FirstScan")
				firstScan = elems[2];
			else if(elems[1] == "LastScan")
				lastScan = elems[2];
			else continue;
			mdCount++;
		}
	}
	
	//check that md is good
	if(scanType == "MS2" &&
	   dataType == "Centroid" &&
	   utils::toInt(firstScan) <= utils::toInt(lastScan) &&
	   mdCount == mdNum)
		return true;
	return false;
}

const char* ms2::Ms2File::makeOffsetQuery(string queryScan) const
{
	size_t qsInt = utils::toInt(queryScan);
	return makeOffsetQuery(qsInt);
}

const char* ms2::Ms2File::makeOffsetQuery(size_t queryScan) const
{
	size_t numDigitsFinal = NUM_DIGITS_IN_SCAN;
	size_t numDigitsQuery = utils::numDigits(queryScan);
	int repeatNum = int(numDigitsFinal - numDigitsQuery);
	
	string ret = "S\t" +
		utils::repeat("0", repeatNum) + utils::toString(queryScan) +
		"\t" + utils::repeat("0", repeatNum) + utils::toString(queryScan);
	
	return ret.c_str();
}

bool ms2::Ms2File::getScan(string queryScan, Spectrum& scan) const
{
	return getScan(utils::toInt(queryScan), scan);
}

bool ms2::Ms2File::getScan(size_t queryScan, Spectrum& scan) const
{
	scan.clear();
	const char* query = makeOffsetQuery(queryScan);
	size_t scanOffset = utils::offset(buffer, size, query);
	if(scanOffset == size)
		return false;
	
	const char* _delim = delim.c_str();
	char* _scan = strtok(strdup(buffer + scanOffset), _delim);
	vector<string> elems;
	size_t numIons = 0;
	
	do{
		string line = string(_scan);
		utils::split(line, IN_DELIM, elems);
		if(_scan[0] == 'S')
		{
			assert(elems.size() == 4);
			utils::split(line, IN_DELIM, elems);
			scan.scanNumber = utils::toInt(elems[2]);
			scan.precursorMZ = utils::toDouble(elems[3]);
		}
		else if(_scan[0] == 'I')
		{
			assert(elems.size() == 3);
			utils::split(line, IN_DELIM, elems);
			if(elems[1] == "RetTime")
				scan.retTime = utils::toDouble(elems[2]);
			else if(elems[1] == "PrecursorInt")
				scan.precursorInt = utils::toDouble(elems[2]);
			else if(elems[1] == "PrecursorFile")
				scan.precursorFile = elems[2];
			else if(elems[1] == "PrecursorScan")
				scan.precursorScan = utils::toInt(elems[2]);
		}
		else if(_scan[0] == 'Z'){
			assert(elems.size() == 3);
			utils::split(line, IN_DELIM, elems);
			scan.precursorCharge = utils::toInt(elems[1]);
		}
		else if(utils::isInteger(string(1, _scan[0]))){
			
			utils::split(line, ' ', elems);
			assert(elems.size() == 2);
			DataPoint tempIon (utils::toDouble(elems[0]), utils::toDouble(elems[1]));
			
			if(numIons == 0)
			{
				scan.minMZ = tempIon.getMZ();
				scan.minInt = tempIon.getIntensity();
			}
			
			scan.ions[tempIon.getMZInt()].push_back(tempIon);
			numIons++;
			
			//get min and max vals
			if(tempIon.getIntensity() > scan.maxInt)
				scan.maxInt = tempIon.getIntensity();
			if(tempIon.getIntensity() < scan.minInt)
				scan.minInt = tempIon.getIntensity();
			if(tempIon.getMZ() > scan.maxMZ)
				scan.maxMZ = tempIon.getMZ();
			if(tempIon.getMZ() < scan.minMZ)
				scan.minMZ = tempIon.getMZ();
		}
		_scan = strtok(nullptr, _delim);
	} while(_scan != nullptr && _scan[0] != 'S');
	return true;
}




