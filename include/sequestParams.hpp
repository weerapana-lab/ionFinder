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
#include <string>

#include <utils.hpp>
#include <aaDB.hpp>

namespace seqpar{

	std::string const DIFF_MOD_LINE = "diff_search_options";
	std::string const DATABSE_NAME_LINE = "database_name";
	
	class SequestParamsFile;
	
	class SequestParamsFile{
	private:
		std::map<std::string,std::string> smodMap;
		std::string fname;
		aaDB::aminoAcidsDBType aaMap;
		//!Path to fasta file used for database searching
		std::string fasta_path;
		
		void initSmodMap();
	public:
		
		SequestParamsFile(){
			initSmodMap();
			fname = "";
		}
		SequestParamsFile(std::string _fname){
			initSmodMap();
			fname = _fname;
		}
		
		bool read(std::string);
		bool read(){
			return read(fname);
		}
		
		aaDB::aminoAcidsDBType getAAMap() const{
			return aaMap;
		}
	};
}

#endif /* sequestParams_hpp */
