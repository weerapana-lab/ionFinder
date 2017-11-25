//
//  sequestParams.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/22/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef sequestParams_hpp
#define sequestParams_hpp

#include <iostream>
#include <map>
#include <stdexcept>
#include "../include/utils.hpp"
#include "../include/aaDB.hpp"

using namespace std;

namespace seqpar{
	
	string const BAD_SMOD = "BAD";
	string const DIFF_MOD_LINE = "diff_search_options";
	
	class SequestParamsFile;
	
	class SequestParamsFile{
	private:
		map<string, string> smodMap;
		string fname;
		aaDB::aminoAcidsDBType aaMap;
		
		void initSmodMap();
	public:
		
		SequestParamsFile(){
			initSmodMap();
			fname = "";
		}
		SequestParamsFile(string _fname){
			initSmodMap();
			fname = _fname;
		}
		
		bool read(string);
		bool read(){
			return read(fname);
		}
		
		aaDB::aminoAcidsDBType getAAMap() const;		
	};
}

#endif /* sequestParams_hpp */
