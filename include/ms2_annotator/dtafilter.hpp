//
//  dtafilter.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/21/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef dtafilter_hpp
#define dtafilter_hpp

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <utils.hpp>
#include <scanData.hpp>

namespace Ms2_annotator{

	class DtaFilterFile;
	
	class DtaFilterFile{
	private:
		char* buffer;
		unsigned long size;
		std::string delim;
		utils::newline_type delimType;
		int beginLine;
		std::string fname;
		
		size_t getBeginLine(size_t) const;
		size_t getEndLine(size_t) const;
		std::string getScanLine(size_t) const;
		
	public:
		DtaFilterFile(){
			size = 0;
			fname = "";
		}
		DtaFilterFile(std::string _fname){
			size = 0;
			fname = _fname;
		}
		~DtaFilterFile(){
			delete [] buffer;
		}
		
		//modifers
		bool read(std::string);
		bool read();
		
		bool getFirstScan(const std::string&, scanData::Scan&) const;
		void getScans(const std::string&, scanData::scansType&) const;
		bool getScan(const std::string& _seq, scanData::scansType& _scans, bool force = true) const;
	};
}

#endif /* dtafilter_hpp */
