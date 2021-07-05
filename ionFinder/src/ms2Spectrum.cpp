//
// ms2Spectrum.cpp
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

#include <ms2Spectrum.hpp>

ms2::DataPoint& ms2::DataPoint::operator = (const ms2::DataPoint& rhs)
{
    labeledIon = rhs.labeledIon;
    label = rhs.label;
    formatedLabel = rhs.formatedLabel;
    topAbundant = rhs.topAbundant;
    ionType = rhs.ionType;
    ionNum = rhs.ionNum;
    _ion = rhs._ion;
    _noise = rhs._noise;
    _snr = rhs._snr;
    return *this;
}

ms2::DataPoint::DataPoint(const ms2::DataPoint& rhs)
{
    labeledIon = rhs.labeledIon;
    label = rhs.label;
    formatedLabel = rhs.formatedLabel;
    topAbundant = rhs.topAbundant;
    ionType = rhs.ionType;
    ionNum = rhs.ionNum;
    _ion = rhs._ion;
    _noise = rhs._noise;
    _snr = rhs._snr;
}

std::string ms2::DataPoint::getLableColor() const
{
    switch(ionType) {
        case PeptideNamespace::IonType::BLANK : return BLANK_COLOR;
            break;
        case PeptideNamespace::IonType::B : return B_COLOR;
            break;
        case PeptideNamespace::IonType::Y : return Y_COLOR;
            break;
        case PeptideNamespace::IonType::M : return M_COLOR;
            break;
        case PeptideNamespace::IonType::B_NL : return B_NL_COLOR;
            break;
        case PeptideNamespace::IonType::Y_NL : return Y_NL_COLOR;
            break;
        case PeptideNamespace::IonType::M_NL : return M_NL_COLOR;
            break;
        default:
            throw std::runtime_error("Not a valid option!");
    }
}

void ms2::Spectrum::writeMetaData(std::ostream& out) const
{
    assert(out);
    out << ms2::BEGIN_METADATA << NEW_LINE
        << ms2::PRECURSOR_FILE << OUT_DELIM << precursorScan.getFile() << NEW_LINE
        << ms2::OFNAME << OUT_DELIM << _scanData->getOfNameBase(getPrecursor().getSample(),
                                                                _scanData->getFullSequence()) << NEW_LINE
        << ms2::SCAN_NUMBER << OUT_DELIM << getScanNum() << NEW_LINE
        << ms2::SEQUENCE << OUT_DELIM << scanData::removeStaticMod(scanData::removeDynamicMod(_scanData->getSequence(), false), false) << NEW_LINE
        << ms2::FULL_SEQUENCE << OUT_DELIM << scanData::removeStaticMod(_scanData->getFullSequence()) << NEW_LINE
        << ms2::RET_TIME << OUT_DELIM << precursorScan.getIntensity() << NEW_LINE
        << ms2::PRECURSOR_CHARGE << OUT_DELIM << precursorScan.getCharge() << NEW_LINE
        << ms2::PLOT_HEIGHT << OUT_DELIM << plotHeight << NEW_LINE
        << ms2::PLOT_WIDTH << OUT_DELIM << plotWidth << NEW_LINE
        << ms2::PRECURSOR_INT << OUT_DELIM << std::scientific << precursorScan.getIntensity() << NEW_LINE;
    out.unsetf(std::ios::scientific);
    out << ms2::PRECURSOR_SCAN << OUT_DELIM << precursorScan.getScan() << NEW_LINE
        << ms2::END_METADATA <<NEW_LINE << ms2::BEGIN_SPECTRUM << NEW_LINE;
}

void ms2::Spectrum::printSpectrum(std::ostream& out, bool includeMetaData) const
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
    out << NEW_LINE;

    for(const auto & ion : _ions)
        out << ion.getMZ() << OUT_DELIM << ion.getIntensity() <<NEW_LINE;

    if(includeMetaData)
        out << ms2::END_SPECTRUM <<NEW_LINE;
}

void ms2::Spectrum::printLabeledSpectrum(std::ostream& out, bool includeMetaData) const
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
    out << NEW_LINE;

    std::streamsize ss = std::cout.precision();
    out.precision(5); //set out to print 5 floating point decimal places
    for(const auto & ion : _dataPoints)
    {
        // if(utils::strContains('*', ion.getFormatedLabel())) {
        //     std::cout << ion.getFormatedLabel() << NEW_LINE;
        // }

        out << std::fixed << ion.getMZ() << OUT_DELIM
            << ion.getIntensity() << OUT_DELIM
            << ion.getLabel() << OUT_DELIM
            << ion.getLableColor() << OUT_DELIM
            << ion.label.getIncludeLabel() << OUT_DELIM
            << PeptideNamespace::ionTypeToStr(ion.getIonType()) << OUT_DELIM
            << ion.getIonNum() << OUT_DELIM
            << ion.getFormatedLabel() << OUT_DELIM
            << ion.label.labelLoc.getX() << OUT_DELIM
            << ion.label.labelLoc.getY() << OUT_DELIM
            << ion.label.getIncludeArrow() << OUT_DELIM
            << ion.label.arrow.beg.getX() << OUT_DELIM
            << ion.label.arrow.beg.getY() << OUT_DELIM
            << ion.label.arrow.end.getX() << OUT_DELIM
            << ion.label.arrow.end.getY() << NEW_LINE;
    }
    out.precision(ss);

    if(includeMetaData)
        out << ms2::END_SPECTRUM << NEW_LINE;
}

void ms2::Spectrum::clear()
{
    _dataPoints.clear();
    utils::msInterface::Scan::clear();
}

/**
 * Set the DataPoint::topAbundant value for the top n ion intensities.<br><br>
 *
 * The function iterates through the Spectrum and finds the top n most
 * intense ions.
 * \param labelTop Top n ion intensities to label.
 */
void ms2::Spectrum::setLabelTop(size_t labelTop)
{
    typedef std::list<ms2::DataPoint*> listType;
    listType pointList;

    for(auto & ion : _dataPoints)
        pointList.push_back(&ion);

    if(pointList.size() > labelTop)
    {
        pointList.sort(ms2::DataPoint::IntComparison());
        pointList.resize(labelTop);
    }

    for(auto & it : pointList)
        it->setTopAbundant(true);
}

/**
 * Set mz range in spectra. Ions below \p minMZ and above \p maxInt will be removed.
 * \param minMZ
 * \param maxMZ
 * \param _sort Should ions be sorted before filtering? This option should be set to true
 * if ions are not already in order.
 */
void ms2::Spectrum::setMZRange(double minMZ, double maxMZ, bool _sort)
{
    //sort ions by mz
    if(_sort)
        sort(_dataPoints.begin(), _dataPoints.end(), DataPoint::MZComparison());

    auto begin = _dataPoints.begin();
    auto end = _dataPoints.end();

    //find min mz
    for(auto it = _dataPoints.begin(); it != _dataPoints.end(); ++it)
    {
        if(it->getMZ() <= minMZ)
        {
            begin = it;
            continue;
        }
        else break;
    }
    //find max mz
    for(auto it = begin; it != _dataPoints.end(); ++it)
    {
        if(it->getMZ() < maxMZ)
        {
            end = it;
            continue;
        }
        else break;
    }

    _dataPoints = ionVecType(begin, end);
    updateRanges();
}

void ms2::Spectrum::removeUnlabeledIons()
{
    for(auto it = _dataPoints.begin(); it != _dataPoints.end();)
    {
        if(!it->getForceLabel())
            _dataPoints.erase(it);
        else ++it;
    }
    updateRanges();
}

/**
 * Remove ions below \p min_int.
 * \param min_int Minimum intensity to remove.
 */
void ms2::Spectrum::removeIntensityBelow(double min_int)
{
    for(auto it = _dataPoints.begin(); it != _dataPoints.end();)
    {
        if(it->getIntensity() < min_int)
            _dataPoints.erase(it);
        else ++it;
    }

    updateRanges();
}

//! Calculate signal to nose ratio of ion intensities
void ms2::Spectrum::calcSNR(double snrConf, std::ostream& out)
{
    // if(getPrecursor().getFile() == "20190912_Thompson_PAD62_GlucTryp_t2.mzML" && _scanNum == 17491)
    //     std::cout << "Found!" << NEW_LINE;

    double sd = statistics::sd<DataPoint>(_dataPoints, [](const DataPoint& i) -> double{return i.getIntensity();});
    double mean = statistics::mean<DataPoint>(_dataPoints, [](const DataPoint& i) -> double{return i.getIntensity();});
    std::vector<double> stats;
    for(auto point : _dataPoints)
        stats.push_back(abs(point.getIntensity() - mean) / sd);

    auto dist = std::shared_ptr<statistics::ProbabilityDist>();
    size_t len = _dataPoints.size();
    if(len > 30)
        dist = std::make_shared<statistics::NormDist>();
    else dist = std::make_shared<statistics::TDist>(double(len - 1));

    double noise = 0;
    size_t noiseLen = 0;
    for(size_t i = 0; i < len; i++){
        double pVal = dist->pValue(stats[i]);
        if(pVal > snrConf)
            _dataPoints[i].setNoise(false);
        else {
            noise += _dataPoints[i].getIntensity();
            noiseLen++;
        }
    }
    noise /= double(noiseLen);

    for(auto & _dataPoint : _dataPoints)
        _dataPoint.setSNR(_dataPoint.getIntensity() / noise);

    for(size_t i = 0; i < len; i++){
        out << getPrecursor().getFile() << ',' << _scanNum << ',' <<
             i << ',' << _dataPoints[i].getIntensity() << ',' <<
             stats[i] << ',' << (_dataPoints[i].getNoise() ? "TRUE": "FALSE") << "," << _dataPoints[i].getSNR() << NEW_LINE;
    }
}

//! Remove ions with a signal to nose ratio below \p snrThreshold
void ms2::Spectrum::removeSNRBelow(double snrThreshold, double snrConf)
{
    std::string fname = "/Users/Aaron/Documents/School_Work/Mass_spec_data/Thompson_lab_samples/Ari/ionFinder_cuttoff_analysis_2/tables/test.csv";
    std::ofstream outF;

    // printIons(std::cout);

    outF.open(fname, std::ios_base::app);
    // outF.open(fname);
    // std::string headers [] = {"file", "scan", "ionNum", "int", "stat", "pVal", "noise", "snr"} ;
    // for(auto h : headers){
    //     if(h == "file") outF << h;
    //     else outF << "," << h;
    // }
    // outF << NEW_LINE;
    calcSNR(snrConf, outF);
}

//! Copy ions from utils::Scan::_ions to labeledIons
void ms2::Spectrum::initLabeledIons()
{
    _dataPoints.clear();
    size_t len = size();
    for(size_t i = 0; i < len; i++) {
        _dataPoints.emplace_back(&_ions[i]);
    }
}

/**
 * Label spectrum with predicted fragment ions from \p peptide.
 * \param peptide Peptide to label spectrum with.
 * \param pars Initialized params object.
 * \param removeUnlabeledFrags Should unlabeled F
 * \param labelTop
 */
void ms2::Spectrum::labelSpectrum(PeptideNamespace::Peptide& peptide,
                                  const base::ParamsBase& pars,
                                  bool removeUnlabeledFrags, size_t labelTop)
{
    initLabeledIons();
    plotWidth = pars.getPlotWidth();
    plotHeight = pars.getPlotHeight();
    size_t len = peptide.getNumFragments();
    size_t labledCount = 0;
    double _labelTolerance;
    bool seqPrinted = false;

    typedef std::list<ms2::DataPoint*> listType;
    listType rangeList;
    listType::iterator label;

    setLabelTop(labelTop); //determine which labeledIons are abundant enough to considered in labeling
    std::sort(_dataPoints.begin(), _dataPoints.end(), DataPoint::MZComparison()); //sort labeledIons by mz
    if(pars.getMZSpecified()) //set user specified mz range if specified
    {
        setMZRange(pars.getMinMZSpecified() ? pars.getMinMZ() : getMaxMZ(),
                   pars.getMaxMZSpecified() ? pars.getMaxMZ() : getMaxMZ(),
                   false);
    }

    //iterate through all calculated fragment ions and label ions on spectrum if they are found
    for(size_t i = 0; i < len; i++)
    {
        double tempMZ = peptide.getFragmentMZ(i);

        rangeList.clear();

        //first get lowest value in range
        _labelTolerance = pars.getMatchTolerance(tempMZ);
        auto lowerBound = std::lower_bound(_dataPoints.begin(), _dataPoints.end(),
                                           (tempMZ - (_labelTolerance)),
                                           DataPoint::MZComparison());

        //ittreate throughout all labeledIons above in range
        for(auto it = lowerBound; it != _dataPoints.end(); ++it)
        {
            if(it->getMZ() > (tempMZ + _labelTolerance))
                break;

            if(it->getTopAbundant()){
                //check that it->mz is in range
                bool inRange = utils::inRange(it->getMZ(), tempMZ, _labelTolerance);
                if(inRange)
                    rangeList.push_back(&(*it));
            }//end of if
        }
        //else continue;

        if(rangeList.empty())
            continue;

        label = rangeList.begin();
        if(rangeList.size() > 1)
        {
            //iterate through all ions within matchTolerance range to get the one with max intensity
            double tempMax = (*label)->getIntensity();
            double tempMZdiff = abs((*label)->getMZ() - tempMZ);
            for(auto it = rangeList.begin(); it != rangeList.end(); ++it)
            {
                if(pars.getMultipleMatchCompare() == "intensity" || pars.getMultipleMatchCompare() == "int"){
                    if((*it)->getIntensity() > tempMax)
                    {
                        label = it;
                        tempMax = (*label)->getIntensity();
                    }
                }
                else if(pars.getMultipleMatchCompare() == "mz"){
                    if(((*it)->getMZ() - tempMZ) < tempMZdiff)
                    {
                        label = it;
                        tempMZdiff = abs((*label)->getMZ() - tempMZ);
                    }
                }
                else{
                    throw std::runtime_error("Unknown multipleMatchCompare method!");
                }
            }
        }

        if((*label)->getLabeledIon() && pars.getVerbose()){
            if(!seqPrinted){
                std::cout << "In sequence: " << peptide.getFullSequence() << NEW_LINE;
                seqPrinted = true;
            }
            std::cout << "\tDuplicate label found: " << (*label)->getLabel() << ", " <<
                      peptide.getFragmentLabel(i) << NEW_LINE;
        }

        //if label is not already labeled or if peptide.getFragment(i) is not a NL
        if(!(*label)->getLabeledIon() || peptide.getFragment(i).isNL())
        {
            if(peptide.getIncludeLabel(i)) //only label spectrum if fragment should be labeled.
            {
                (*label)->setLabel(peptide.getFragmentLabel(i));
                (*label)->setFormatedLabel(peptide.getFormatedLabel(i));
                (*label)->setLabeledIon(true);
                (*label)->label.setIncludeLabel(true);
                (*label)->setIonType(peptide.getFragment(i).getIonType());
                (*label)->setIonNum(peptide.getFragment(i).getNum());
                labledCount++;
            }
        }
        peptide.setFound(i, true);
        peptide.setFoundMZ(i, (*label)->getMZ());
        peptide.setFoundIntensity(i, (*label)->getIntensity());
    }//end of for
    ionPercent = (double(labledCount) / double(len)) * 100;

    //remove unlabeled ions if necessary
    if(!pars.getIncludeAllIons())
        removeUnlabeledIons();

    //remove unlabeled peptide fragments
    //only used for debugging
    if(removeUnlabeledFrags)
        peptide.removeUnlabeledFrags();

}//end of function

void ms2::Spectrum::makePoints(labels::Labels& labs, double maxPerc,
                               double offset_x, double offset_y,
                               double x_padding, double y_padding)
{
    for(auto & ion : _dataPoints)
    {
        if(ion.getIntensity() >= maxPerc || ion.getLabeledIon())
        {
            if(!ion.getLabeledIon())
            {
                ion.setFormatedLabel(std::to_string(ion.getMZ()));
                ion.setLabel(std::to_string(ion.getMZ()));
                ion.label.setIncludeLabel(true);
                ion.label.labelLoc = geometry::Rect(ion.getMZ() + offset_x,
                                                    ion.getIntensity() + offset_y, x_padding, y_padding);
            }
            else {
                ion.label.labelLoc = geometry::Rect(ion.getMZ() + offset_x,
                                                    ion.getIntensity() + offset_y, x_padding, y_padding);
            }
            labs.push_back_labeledPoint(&(ion.label));
            labs.push_back_dataPoint(geometry::Rect(ion.getMZ(), ion.getIntensity() / 2, POINT_PADDING,
                                                    ion.getIntensity()));
        }
    }
}

void ms2::Spectrum::calcLabelPos()
{
    double const xPadding = _mzRange / 22.31972;
    double const yPadding = 6;

    calcLabelPos(DEFAULT_MAX_PERC,
                 DEFAULT_X_OFFSET, DEFAULT_Y_OFFSET,
                 xPadding, yPadding);
}

void ms2::Spectrum::calcLabelPos(double maxPerc,
                                 double offset_x, double offset_y,
                                 double x_padding, double y_padding)
{
    //initialize points
    labels::Labels labs(getMinMZ(), getMaxMZ(), _minInt, _maxInt);

    makePoints(labs, maxPerc, offset_x, offset_y, x_padding, y_padding);
    labs.spaceOutAlg2();

    //copy corrected points to spectrum
    //copyPointsToSpectrum(points);
}
