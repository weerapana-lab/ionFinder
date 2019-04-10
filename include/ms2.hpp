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
#include <bufferFile.hpp>
#include <peptide.hpp>
#include <ms2Spectrum.hpp>

namespace ms2 {
	
	size_t const NUM_DIGITS_IN_SCAN = 6;
	int const MD_NUM = 4;
	
	class Ms2File;
	
	class Ms2File : public utils::BufferFile{
	private:
		//metadata
		//!base file name without extension
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

		Ms2File(std::string fname = "") : BufferFile(fname){
			initMetadata();
		}
		~Ms2File(){}
		
		//!copy constructor
		Ms2File(const Ms2File& rhs) : BufferFile(rhs){
			copyMetadata(rhs);
		}
		//!copy assignment
		Ms2File& operator = (Ms2File rhs)
		{
			BufferFile::operator=(rhs);
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
