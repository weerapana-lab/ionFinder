//
//  fastaFile.cpp
//  citFinder
//
//  Created by Aaron Maurais on 1/5/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#include <fastaFile.hpp>

/**
 \brief Search for \p proteinID in FastaFile::_buffer and extract protein sequence.
 \param proteinID uniprot ID of protein to search for.
 \param verbose Should detials of ids not found be printed to std::cerr?
 \return If found, parent protein sequence. If protein sequence is not found returns
 fastaFile::PROT_SEQ_NOT_FOUND.
 */
std::string fastaFile::FastaFile::getSequence(std::string proteinID, bool verbose)
{
	//if proteinID has already been parsed, return the seq and exit
	auto foundIt = _foundSequences.find(proteinID);
	if(foundIt != _foundSequences.end())
		return foundIt->second;
	
	//get offset of proteinID
	auto offsetIt = _idIndex.find(proteinID);
	if(offsetIt == _idIndex.end()){
		if(verbose){
			std::cerr << "Warning! ID: " << proteinID << " not found in fastaFile.\n";
		}
		_foundSequences[proteinID] = fastaFile::PROT_SEQ_NOT_FOUND;
		return fastaFile::PROT_SEQ_NOT_FOUND;
	}
	
	std::string temp(_buffer + offsetIt->second.first, _buffer + offsetIt->second.second);
	std::stringstream ss(temp);
	
	std::string line;
	std::string seq = "";
	int begin_count = 0;
	while(utils::safeGetline(ss, line))
	{
		if(line[0] == '>')
		{
			if(begin_count > 0)
				break;
			begin_count ++;
			continue;
		}
		seq += line;
	}
	
	_foundSequences[proteinID] = seq;
	return seq;
}

/**
 \brief Get position residue and position of \p modLoc in parent protein
 of \p peptideSeq.
 \param proteinID parent protein uniprot ID of \p peptideSeq
 \param peptideSeq unmodified peptide sequence.
 \param modLoc location of modified residue in peptide
 (where 0 is the beginning of the peptide.)
 */
std::string fastaFile::FastaFile::getModifiedResidue(std::string proteinID,
													 std::string peptideSeq,
													 int modLoc)
{
	bool temp;
	return getModifiedResidue(proteinID, peptideSeq, modLoc, false, temp);
}

/**
 \brief Get position residue and position of \p modLoc in parent protein
 of \p peptideSeq.
 \param proteinID parent protein uniprot ID of \p peptideSeq
 \param peptideSeq unmodified peptide sequence.
 \param modLoc location of modified residue in peptide
 (where 0 is the beginning of the peptide.)
 \param verbose Should detials of ids not found be printed to std::cerr?
 \param found set to false if first instance of searching for protein and it not being found
 */
std::string fastaFile::FastaFile::getModifiedResidue(std::string proteinID,
													 std::string peptideSeq,
													 int modLoc,
													 bool verbose,
													 bool& found)
{
	found = true;
	std::string seq;
	if(_foundSequences.find(proteinID) == _foundSequences.end()){
		seq = getSequence(proteinID, verbose);
		if(seq == fastaFile::PROT_SEQ_NOT_FOUND)
			found = false;
	}
	else seq = _foundSequences[proteinID];
	if(seq == fastaFile::PROT_SEQ_NOT_FOUND)
		return fastaFile::PROT_SEQ_NOT_FOUND;
	
	size_t begin = seq.find(peptideSeq);
	if(begin == std::string::npos)
		return fastaFile::PEP_SEQ_NOT_FOUND;
	size_t modNum = begin + modLoc;
	std::string ret = std::string(1, peptideSeq[modLoc]) + std::to_string(modNum + 1);
	
	return ret;
}

/**
 \brief iterate through _buffer to search for indecies of begining and end of all entries.
 */
void fastaFile::FastaFile::_buildIndex()
{
	//iterate through _buffer to search for where entries begin
	std::vector<size_t> combined, trIdx;
	utils::getIdxOfSubstr(_buffer, ">sp", combined);
	utils::getIdxOfSubstr(_buffer, ">tr", trIdx);
	
	//combine and sort vectors
	combined.insert(combined.end(), trIdx.begin(), trIdx.end());
	combined.push_back(_size); //add index to end of buffer
	std::sort(combined.begin(), combined.end());
	
	//build _idIndex
	int const scanLen = 20;
	std::string newID;
	size_t len = combined.size();
	for(size_t i = 0; i < len - 1; i++) //for all but last index in combined
	{
		char* c = &_buffer[combined[i] + 4];
		newID = "";
		for(int j = 0; j < scanLen; j++)
		{
			newID += *c;
			c += 1;
			if(*c == '|')
				break;
		}
		_idIndex[newID] = IntPair(combined[i], combined.at(i + 1));
	}
}


bool fastaFile::FastaFile::read()
{
	if(!BufferFile::read()) return false;
	_buildIndex();
	return true;
}

bool fastaFile::FastaFile::read(std::string fname)
{
	_fname = fname;
	return FastaFile::read();
}

