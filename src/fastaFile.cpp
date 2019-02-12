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
 \return If found, parent protein sequence. If protein sequence is not found returns
 fastaFile::PROT_SEQ_NOT_FOUND.
 */
std::string fastaFile::FastaFile::getSequence(std::string proteinID)
{
	std::string ret = "";
	
	//first search for UniProtKB/Swiss-Prot IDs (sp in db position)
	std::string query = makeOffsetQuery(proteinID, "sp");
	size_t offset = utils::offset(_buffer, _size, query);
	size_t queryLen = query.size();
	
	//if not found, try UniProtKB/TrEMBL (tr in db position)
	if(offset == _size){
		query = makeOffsetQuery(proteinID, "tr");
		offset = utils::offset(_buffer, _size, query);
	}
	
	//if still not found warn user and exit
	if(offset == _size){
		std::cerr << "Warning! ID: " << proteinID << " not found in fastaFile.\r";
		//std::cerr << query << std::endl;
		std::cerr.flush();
		_foundSequences[proteinID] = fastaFile::PROT_SEQ_NOT_FOUND;
		return fastaFile::PROT_SEQ_NOT_FOUND;
	}
	
	size_t end = utils::offset(_buffer + offset + queryLen,
							   _size - (offset + queryLen), ">") + queryLen;
	
	std::string temp(_buffer + offset, _buffer + offset + end);
	std::stringstream ss(temp);
	
	std::string line;
	while(utils::safeGetline(ss, line))
	{
		if(line[0] == '>') continue;
		ret += line;
	}
	
	_foundSequences[proteinID] = ret;
	return ret;
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
	return getModifiedResidue(proteinID, peptideSeq, modLoc, temp);
}

/**
 \brief Get position residue and position of \p modLoc in parent protein
 of \p peptideSeq.
 \param proteinID parent protein uniprot ID of \p peptideSeq
 \param peptideSeq unmodified peptide sequence.
 \param modLoc location of modified residue in peptide
 (where 0 is the beginning of the peptide.)
 \param found set to false if first instance of searching for protein and it not being found
 */
std::string fastaFile::FastaFile::getModifiedResidue(std::string proteinID,
													 std::string peptideSeq,
													 int modLoc,
													 bool& found)
{
	found = true;
	std::string seq;
	if(_foundSequences.find(proteinID) == _foundSequences.end()){
		seq = getSequence(proteinID);
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
