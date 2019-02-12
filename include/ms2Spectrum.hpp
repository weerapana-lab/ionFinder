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
#include <string>
#include <iomanip>

#include <utils.hpp>
#include <peptide.hpp>
#include <geometry.hpp>
#include <calcLableLocs.hpp>
#include <scanData.hpp>

namespace ms2{
	
	std::string const SPECTRUM_COL_HEADERS [] = {"mz", "intensity", "label", "includeLabel",
		"ionType", "ionNum", "formatedLabel", "labelX", "labelY", "includeArrow", "arrowBegX",
		"arrowBegY", "arrowEndX", "arrowEndX"};
	size_t const NUM_SPECTRUM_COL_HEADERS_SHORT = 2;
	size_t const NUM_SPECTRUM_COL_HEADERS_LONG = 14;
	std::string const NA_STR = "NA";
	
	std::string const BEGIN_METADATA = "<metadata>";
	std::string const END_METADATA = "</metadata>";
	std::string const BEGIN_SPECTRUM = "<spectrum>";
	std::string const END_SPECTRUM = "</spectrum>";
	
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
		//typedef int mz_intType;
		Ion(){
			intensity = 0;
			mz = 0;
			//mz_int = 0;
		}
		Ion(double _mz, double _int){
			mz = _mz;
			intensity = _int;
			//mz_int = utils::round(mz); //mz_int is rounded here
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
		/*mz_intType getMZInt() const{
			return mz_int;
		}*/
		
		void write(std::ofstream&) const;
	protected:
		
		double intensity;
		double mz;
		//mz_intType mz_int;
	};
	
	class DataPoint : public Ion{
		friend class Spectrum;
	private:
		bool labeledIon;
		geometry::DataLabel label;
		std::string formatedLabel;
		bool topAbundant;
		std::string ionType;
		int ionNum;
		
		/**
		 Initialize DataPoint stats with default values.
		 */
		void initStats(){
			label = geometry::DataLabel();
			labeledIon = false;
			topAbundant = false;
			formatedLabel = NA_STR;
			ionType = "blank";
			ionNum = 0;
		}
	public:
		DataPoint() : Ion(){
			initStats();
		}
		DataPoint(double _mz, double _int) : Ion(_mz, _int){
			initStats();
		}
		~DataPoint () {}
		
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
		void setIonType(std::string str){
			ionType = str;
		}
		void setIonNum(int num){
			ionNum = num;
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
		std::string getIonType() const{
			return ionType;
		}
		int getIonNum() const{
			return ionNum;
		}
		
		//for utils::insertSorted()
		inline bool insertCompare(const DataPoint& comp) const{
			return intensity > comp.intensity;
		}
		
		struct MZComparison {
			bool const operator()(const DataPoint& lhs, const DataPoint& rhs) const{
				return lhs.getMZ() < rhs.getMZ();
			}
			
			bool const operator()(const DataPoint& lhs, double rhs) const{
				return lhs.getMZ() < rhs;
			}
		};
		
		struct IntComparison {
			bool const operator()(DataPoint *lhs, DataPoint *rhs) const{
				return lhs->insertCompare(*rhs);
			}
		};
	};
	
	class Spectrum : public scanData::Scan{
		friend class Ms2File;
	private:
		typedef std::vector<ms2::DataPoint> ionVecType;
		typedef ionVecType::const_iterator ionsTypeConstIt;
		typedef ionVecType::iterator ionsTypeIt;
		
		//static metadata
		double retTime;
		double precursorInt;
		//!pretty parent file name with out extension for naming
		std::string _parentMs2;
		
		//dynamic metadata
		double maxInt;
		double minInt;
		double minMZ;
		double maxMZ;
		double mzRange;
		
		//match stats
		double ionPercent;
		double spScore;
		
		ionVecType ions;
		
		void makePoints(labels::Labels&, double, double, double, double, double);
		void setLabelTop(size_t);
		void setMZRange(double minMZ, double maxMZ, bool _sort = true);
		void removeUnlabeledIons();
		void removeIntensityBelow(double minInt);
		double calcMaxInt() const;
		double calcMinInt() const;
		void updateDynamicMetadata();
		
	public:
		Spectrum() : scanData::Scan()
		{
			retTime = 0;
			precursorInt = 0;
			maxInt = 0;
			minInt = 0;
			ionPercent = 0;
			spScore = 0;
			minMZ = 0;
			maxMZ = 0;
			mzRange = 0;
		}
		~Spectrum() {}
		
		//modifers
		void clear();
		template<typename _Tp> void normalizeIonInts(_Tp _den){
			double den = getMaxIntensity() / _den;
			for(ionVecType::iterator it = ions.begin(); it != ions.end(); ++it)
				it->normalizeIntensity(den);
			
			updateDynamicMetadata();
		}
		double getMaxIntensity() const{
			return maxInt;
		}
		void labelSpectrum(PeptideNamespace::Peptide& peptide,
						   const base::ParamsBase& pars,
						   bool removeUnlabeledFrags = false,
						   size_t labelTop = LABEL_TOP);
		void calcLabelPos(double maxPerc,
						  double offset_x, double offset_y,
						  double padding_x, double padding_y);
		void calcLabelPos();
		
		void writeMetaData(std::ostream&) const;
		void printSpectrum(std::ostream&, bool) const;
		void printLabeledSpectrum(std::ostream&, bool) const;
	};
}

#endif /* ms2Spectrum_hpp */
