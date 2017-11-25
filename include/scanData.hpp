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

namespace scanData{
	
	class Scan;
	
	typedef vector<Scan> scansType;
	string const OF_EXT = ".spectrum";
	
	class Scan{
	private:
		string parentFile;
		size_t scanNum;
		string sequence;
		string fullSequence;
		int charge;
		string xcorr;
		
		string makeSequenceFromFullSequence(string) const;	
	public:
		Scan(){
			parentFile = "";
			scanNum = 0;
			sequence = "";
			charge = 0;
		}
		Scan(string _sequence, size_t _scanNum, string _parentFile){
			sequence = _sequence;
			fullSequence = _sequence;
			scanNum = _scanNum;
			parentFile = _parentFile;
		}
		
		Scan (string);
		~Scan() {}
		
		void clear();
		
		void setSequence(string _seq){
			sequence = _seq;
		}
		void setParentFile(string str){
			parentFile = str;
		}
		
		string getParentFile() const{
			return parentFile;
		}
		size_t getScanNum() const{
			return scanNum;
		}
		string getSequence() const{
			return sequence;
		}
		string getFullSequence() const{
			return fullSequence;
		}
		int getCharge() const{
			return charge;
		}
		string getXcorr() const{
			return xcorr;
		}
		string getOfname() const;
	};
}

#endif /* scanData_hpp */
