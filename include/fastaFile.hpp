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
#include <bufferFile.hpp>

namespace fastaFile {
	class FastaFile;
	
	const std::string PROT_SEQ_NOT_FOUND = "PROT_SEQ_NOT_FOUND";
	const std::string PEP_SEQ_NOT_FOUND = "PEP_SEQ_NOT_FOUND";
	
	class FastaFile : public base::BufferFile{
	private:
		typedef std::pair<size_t, size_t> IntPair;
		typedef std::map<std::string, IntPair> IndexMapType;
		
		//!All peptide sequences which were already found are stored internally
		std::map<std::string, std::string> _foundSequences;
		//!Stores begining and ending offset indecies of eacn protein ID
		IndexMapType _idIndex;
		
		void _buildIndex();
		
	public:
		FastaFile(std::string fname = "") : BufferFile(fname) {}
		FastaFile(const FastaFile& rhs) : BufferFile(rhs) {}
		~FastaFile(){}
		
		//modifers
		FastaFile& operator = (FastaFile rhs){
			BufferFile::operator=(rhs);
			_idIndex = rhs._idIndex;
			_foundSequences = rhs._foundSequences;
			return *this;
		}
		bool read();
		bool read(std::string);
		
		//properties
		std::string getSequence(std::string proteinID, bool verbose = false);
		std::string getModifiedResidue(std::string proteinID, std::string peptideSeq, int modLoc);
		std::string getModifiedResidue(std::string proteinID, std::string peptideSeq,
									   int modLoc, bool verbose, bool& found);
		int getMoodifiedResidueNumber(std::string peptideSeq, int modLoc) const;
	};
}

#endif /* fastaFile_hpp */
