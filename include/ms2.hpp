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
#include "../include/utils.hpp"
#include "../include/peptide.hpp"
#include "../include/ms2Spectrum.hpp"

namespace ms2 {
	
	size_t const NUM_DIGITS_IN_SCAN = 6;
	
	class Ms2File;
	
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
