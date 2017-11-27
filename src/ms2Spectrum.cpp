//
//  ms2Spectrum.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "../include/ms2Spectrum.hpp"

void ms2::Spectrum::writeMetaData(ostream& out) const
{
	assert(out);
	out << ms2::BEGIN_METADATA << endl
	<< "precursorFile" << OUT_DELIM << precursorFile << endl
	<< "scanNumber" << OUT_DELIM << scanNumber << endl
	<< "retTime" << OUT_DELIM << retTime << endl
	<< "precursorCharge" << OUT_DELIM << precursorCharge << endl
	<< "precursorInt" << OUT_DELIM << std::scientific << precursorInt << endl;
	out.unsetf(std::ios::scientific);
	out << "precursorScan" << OUT_DELIM << precursorScan << endl
	<< ms2::END_METADATA << endl << ms2::BEGIN_SPECTRUM << endl;
}

void ms2::Spectrum::printSpectrum(ostream& out, bool includeMetaData) const
{
	assert(out);
	if(includeMetaData)
		writeMetaData(out);
	
	for(size_t i = 0; i < NUM_SPECTRUM_COL_HEADERS_SHORT; i++)
	{
		if(i == 0)
			out << SPECTRUM_COL_HEADERS[i];
		else out << OUT_DELIM << SPECTRUM_COL_HEADERS[i];
	}
	out << endl;
	
	for(ionsTypeConstIt it = ions.begin(); it != ions.end(); ++it)
		out << it->getMZ() << OUT_DELIM << it->getIntensity() << endl;
	
	if(includeMetaData)
		out << ms2::END_SPECTRUM << endl;
}

void ms2::Spectrum::printLabeledSpectrum(ostream& out, bool includeMetaData) const
{
	assert(out);
	if(includeMetaData)
		writeMetaData(out);
	
	for(size_t i = 0; i < NUM_SPECTRUM_COL_HEADERS_LONG; i++)
	{
		if(i == 0)
			out << SPECTRUM_COL_HEADERS[i];
		else out << OUT_DELIM << SPECTRUM_COL_HEADERS[i];
	}
	out << endl;
	
	for(ionsTypeConstIt it = ions.begin(); it != ions.end(); ++it)
		out << it->getMZ() << OUT_DELIM
		<< it->getIntensity() << OUT_DELIM
		<< it->getLabel() << OUT_DELIM
		<< it->getIncludeLabel() << OUT_DELIM
		<< it->getIonType() << OUT_DELIM
		<< it->getFormatedLabel() << OUT_DELIM
		<< it->label.labelLoc.getX() << OUT_DELIM
		<< it->label.labelLoc.getY() << endl;
	
	if(includeMetaData)
		out << ms2::END_SPECTRUM << endl;
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
	for(ionVecType::iterator it = ions.begin(); it != ions.end(); ++it)
		it->normalizeIntensity(den);
	maxInt = _den;
	minInt = 0;
}

void ms2::Spectrum::setLabelTop(size_t labelTop)
{
	typedef list<ms2::DataPoint*> listType;
	listType pointList;
	
	for(ionVecType::iterator it = ions.begin(); it != ions.end(); ++it)
		pointList.push_back(&(*it));
	
	if(pointList.size() > labelTop)
	{
		pointList.sort(ms2::DataPointIntComparison());
		pointList.resize(labelTop);
	}
	
	for(listType::iterator it = pointList.begin(); it != pointList.end(); ++it)
		(*it)->setTopAbundant(true);
}

void ms2::Spectrum::setMZRange(double minMZ, double maxMZ, bool _sort)
{
	//sort ions by mz
	if(_sort)
		sort(ions.begin(), ions.end(), DataPointMZComparison());
	
	ionsTypeIt begin = ions.begin();
	ionsTypeIt end = ions.end();
	
	//find min mz
	for(ionsTypeIt it = ions.begin(); it != ions.end(); ++it)
	{
		if(it->getMZ() <= minMZ)
			continue;
		else{
			begin = it;
			break;
		}
	}
	//find max mz
	for(ionsTypeIt it = begin; it != ions.end(); ++it)
	{
		if(it->getMZ() < maxMZ)
		{
			end = it;
			continue;
		}
		else break;
	}
	
	minMZ = begin->getMZ();
	maxMZ = end->getMZ();
	mzRange = maxMZ - minMZ;
	ions = ionVecType(begin, end);
}

void ms2::Spectrum::removeUnlabeledIons()
{
	for(ionsTypeIt it = ions.begin(); it != ions.end();)
		if(!it->getForceLabel())
			ions.erase(it);
		else ++it;
	
	minMZ = ions.begin()->getMZ();
	maxMZ = ions.end()->getMZ();
	mzRange = maxMZ - minMZ;
}

void ms2::Spectrum::removeIntensityBelow(double minInt)
{
	for(ionsTypeIt it = ions.begin(); it != ions.end();)
		if(it->getIntensity() < minInt)
			ions.erase(it);
		else ++it;
	
	minMZ = ions.begin()->getMZ();
	maxMZ = ions.end()->getMZ();
	mzRange = maxMZ - minMZ;
}

ms2::Spectrum::ionsTypeIt ms2::Spectrum::lowerBound(double _mz)
{
	ionsTypeIt it, first;
	first = ions.begin();
	iterator_traits<ionsTypeIt>::difference_type count, step;
	count = std::distance(first, ions.end());
	
	while(count > 0)
	{
		it = first;
		step = count / 2;
		std::advance(it, step);
		if(it->getMZ() < _mz)
		{
			first = ++it;
			count -= step + 1;
		}
		else count = step;
	}
	return first;
}

void ms2::Spectrum::labelSpectrum(const peptide::Peptide& peptide,
								  const params::Params& pars, bool calcLables, size_t labelTop)
{
	size_t len = peptide.getNumFragments();
	size_t labledCount = 0;
	sequence = peptide.getSequence();
	
	typedef list<ms2::DataPoint*> listType;
	listType rangeList;
	listType::iterator label;
	
	setLabelTop(labelTop);
	sort(ions.begin(), ions.end(), DataPointMZComparison()); //sort ions by mz
	if(pars.getMZSpecified())
		setMZRange(pars.getMinMZ(), pars.getMaxMZ(), false);
	
	for(size_t i = 0; i < len; i++)
	{
		double tempMZ = peptide.getFragmentMZ(i);
		rangeList.clear();
		
		//get ions whithin labelTolerance
		for(ionsTypeIt it = ions.begin(); it != ions.end(); ++it)
			if(it->getTopAbundant())
				if(utils::inRange(it->getMZ(), tempMZ, pars.getMatchTolerance()) //check that it->mz is in range
				   && (it->getIntensity() > pars.getMinLabelIntensity())) //check that it->int is sufficiently high
					rangeList.push_back(&(*it));
		
		if(rangeList.size() == 0)
			continue;
		
		label = rangeList.begin();
		if(rangeList.size() == 1)
		{
			(*label)->setLabel(peptide.getFragmentLabel(i));
			(*label)->setFormatedLabel(peptide.getFormatedLabel(i));
			(*label)->setLabeledIon(true);
			(*label)->label.setIncludeLabel(true);
			(*label)->setIonType(string(1, peptide.getBY(i)));
		}
		else
		{
			//iterate through all ions whithin matchTolerance range to get one with max intensity
			double tempMax = (*label)->getIntensity();
			for(listType::iterator it = rangeList.begin(); it != rangeList.end(); ++it)
			{
				if((*it)->getIntensity() > tempMax)
					label = it;
			}
			
			(*label)->setLabel(peptide.getFragmentLabel(i));
			(*label)->setFormatedLabel(peptide.getFormatedLabel(i));
			(*label)->setLabeledIon(true);
			(*label)->label.setIncludeLabel(true);
			(*label)->setIonType(string(1, peptide.getBY(i)));
		}
		
	}//end of for
	ionPercent = (double(labledCount) / double(len)) * 100;
	
	//remove unlabeled ions if necisary
	if(!pars.getIncludeAllIons())
		removeUnlabeledIons();
	
	//remove ions below specified intensity
	if(!pars.getMinIntensitySpecified())
		removeIntensityBelow(pars.getMinIntensity());
	
	if(calcLables)
		calcLabelPos();
}//end of function

void ms2::Spectrum::makePoints(labels::Labels& labs, double maxPerc,
							   double offset_x, double offset_y,
							   double x_padding, double y_padding)
{
	for(ionVecType::iterator it = ions.begin(); it != ions.end(); ++it)
	{
		if(it->getIntensity() >= maxPerc || it->getLabeledIon())
		{
			if(!it->getLabeledIon())
			{
				it->setFormatedLabel(utils::toString(it->getMZ()));
				it->setLabel(utils::toString(it->getMZ()));
				it->label.setIncludeLabel(true);
				it->label.labelLoc = geometry::Rect(it->getMZ() + offset_x,
													it->getIntensity() + offset_y, x_padding, y_padding);
			}
			else {
				it->label.labelLoc = geometry::Rect(it->getMZ() + offset_x,
													it->getIntensity() + offset_y, x_padding * 2, y_padding);
			}
			labs.push_back_labeledPoint(&(it->label));
			labs.push_back_dataPoint(geometry::Rect(it->getMZ(), it->getIntensity() / 2, POINT_PADDING, it->getIntensity()));
		}
	}
}
void ms2::Spectrum::calcLabelPos()
{
	double const xPadding = mzRange / 22.31972;
	double const yPadding = 6;
	
	calcLabelPos(DEFAULT_MAX_PERC,
				 DEFAULT_X_OFFSET, DEFAULT_Y_OFFSET,
				 xPadding, yPadding);
}

void ms2::Spectrum::calcLabelPos(double maxPerc,
								 double offset_x, double offset_y,
								 double x_padding, double y_padding)
{
	//initalize points
	labels::Labels labs(minMZ, maxMZ, minInt, maxInt);
	
	makePoints(labs, maxPerc, offset_x, offset_y, x_padding, y_padding);
	labs.spaceOutAlg2();
	
	//copy corrected points to spectrum
	//copyPointsToSpectrum(points);
}


