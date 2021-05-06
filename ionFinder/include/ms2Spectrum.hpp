//
// ms2Spectrum.hpp
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

#ifndef ms2Spectrum_hpp
#define ms2Spectrum_hpp

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cassert>
#include <list>
#include <algorithm>
#include <string>
#include <iomanip>
#include <type_traits>

#include <utils.hpp>
#include <msInterface/msScan.hpp>
#include <peptide.hpp>
#include <geometry.hpp>
#include <calcLableLocs.hpp>
#include <scanData.hpp>
#include <spectrum_constants.hpp>

namespace ms2{
	
	std::string const SPECTRUM_COL_HEADERS [] = {MZ, INTENSITY, LABEL, COLOR, INCLUDE_LABEL,
		ION_TYPE, ION_NUM, FORMATED_LABEL, LABEL_X, LABEL_Y, INCLUDE_ARROW, ARROW_BEG_X,
		ARROW_BEG_Y, ARROW_END_X, ARROW_END_Y};
	size_t const NUM_SPECTRUM_COL_HEADERS_SHORT = 2;
	size_t const NUM_SPECTRUM_COL_HEADERS_LONG = 15;
	std::string const NA_STR = "NA";
	
	double const POINT_PADDING = 1;
	double const DEFAULT_MAX_PERC = 1;
	double const DEFAULT_X_OFFSET = 0;
	double const DEFAULT_Y_OFFSET = 3;
	size_t const LABEL_TOP = 200;
	
	class Spectrum;
	class DataPoint;

    class DataPoint {
		friend class Spectrum;
	private:
        utils::msInterface::ScanIon* _ion;
		bool labeledIon;
		geometry::DataLabel label;
		std::string formatedLabel;
		//!Is the ion one of the top n most intense ions in the Spectrum?
		bool topAbundant;
		PeptideNamespace::IonType ionType;
		int ionNum;

		//! Initialize DataPoint stats with default values.
		void initStats(){
			label = geometry::DataLabel();
			labeledIon = false;
			topAbundant = false;
			formatedLabel = NA_STR;
			ionType = PeptideNamespace::IonType::BLANK;
			ionNum = 0;
		}
	public:
		DataPoint(){
			_ion = nullptr;
			initStats();
		}
		DataPoint(utils::msInterface::ScanIon* ion){
			_ion = ion;
			initStats();
		}
		DataPoint(const DataPoint&);
		~DataPoint () = default;
        DataPoint& operator = (const DataPoint&);

		void setLabeledIon(bool _lab){
			labeledIon = _lab;
			label.forceLabel = labeledIon;
		}
		
		void setLabel(std::string str){
			label.setLabel(str);
		}
		void setFormatedLabel(std::string str){
			formatedLabel = str;
		}
		void setTopAbundant(bool boo){
			topAbundant = boo;
		}
		void setForceLabel(bool boo){
			label.forceLabel = boo;
		}
		void setIonType(PeptideNamespace::IonType id){
			ionType = id;
		}
		void setIonNum(int num){
			ionNum = num;
		}
		void setMZ(utils::msInterface::ScanMZ mz) {
            _ion->setMZ(mz);
        }
        void setIntensity(utils::msInterface::ScanIntensity intensity) {
            return _ion->setIntensity(intensity);
        }
		
		std::string getLabel() const{
			return label.getLabel();
		}
		bool getLabeledIon() const{
			return labeledIon;
		}
		bool getForceLabel() const{
			return label.forceLabel;
		}
		bool getTopAbundant() const{
			return topAbundant;
		}
		std::string getFormatedLabel() const{
			return formatedLabel;
		}
		PeptideNamespace::IonType getIonType() const{
			return ionType;
		}
		int getIonNum() const{
			return ionNum;
		}
		utils::msInterface::ScanIntensity getIntensity() const {
			return _ion->getIntensity();
        }
        utils::msInterface::ScanMZ getMZ() const {
            return _ion->getMZ();
        }
		std::string getLableColor() const;

        //for utils::insertSorted()
        inline bool insertCompare(const DataPoint& comp) const{
            return getIntensity() > comp.getIntensity();
        }

        struct MZComparison {
            bool operator()(const DataPoint& lhs, const DataPoint& rhs) const{
                return lhs.getMZ() < rhs.getMZ();
            }

            bool operator()(const DataPoint& lhs, utils::msInterface::ScanMZ rhs) const{
                return lhs.getMZ() < rhs;
            }
        };

        struct IntComparison {
            bool operator()(DataPoint *lhs, DataPoint *rhs) const{
                return lhs->insertCompare(*rhs);
            }
        };
	};
	
    class Spectrum : public utils::msInterface::Scan{
	private:
		typedef std::vector<ms2::DataPoint> ionVecType;
		typedef ionVecType::const_iterator ionsTypeConstIt;
		typedef ionVecType::iterator ionsTypeIt;
		
		//static metadata
		double plotHeight;
		double plotWidth;
		
		//match stats
		double ionPercent;
		double spScore;

		//! Stores data about scan from input, not what was retrieved from the ms2 file.
		scanData::Scan* _scanData;

		ionVecType _dataPoints;
		
		void makePoints(labels::Labels&, double, double, double, double, double);
		void setLabelTop(size_t);
		void removeUnlabeledIons();
		void initLabeledIons();

	public:
		Spectrum() : utils::msInterface::Scan()
		{
            ionPercent = 0;
            spScore = 0;
			plotWidth = 0;
			plotHeight = 0;
			_dataPoints = ionVecType();
			_scanData = nullptr;
		}
		~Spectrum() = default;
		
		//modifiers
		void clear();
        void removeIntensityBelow(double minInt);
        void setMZRange(double minMZ, double maxMZ, bool _sort = true);

		/**
		 * Normalize ion intensities so that the max intensity is \p max.
		 * \param max Max intensity.
		 */
		template<typename _Tp> void normalizeIonInts(_Tp max)
		{
			static_assert(std::is_arithmetic<_Tp>::value, "Max must be arithmetic!");
			utils::msInterface::ScanIntensity den = getMaxInt() / max;
			for(auto & ion : _ions)
				ion.setIntensity(ion.getIntensity() / den);

			updateRanges();
		}
		void labelSpectrum(PeptideNamespace::Peptide& peptide,
						   const base::ParamsBase& pars,
						   bool removeUnlabeledFrags = false,
						   size_t labelTop = LABEL_TOP);
		void calcLabelPos(double maxPerc,
						  double offset_x, double offset_y,
						  double padding_x, double padding_y);
		void calcLabelPos();
		void setScanData(scanData::Scan* scan) {
            _scanData = scan;
        }
		
		void writeMetaData(std::ostream&) const;
		void printSpectrum(std::ostream&, bool includeMetadata = false) const;
		void printLabeledSpectrum(std::ostream&, bool) const;
        const utils::msInterface::PrecursorScan& getPrecursor() const{
            return utils::msInterface::Scan::getPrecursor();
        }
	};
}

#endif /* ms2Spectrum_hpp */
