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
#include <map>

#include <utils.hpp>
#include <bufferFile.hpp>
#include <peptide.hpp>
#include <ms2Spectrum.hpp>

namespace ms2 {
	
	int const MD_NUM = 4;
	size_t const SCAN_INDEX_NOT_FOUND = std::string::npos;
	
	class Ms2File;
	
	class Ms2File : public utils::BufferFile{
	private:
		typedef std::pair<size_t, size_t> IntPair;
		typedef std::vector<IntPair> OffsetIndexType;
		
		//!Stores pairs of offset values for scans
		OffsetIndexType _offsetIndex;
		//!Maps scan numbers to indecies in _offsetIndex
		std::map<size_t, size_t> _scanMap;
		//!Actual number of scans read from file
		size_t _scanCount;
		
		//metadata
		//!base file name without extension
		std::string _parentMs2;
		size_t firstScan, lastScan;
		std::string dataType;
		std::string scanType;
		
		bool getMetaData();
		void calcParentMs2(std::string path){
			_parentMs2 = utils::baseName(utils::removeExtension(path));
		}
		
		void copyMetadata(const Ms2File& rhs);
		void initMetadata();
		void _buildIndex();
		size_t _getScanIndex(size_t) const;
		
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
