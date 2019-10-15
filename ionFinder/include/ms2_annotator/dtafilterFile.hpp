//
//  dtafilterFile.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/21/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
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
		
		//modifers
		bool read(std::string);
		bool read();
		
		//bool getFirstScan(const std::string&, scanData::Scan&) const;
		void getScans(const std::string&, scanData::scansType&) const;
		bool getScan(const std::string& _seq, scanData::scansType& _scans, bool force = true) const;
	};
}

#endif /* dtafilterFile_hpp */
