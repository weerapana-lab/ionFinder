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
		///stores unparsed ms2 file
		char* buffer;
		///number of chars in buffer
		size_t size;
		
		//metadata
		///full file name used for reading
		std::string fname;
		///base file name without extension
		std::string _parentMs2;
		size_t firstScan, lastScan;
		std::string dataType;
		std::string scanType;
		
		bool getMetaData();
		const char* makeOffsetQuery(std::string) const;
		const char* makeOffsetQuery(size_t) const;
		void calcParentMs2(std::string path){
			_parentMs2 = utils::baseName(utils::removeExtension(path));
		}
		
		void copyMetadata(const Ms2File& rhs);
		void initMetadata();
		
	public:

		Ms2File(std::string _fname = ""){
			fname = _fname;
			size = 0;
			buffer = new char [size];
			initMetadata();
		}
		~Ms2File(){
			delete [] buffer;
		}
		
		///copy constructor
		Ms2File(const Ms2File& rhs)
		{
			//copy buffer
			buffer = new char[rhs.size];
			std::copy(rhs.buffer, rhs.buffer + rhs.size, buffer);
			
			//other vars
			size = rhs.size;
			copyMetadata(rhs);
		}
		///copy assignment
		Ms2File& operator = (Ms2File rhs)
		{
			std::swap(buffer, rhs.buffer);
			
			size = rhs.size;
			copyMetadata(rhs);
			return *this;
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
