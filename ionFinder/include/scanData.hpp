//
//  scanData.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/23/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef scanData_hpp
#define scanData_hpp

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include <utils.hpp>

namespace scanData{
	
	class Scan;
	
	typedef std::vector<Scan> scansType;
	std::string const OF_EXT = ".spectrum";
	const char MOD_CHAR = '*';
	
	std::string removeStaticMod(std::string s, bool lowercase = true);
	std::string removeDynamicMod(std::string s, bool lowercase = true);
	
	class Scan{
	protected:
		std::string _precursorFile;
		size_t _scanNum;
		std::string _sequence;
		std::string _fullSequence;
		int _charge;
		std::string _xcorr;
		std::string _precursorMZ;
		std::string _precursorScan;
		bool _modified;
		int _spectralCounts;
		
		void initilizeFromLine(std::string);
		std::string makeSequenceFromFullSequence(std::string) const;
		std::string makeOfSequenceFromSequence(std::string) const;
	public:
		Scan(){
			_precursorFile = "";
			_scanNum = 0;
			_sequence = "";
			_charge = 0;
			_xcorr = "";
			_precursorMZ = "";
			_precursorScan = "";
			_modified = false;
			_spectralCounts = 0;
		}
		Scan(std::string sequence, size_t scanNum, std::string parentFile){
			_sequence = makeSequenceFromFullSequence(sequence);
			_fullSequence = sequence;
			_scanNum = scanNum;
			_precursorFile = parentFile;
			_modified = utils::strContains(MOD_CHAR, _sequence);
			_spectralCounts = 0;
		}
		
		Scan(std::string line){
			initilizeFromLine(line);
		}
		~Scan() {}

        virtual //modifers
		void clear();
	
		void operator = (const Scan& rhs){
			_precursorFile = rhs._precursorFile;
			_scanNum = rhs._scanNum;
			_sequence = rhs._sequence;
			_fullSequence = rhs._fullSequence;
			_charge = rhs._charge;
			_xcorr = rhs._xcorr;
			_precursorScan = rhs._precursorScan;
			_spectralCounts = rhs._spectralCounts;
		}
		
		void setSequence(std::string seq){
			_sequence = seq;
		}
		void setFullSequence(std::string s, bool resetSequence = false){
			_fullSequence = s;
			if(resetSequence)
				_sequence = makeOfSequenceFromSequence(s);
		}
		void setPrecursorFile(std::string str){
			_precursorFile = str;
		}
		void setCharge(int charge){
			_charge = charge;
		}
		void setPrecursorMZ(std::string mz){
			_precursorMZ = mz;
		}
		void setPrecursorScan(std::string s){
			_precursorScan = s;
		}
		void setScanNum(size_t s){
			_scanNum = s;
		}
		void setXcorr(std::string s){
			_xcorr = s;
		}
		void setSpectralCounts(int sc){
			_spectralCounts = sc;
		}
		
		//properties
		std::string getPrecursorFile() const{
			return _precursorFile;
		}
		size_t getScanNum() const{
			return _scanNum;
		}
		std::string getSequence() const{
			return _sequence;
		}
		std::string getFullSequence() const{
			return _fullSequence;
		}
		int getCharge() const{
			return _charge;
		}
		std::string getPrecursorMZ() const{
			return _precursorMZ;
		}
		std::string getPrecursorScan() const{
			return _precursorScan;
		}
		std::string getXcorr() const{
			return _xcorr;
		}
		int getSpectralCounts() const{
			return _spectralCounts;
		}
		//!Does the peptide contain a dynamic modification?
		bool isModified() const{
			return _modified;
		}
		std::string getOfNameBase(std::string, std::string) const;
		std::string getOfname() const;
	};
}

#endif /* scanData_hpp */
