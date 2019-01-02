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
#include <string>

#include <utils.hpp>
#include <peptide.hpp>
#include <ms2Spectrum.hpp>

namespace ms2 {
	
	size_t const NUM_DIGITS_IN_SCAN = 6;
	int const MD_NUM = 4;
	
	class Ms2File;
	
	class Ms2File{
	private:
		//file buffer vars
		char* buffer;
		unsigned long size;
		
		//metadata
		std::string fname;
		size_t firstScan, lastScan;
		std::string dataType;
		std::string scanType;
		
		bool getMetaData();
		const char* makeOffsetQuery(std::string) const;
		const char* makeOffsetQuery(size_t) const;
		
	public:
		Ms2File() {
			size = 0;
			fname = "";
			buffer = new char [size];
		}
		Ms2File(std::string _fname){
			fname = _fname;
			size = 0;
		}
		~Ms2File(){
			//delete [] buffer;
		}
		
		//modifers
		bool read(std::string);
		bool read();
		
		//properties
		bool getScan(std::string, Spectrum&) const;
		bool getScan(size_t, Spectrum&) const;
	};
		
}//end of namespace

#endif /* ms2_hpp */
