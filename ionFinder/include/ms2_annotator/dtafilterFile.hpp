//
// dtafilterFile.hpp
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

#ifndef dtafilterFile_hpp
#define dtafilterFile_hpp

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <utils.hpp>
#include <scanData.hpp>
#include <dtafilter.hpp>

namespace Ms2_annotator{

	class DtaFilterFile;
	
	class DtaFilterFile{
	private:
		std::string _fname;
		
		//!list of scans in filter file
		std::vector<Dtafilter::Scan> _scans;
		
	public:
		DtaFilterFile(){
			_fname = "";
		}
		DtaFilterFile(std::string fname){
			_fname = fname;
		}
		~DtaFilterFile(){}
		
		//modifiers
		bool read(std::string);
		bool read();
		
		//bool getFirstScan(const std::string&, scanData::Scan&) const;
		void getScans(const std::string&, scanData::scansType&) const;
		bool getScan(const std::string& _seq, scanData::scansType& _scans, bool force = true) const;
	};
}

#endif /* dtafilterFile_hpp */
