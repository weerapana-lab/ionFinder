//
//  fastaFile.cpp
//  citFinder
//
//  Created by Aaron Maurais on 1/5/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#include <fastaFile.hpp>

///copy constructor
fastaFile::FastaFile::FastaFile(const FastaFile& rhs)
{
	//copy buffer
	buffer = new char[rhs.size];
	std::copy(rhs.buffer, rhs.buffer + rhs.size, buffer);
	
	//other vars
	size = rhs.size;
}

///copy assignment
fastaFile::FastaFile& fastaFile::FastaFile::operator = (FastaFile rhs)
{
	std::swap(buffer, rhs.buffer);
	
	size = rhs.size;
	return *this;
}

bool fastaFile::FastaFile::read(std::string fname)
{
	return true;
}

bool fastaFile::FastaFile::read()
{
	return true;
}
