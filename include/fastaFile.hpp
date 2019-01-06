//
//  fastaFile.hpp
//  citFinder
//
//  Created by Aaron Maurais on 1/5/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#ifndef fastaFile_hpp
#define fastaFile_hpp

#include <iostream>
#include <algorithm>

#include <utils.hpp>

namespace fastaFile {
	class FastaFile;
	
	class FastaFile{
	private:
		std::string _fname;
		char* buffer;
		size_t size;
		
	public:
		
		FastaFile(std::string fname = ""){
			_fname = fname;
			size = 0;
			buffer = new char [size];
		}
		~FastaFile(){
			delete [] buffer;
		}
		
		FastaFile(const FastaFile& rhs);
		
		//modifers
		FastaFile& operator = (FastaFile rhs);
		bool read();
		bool read(std::string fname);
	};
	
}

#endif /* fastaFile_hpp */
