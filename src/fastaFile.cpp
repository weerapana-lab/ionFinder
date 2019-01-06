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
	_buffer = new char[rhs._size];
	std::copy(rhs._buffer, rhs._buffer + rhs._size, _buffer);
	
	//other vars
	_size = rhs._size;
}

///copy assignment
fastaFile::FastaFile& fastaFile::FastaFile::operator = (FastaFile rhs)
{
	std::swap(_buffer, rhs._buffer);
	
	_size = rhs._size;
	return *this;
}

/**
 \brief Read contents of \p fname into FastaFile::_buffer
 \param fname path of file to read
 \return true if sucessful
 */
bool fastaFile::FastaFile::read(std::string fname)
{
	_fname = fname;
	return read();
}

/**
 \brief Read contents of FastaFile::_fname into FastaFile::_buffer
 \pre FastaFile::_fname is not empty
 \return true if sucessful
 */
bool fastaFile::FastaFile::read()
{
	std::ifstream inF(_fname);
	if(!inF) return false;
	
	utils::readBuffer(_fname, &_buffer, _size);
	return true;
}

std::string fastaFile::FastaFile::getSequence(std::string proteinID)
{
	std::string ret = "";
	
	//first search for UniProtKB/Swiss-Prot IDs (sp in db position)
	const char* query = makeOffsetQuery(proteinID, "sp");
	size_t offset = utils::offset(_buffer, _size, query);
	size_t queryLen = strlen(query);
	
	//if not found, try UniProtKB/TrEMBL (tr in db position)
	if(offset == _size){
		query = makeOffsetQuery(proteinID, "tr");
		offset = utils::offset(_buffer, _size, query);
	}
	
	//if still not found warn user and exit
	if(offset == _size){
		std::cerr << "Warning! ID: " << proteinID << " not found in fastaFile." << NEW_LINE;
		_foundSequences[proteinID] = fastaFile::PROT_SEQ_NOT_FOUND;
		return fastaFile::PROT_SEQ_NOT_FOUND;
	}
	
	size_t end = utils::offset(_buffer + offset + queryLen,
							   _size - (offset + queryLen), ">") + queryLen;
	
	std::string temp(_buffer + offset, _buffer + offset + end);
	std::stringstream ss(temp);
	
	std::string line;
	while(utils::safeGetLine(ss, line))
	{
		if(line[0] == '>') continue;
		ret += line;
	}
	
	_foundSequences[proteinID] = ret;
	return ret;
}

std::string fastaFile::FastaFile::getModifiedResidue(std::string protinID,
													 std::string peptideSeq,
													 int modLoc)
{
	std::string seq;
	if(_foundSequences.find(protinID) == _foundSequences.end())
		seq = getSequence(protinID);
	else seq = _foundSequences[protinID];
	
	return "FUCK!";
}
