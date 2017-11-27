//
//  ms2Spectrum.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
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
#include "../include/utils.hpp"
#include "../include/peptide.hpp"
#include "../include/geometry.hpp"
#include "../include/calcLableLocs.hpp"

namespace ms2{
	
	string const SPECTRUM_COL_HEADERS [] = {"mz", "intensity", "label", "includeLabel",
		"ionType", "formatedLabel", "labelX", "labelY"};
	size_t const NUM_SPECTRUM_COL_HEADERS_SHORT = 2;
	size_t const NUM_SPECTRUM_COL_HEADERS_LONG = 8;
	string const NA_STR = "NA";
	
	string const BEGIN_METADATA = "<metadata>";
	string const END_METADATA = "</metadata>";
	string const BEGIN_SPECTRUM = "<spectrum>";
	string const END_SPECTRUM = "</spectrum>";
	
	double const LABEL_MZ_TOLERANCE = 0.25;
	double const POINT_PADDING = 1;
	double const DEFAULT_MAX_PERC = 1;
	double const DEFAULT_X_OFFSET = 0;
	double const DEFAULT_Y_OFFSET = 3;
	size_t const LABEL_TOP = 200;
	
	class Spectrum;
	class Ion;
	class DataPoint;
	struct DataPointIntComparison;
	struct DataPointMZComparison;
	
	class Ion{
	public:
		typedef int mz_intType;
		Ion(){
			intensity = 0;
			mz = 0;
			mz_int = 0;
		}
		Ion(double _mz, double _int){
			mz = _mz;
			intensity = _int;
			mz_int = utils::round(mz); //mz_int is rounded here
		}
		~Ion() {};
		
		template<typename _Tp>
		void normalizeIntensity(_Tp den){
			intensity = intensity / den;
		}
		
		double getIntensity() const{
			return intensity;
		}
		double getMZ() const{
			return mz;
		}
		mz_intType getMZInt() const{
			return mz_int;
		}
		
		void write(ofstream&) const;
	protected:
		
		double intensity;
		double mz;
		mz_intType mz_int;
	};
	
	class DataPoint : public Ion{
		friend class Spectrum;
	private:
		bool labeledIon;
		geometry::DataLabel label;
		string formatedLabel;
		bool topAbundant;
		string ionType;
		
	public:
		DataPoint() : Ion(){
			label = geometry::DataLabel();
			labeledIon = false;
			topAbundant = false;
			formatedLabel = NA_STR;
			ionType = "blank";
		}
		DataPoint(double _mz, double _int) : Ion(_mz, _int){
			label = geometry::DataLabel();
			labeledIon = false;
			topAbundant = false;
			formatedLabel = NA_STR;
			ionType = "blank";
		}
		~DataPoint () {}
		
		void setLabeledIon(bool _lab){
			labeledIon = _lab;
			label.forceLabel = labeledIon;
		}
		
		void setLabel(string str){
			label.setLabel(str);
		}
		void setFormatedLabel(string str){
			formatedLabel = str;
		}
		void setTopAbundant(bool boo){
			topAbundant = boo;
		}
		void setForceLabel(bool boo){
			label.forceLabel = boo;
		}
		void setIonType(string str){
			ionType = str;
		}
		
		string getLabel() const{
			return label.getLabel();
		}
		bool getLabeledIon() const{
			return labeledIon;
		}
		bool getForceLabel() const{
			return label.forceLabel;
		}
		bool getIncludeLabel() const{
			return label.getIncludeLabel();
		}
		bool getTopAbundant() const{
			return topAbundant;
		}
		string getFormatedLabel() const{
			return formatedLabel;
		}
		string getIonType() const{
			return ionType;
		}
		
		//for utils::insertSorted()
		inline bool insertComp(const DataPoint& comp) const{
			return intensity > comp.intensity;
		}
	};
	
	struct DataPointIntComparison {
		bool const operator()(DataPoint *lhs, DataPoint *rhs) const{
			return lhs->insertComp(*rhs);
		}
	};
	
	struct DataPointMZComparison {
		bool const operator()(const DataPoint& lhs, const DataPoint& rhs) const{
			return lhs.getMZ() < rhs.getMZ();
		}
	};
	
	class Spectrum{
		friend class Ms2File;
	private:
		typedef std::vector<ms2::DataPoint> ionVecType;
		typedef ionVecType::const_iterator ionsTypeConstIt;
		typedef ionVecType::iterator ionsTypeIt;
		
		//metadata
		int scanNumber;
		int scanNumInt;
		double retTime;
		double precursorInt;
		string precursorFile;
		int precursorScan;
		int precursorCharge;
		double precursorMZ;
		double maxInt;
		double minInt;
		double minMZ;
		double maxMZ;
		double mzRange;
		string sequence;
		
		//match stats
		double ionPercent;
		double spScore;
		
		ionVecType ions;
		
		void makePoints(labels::Labels&, double, double, double, double, double);
		void setLabelTop(size_t);
		void setMZRange(double minMZ, double maxMZ, bool _sort = true);
		void removeUnlabeledIons();
		void removeIntensityBelow(double minInt);
		ionsTypeIt upperBound(double);
		ionsTypeIt lowerBound(double);
		
	public:
		Spectrum()
		{
			scanNumber = 0;
			scanNumInt = 0;
			retTime = 0;
			precursorInt = 0;
			precursorFile = "";
			precursorScan = 0;
			precursorCharge = 0;
			precursorMZ = 0;
			maxInt = 0;
			minInt = 0;
			ionPercent = 0;
			spScore = 0;
			minMZ = 0;
			maxMZ = 0;
			mzRange = 0;
			sequence = "";
		}
		~Spectrum() {}
		
		//modifers
		void clear();
		template<typename _Tp> void normalizeIonInts(_Tp den);
		double getMaxIntensity() const{
			return maxInt;
		}
		double getPrecursorCharge() const{
			return precursorCharge;
		}
		void labelSpectrum(const peptide::Peptide& peptide,
						   const params::Params& pars,
						   bool calclabels = true, size_t labelTop = LABEL_TOP);
		void calcLabelPos(double maxPerc,
						  double offset_x, double offset_y,
						  double padding_x, double padding_y);
		void calcLabelPos();
		
		void writeMetaData(ostream&) const;
		void printSpectrum(ostream&, bool) const;
		void printLabeledSpectrum(ostream&, bool) const;
	};
}

#endif /* ms2Spectrum_hpp */
