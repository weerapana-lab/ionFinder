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
	
	class Scan{
	protected:
		std::string precursorFile;
		size_t scanNum;
		std::string sequence;
		std::string fullSequence;
		int charge;
		std::string xcorr;
		
		void initilizeFromLine(std::string);
		std::string makeSequenceFromFullSequence(std::string) const;
		std::string makeOfSequenceFromSequence(std::string) const;
	public:
		Scan(){
			precursorFile = "";
			scanNum = 0;
			sequence = "";
			charge = 0;
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
		}
		
		void setSequence(std::string _seq){
			sequence = _seq;
		}
		void setPrecursorFile(std::string str){
			precursorFile = str;
		}
		void setCharge(double _charge){
			charge = _charge;
		}
		
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
		std::string getXcorr() const{
			return xcorr;
		}
		std::string getOfNameBase() const;
		std::string getOfname() const;
	};
}

#endif /* scanData_hpp */
