//
//  fileBuffer.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 1/30/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#ifndef fileBuffer_hpp
#define fileBuffer_hpp

#include <string>
#include <utils.hpp>

namespace base{
	class BufferFile;
	
	//!Base class for reading and manipulating large file buffers.
	class BufferFile{
	protected:
		//!file path
		std::string _fname;
		
		//!file buffer
		char* _buffer;
		
		//!_buffer length in chars
		size_t _size;
	public:
		BufferFile(std::string fname = "");
		BufferFile(const BufferFile& rhs);
		
		~BufferFile(){
			delete [] _buffer;
		}
		
		//modifers
		BufferFile& operator = (BufferFile rhs);
		bool read();
		bool read(std::string);
	};
}

#endif /* fileBuffer_hpp */
