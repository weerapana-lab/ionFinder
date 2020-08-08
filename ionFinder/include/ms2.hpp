//
// ms2.hpp
// ionFinder
// -----------------------------------------------------------------------------
// MIT License
// Copyright 2020 Aaron Maurais
// -----------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -----------------------------------------------------------------------------
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
		size_t _scanCount{};
		
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
		
		//modifiers
		bool read(std::string);
		bool read();
		
		//properties
		bool getScan(std::string, Spectrum&) const;
		bool getScan(size_t, Spectrum&) const;
	};
		
}//end of namespace

#endif /* ms2_hpp */
