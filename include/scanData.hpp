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
	
	std::string removeStaticMod(std::string s, bool lowercase = true);
	std::string removeDynamicMod(std::string s, bool lowercase = true);
	
	class Scan{
	protected:
		std::string precursorFile;
		size_t scanNum;
		std::string sequence;
		std::string fullSequence;
		int charge;
		std::string xcorr;
		std::string precursorMZ;
		std::string precursorScan;
		
		void initilizeFromLine(std::string);
		std::string makeSequenceFromFullSequence(std::string) const;
		std::string makeOfSequenceFromSequence(std::string) const;
	public:
		Scan(){
			precursorFile = "";
			scanNum = 0;
			sequence = "";
			charge = 0;
			xcorr = "";
			precursorMZ = "";
			precursorScan = "";
		}
		Scan(std::string _sequence, size_t _scanNum, std::string _parentFile){
			sequence = makeSequenceFromFullSequence(_sequence);
			fullSequence = _sequence;
			scanNum = _scanNum;
			precursorFile = _parentFile;
		}
		
		Scan(std::string line){
			initilizeFromLine(line);
		}
		~Scan() {}
		
		//modifers
		void clear();
	
		void operator = (const Scan& rhs){
			precursorFile = rhs.precursorFile;
			scanNum = rhs.scanNum;
			sequence = rhs.sequence;
			fullSequence = rhs.fullSequence;
			charge = rhs.charge;
			xcorr = rhs.xcorr;
			precursorScan = rhs.precursorScan;
		}
		
		void setSequence(std::string _seq){
			sequence = _seq;
		}
		void setFullSequence(std::string s, bool resetSequence = false){
			fullSequence = s;
			if(resetSequence)
				sequence = makeOfSequenceFromSequence(s);
		}
		void setPrecursorFile(std::string str){
			precursorFile = str;
		}
		void setCharge(int _charge){
			charge = _charge;
		}
		void setPrecursorMZ(std::string mz){
			precursorMZ = mz;
		}
		void setPrecursorScan(std::string s){
			precursorScan = s;
		}
		void setScanNum(size_t s){
			scanNum = s;
		}
		void setXcorr(std::string s){
			xcorr = s;
		}
		
		//properties
		std::string getPrecursorFile() const{
			return precursorFile;
		}
		size_t getScanNum() const{
			return scanNum;
		}
		std::string getSequence() const{
			return sequence;
		}
		std::string getFullSequence() const{
			return fullSequence;
		}
		int getCharge() const{
			return charge;
		}
		std::string getPrecursorMZ() const{
			return precursorMZ;
		}
		std::string getPrecursorScan() const{
			return precursorScan;
		}
		std::string getXcorr() const{
			return xcorr;
		}
		std::string getOfNameBase(std::string, std::string) const;
		std::string getOfname() const;
	};
}

#endif /* scanData_hpp */
