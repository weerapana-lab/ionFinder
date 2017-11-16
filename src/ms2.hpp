//
//  ms2.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/6/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef ms2_hpp
#define ms2_hpp

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cassert>
#include <list>
#include <algorithm>
#include "utils.hpp"
#include "peptide.hpp"
#include "geometry.hpp"
#include "calcLableLocs.hpp"

namespace ms2{
	
	size_t const NUM_DIGITS_IN_SCAN = 6;
	
	string const SPECTRUM_COL_HEADERS [] = {"mz", "intensity", "label",
		"formated_label", "label_x", "label_y"};
	size_t const NUM_SPECTRUM_COL_HEADERS_SHORT = 2;
	size_t const NUM_SPECTRUM_COL_HEADERS_LONG = 6;
	
	double const DEFAULT_MAX_PERC = 5;
	double const DEFAULT_X_PADDING = 5;
	double const DEFAULT_Y_PADDING = 50;
	double const DEFAULT_X_OFFSET = 0;
	double const DEFAULT_Y_OFFSET = 5;
	size_t const LABEL_TOP = 200;
	
	class Ms2File;
	class Spectrum;
	class Ion;
	class DataPoint;
	struct DataPointComparison;
	
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
		bool includeLabel;
		geometry::DataLabel label;
		string formatedLabel;
		bool topAbundant;
		
	public:
		DataPoint() : Ion(){
			label = geometry::DataLabel();
			includeLabel = false;
			topAbundant = false;
			formatedLabel = "";
		}
		DataPoint(double _mz, double _int) : Ion(_mz, _int){
			label = geometry::DataLabel();
			includeLabel = false;
			topAbundant = false;
			formatedLabel = "";
		}
		~DataPoint () {}
		
		void setIncludeLabel(bool _lab){
			includeLabel = _lab;
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
		
		string getLabel() const{
			return label.getLabel();
		}
		bool getIncludeLabel() const{
			return includeLabel;
		}
		bool getTopAbundant() const{
			return topAbundant;
		}
		string getFormatedLabel() const{
			return formatedLabel;
		}
		
		//for utils::insertSorted()
		inline bool insertComp(const DataPoint& comp) const{
			return intensity > comp.intensity;
		}
	};
	
	struct DataPointComparison {
		bool const operator()(DataPoint *lhs, DataPoint *rhs) const{
			return lhs->insertComp(*rhs);
		}
	};
	
	class Spectrum{
		friend class Ms2File;
	private:
		typedef std::vector<ms2::DataPoint> ionVecType;
		typedef std::map<ms2::Ion::mz_intType, ionVecType> ionsType;
		typedef ionsType::const_iterator ionsTypeIt;
		
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
		
		//match stats
		double ionPercent;
		double spScore;
		
		ionsType ions;
		
		void makePoints(labels::Labels&, double, double, double, double, double);
		void setLabelTop(size_t);
		
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
		}
		~Spectrum() {}
		
		//modifers
		void clear();
		template<typename _Tp> void normalizeIonInts(_Tp den);
		double getMaxIntensity() const{
			return maxInt;
		}
		void labelSpectrum(const peptide::Peptide& peptide,
						   bool calclabels = true, size_t labelTop = LABEL_TOP);
		void calcLabelPos(double maxPerc,
						  double offset_x, double offset_y,
						  double padding_x, double padding_y);
		void calcLabelPos();
		
		void printSpectrum(ostream&, bool) const;
		void printLabeledSpectrum(ostream&, bool) const;
	};
	
	class Ms2File{
	private:
		//file buffer vars
		char* buffer;
		unsigned long size;
		string delim;
		utils::newline_type delimType;
		int beginLine;
		
		//metadata
		string fname;
		string firstScan, lastScan;
		string dataType;
		string scanType;
		static int mdNum;
		
		bool getMetaData();
		const char* makeOffsetQuery(string) const;
		const char* makeOffsetQuery(size_t) const;
		
	public:
		Ms2File() {
			size = 0;
			fname = "";
		}
		Ms2File(string _fname){
			fname = _fname;
			size = 0;
		}
		~Ms2File(){
			delete [] buffer;
		}
		
		//modifers
		bool read(string);
		bool read();
		
		//properties
		bool getScan(string, Spectrum&) const;
		bool getScan(size_t, Spectrum&) const;
	};
	
	int Ms2File::mdNum = 4;
	
}//end of namespace

#endif /* ms2_hpp */
