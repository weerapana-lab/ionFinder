//
//  bufferFile.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 1/30/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#ifndef bufferFile_hpp
#define bufferFile_hpp

#include <string>
#include <utils.hpp>

namespace base{
	class FileBuffer;
	
	class FileBuffer{
	private:
		//!file path
		std::string _fname;
		
		//!file buffer
		char* _buffer;
		
		//!_buffer length in chars
		size_t _size;
	public:
		FileBuffer(std::string fname = "");
		FileBuffer(const FileBuffer& rhs);
		
		~FileBuffer(){
			delete [] _buffer;
		}
		
		//modifers
		FileBuffer& operator = (FileBuffer rhs);
		bool read();
		bool read(std::string);
		
	};
}

#endif /* bufferFile_hpp */
