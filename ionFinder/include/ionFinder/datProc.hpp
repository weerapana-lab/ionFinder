//
//  datProc.hpp
//  ionFinder
//
//  Created by Aaron Maurais on 12/10/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#ifndef datProc_hpp
#define datProc_hpp

#include <cassert>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <atomic>

#include <ionFinder/ionFinder.hpp>
#include <ionFinder/params.hpp>
#include <dtafilter.hpp>
#include <fastaFile.hpp>
#include <peptide.hpp>
#include <ms2.hpp>
#include <scanData.hpp>

namespace IonFinder{
	
	class FragmentData;
	class RichFragmentIon;
	class PeptideStats;
	class PeptideFragmentsMap;
	
	const std::string FRAG_DELIM = "|";
	int const N_ION_TYPES = 6;
	const std::string ION_TYPES_STR [] = {"frag", "detFrag", "ambFrag",
		 "detNLFrag", "ambNLFrag", "artNLFrag"};
	
	//!Progress bar sleep time in seconds
	int const PROGRESS_SLEEP_TIME = 1;
	//!Max iterations of progressbar loop with no progress before quitting
	int const MAX_PROGRESS_ITTERATIONS = 5;
	//!Progress bar width in chars
	int const PROGRESS_BAR_WIDTH = 60;
	typedef std::map<std::string, ms2::Ms2File> Ms2Map;
	
	bool findFragmentsParallel(std::vector<Dtafilter::Scan>&,
							   std::vector<PeptideNamespace::Peptide>&,
							   const IonFinder::Params&);
	
	void findFragments_threadSafe(std::vector<Dtafilter::Scan>& scans,
								  size_t beg, size_t end,
								  const Ms2Map& ms2Map,
								  std::vector<PeptideNamespace::Peptide>& peptides,
								  const IonFinder::Params& pars,
								  bool* success, std::atomic<size_t>& scansIndex);
	
	void findFragmentsProgress(std::atomic<size_t>& scansIndex, size_t count,
							   unsigned int nThread,
							   int sleepTime = PROGRESS_SLEEP_TIME);
	
	bool readMs2s(Ms2Map&, const std::vector<Dtafilter::Scan>&);
	
	bool findFragments(std::vector<Dtafilter::Scan>& scans,
					   std::vector<PeptideNamespace::Peptide>& peptides,
					   IonFinder::Params& pars);
	
	bool analyzeSequences(std::vector<Dtafilter::Scan>&,
						  const std::vector<PeptideNamespace::Peptide>&,
						  std::vector<PeptideStats>&,
						  const IonFinder::Params&);
	
	bool printPeptideStats(const std::vector<PeptideStats>&,
						   const IonFinder::Params&);
	
	bool allignSeq(const std::string& ref, const std::string& query, size_t& beg, size_t& end);
		
	class PeptideStats{
	public:
		friend bool analyzeSequences(std::vector<Dtafilter::Scan>&,
									 const std::vector<PeptideNamespace::Peptide>&,
									 std::vector<PeptideStats>&,
									 const IonFinder::Params&);
		
		friend bool printPeptideStats(const std::vector<PeptideStats>&,
									  const IonFinder::Params&);
		enum class IonType{
			//!All fragments identified
			FRAG,
			//!B or Y fragments with modification not containing amb residue
			DET_FRAG,
			//!B or Y ions not containing modification or ambigious residue
			AMB_FRAG,
			//!Modification determining NL fragments
			DET_NL_FRAG,
            //!Ambiguous NL fragment
            AMB_NL_FRAG,
			//!NL fragments not containing modification
			ART_NL_FRAG,
			Last,
			First = FRAG
		};
	private:
		
		typedef std::pair<std::string, int> IonTypeDatType;
		typedef std::map<IonType, IonTypeDatType> IonTypesCountType;
		IonTypesCountType ionTypesCount;
		
		//!Does peptide contain cit
		std::string containsCit;
		
		//!Fragment deliminator in output
		std::string _fragDelim;
		
		//!sequence with modification removed
		std::string sequence;
		//!full sequence with modification
		std::string fullSequence;
		int charge;
		//!Monoisotopic mass of neutral peptide
		double mass;
		//!Positions of modifications
		std::vector<size_t> modLocs;
		
		//!Positions of modified residues on protein
		std::string modResidues;
		
		//!pointer to corresponding scan object
		Dtafilter::Scan* _scan;
		
		void initStats();
		//void initModLocs(const char* diffmods = "*");
		bool containsAmbResidues(const std::string& ambResidues, std::string fragSeq) const;
		void calcContainsCit();
		void incrementIonCount(std::string ionStr, IonTypeDatType& ion, int inc = 1);
		void addMod(std::string mod);
	
	public:		
		PeptideStats(){
			_scan = new Dtafilter::Scan;
			initStats();
			_fragDelim = FRAG_DELIM;
			
			//peptide data
			sequence = "";
			fullSequence = "";
			charge = 0;
			mass = 0;
		}
		explicit PeptideStats(const PeptideNamespace::Peptide& p){
			//PeptideStats data
			_scan = new Dtafilter::Scan;
			_fragDelim = FRAG_DELIM;
			initStats();
			
			//Peptide data
			sequence = p.getSequence();
			fullSequence = scanData::removeStaticMod(p.getFullSequence(), false);
			charge = p.getCharge();
			mass = p.getMass();
			modLocs.clear();
			modLocs.insert(modLocs.begin(), p.getModLocs().begin(), p.getModLocs().end());
		}
		~PeptideStats() = default;
		
		//modifers
		void addSeq(const PeptideNamespace::FragmentIon&, const std::string&);
		static std::string ionTypeToStr(const IonType&);
	};
	
	inline PeptideStats::IonType operator++(PeptideStats::IonType& x ){
		return x = (PeptideStats::IonType)(((int)(x) + 1));
	}
}

#endif /* datProc_hpp */
