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
#include <fstream>
#include <algorithm>
#include <map>
#include <sstream>
#include <string>

#include <utils.hpp>

//TODO: maybe make this into a base class
namespace fastaFile {
	class FastaFile;
	
	const std::string PROT_SEQ_NOT_FOUND = "PROT_SEQ_NOT_FOUND";
	const std::string PEP_SEQ_NOT_FOUND = "PEP_SEQ_NOT_FOUND";
	
	class FastaFile{
	private:
		///fasta file path
		std::string _fname;
		
		///unparsed fasta file data
		char* _buffer;
		
		///_buffer length in chars
		size_t _size;
		
		///All peptide sequences which were already found are stored internally
		std::map<std::string, std::string> _foundSequences;
		
		std::string makeOffsetQuery(std::string proteinID, std::string database) const{
			std::string ret = ">" + database + "|" + proteinID + "|";
			return ret;
		}
		
	public:
		
		FastaFile(std::string fname = ""){
			_fname = fname;
			_size = 0;
			_buffer = new char [_size];
		}
		~FastaFile(){
			delete [] _buffer;
		}
		
		FastaFile(const FastaFile& rhs);
		
		//modifers
		FastaFile& operator = (FastaFile rhs);
		bool read();
		bool read(std::string fname);
		
		//properties
		std::string getSequence(std::string proteinID);
		//void getMatchRange()
		//std::string getModifiedResidue(std::string peptideSeq) const;
		std::string getModifiedResidue(std::string proteinID, std::string peptideSeq, int modLoc);
		std::string getModifiedResidue(std::string proteinID, std::string peptideSeq,
									   int modLoc, bool& found);
		int getMoodifiedResidueNumber(std::string peptideSeq, int modLoc) const;
	};
	
}

#endif /* fastaFile_hpp */
