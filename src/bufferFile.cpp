//
//  bufferFile.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 1/30/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#include <bufferFile.hpp>

/**
 \brief constructor
 \param fname path of file to be read
 */
base::BufferFile::BufferFile(std::string fname)
{
	_fname = fname;
	_size = 0;
	_buffer = new char[_size];
}

/**
 \brief copy constructor
 \param rhs object to copy
 */
base::BufferFile::BufferFile(const base::BufferFile& rhs)
{
	//copy buffer
	_buffer = new char[rhs._size];
	std::copy(rhs._buffer, rhs._buffer + rhs._size, _buffer);
	
	//other vars
	_fname = rhs._fname;
	_size = rhs._size;
}

/**
 \brief copy assignment
 \param rhs object to copy
 */
base::BufferFile& base::BufferFile::operator = (base::BufferFile rhs)
{
	std::swap(_buffer, rhs._buffer);
	
	//other vars
	_fname = rhs._fname;
	_size = rhs._size;
	return *this;
}

/**
 \brief Read contents of \p fname into FileBuffer::_buffer
 \param fname path of file to read
 \return true if successful
 */
bool base::BufferFile::read(std::string fname)
{
	_fname = fname;
	return read();
}

/**
 \brief Read contents of FileBuffer::_fname into FileBuffer::_buffer
 \pre FileBuffer::_fname is not empty
 \return true if successful
 */
bool base::BufferFile::read()
{
	std::ifstream inF(_fname);
	if(!inF) return false;
	
	utils::readBuffer(_fname, &_buffer, _size);
	return true;
}
