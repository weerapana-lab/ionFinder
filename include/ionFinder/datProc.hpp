//
//  datProc.hpp
//  citFinder
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
		"ambModFrag", "detNLFrag", "artNLFrag"};
	
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
	
	/**
	 Stores fragment ion sequence and begining and end indecies relative
	 to peptide seq.
	 */
	class FragmentData{
	protected:
		//!sequence of fragment
		std::string _sequence;
		//!index of begining of fragment relative to full sequence
		size_t _begin;
		//!index of end of fragment relative to full sequence
		size_t _end;
	public:
		//!default constructor
		FragmentData(){
			_sequence = "";
			_begin = std::string::npos;
			_end = std::string::npos;
		}
		FragmentData(std::string, size_t, size_t);
		~FragmentData(){}
		
		//modifers
		FragmentData& operator = (const FragmentData& rhs);
		//void operator = (const FragmentData& rhs);
		
		//properties
		std::string getSequence() const{
			return _sequence;
		}
		size_t getBegin() const{
			return _begin;
		}
		size_t getEnd() const{
			return _end;
		}
	};
	
	//!Used to map fragment ions to a peptide sequence
	class PeptideFragmentsMap{
	private:
		typedef std::map<std::string, FragmentData> FragmentMapType;
		FragmentMapType fragmentMap;
		std::string _sequence;
		
	public:
		//!default constructor
		PeptideFragmentsMap() {
			_sequence = "";
		}
		//!Constructor from string
		PeptideFragmentsMap(std::string sequence){
			_sequence = sequence;
			populateMap(_sequence);
		}
		~PeptideFragmentsMap() {}
		
		//modifers
		void populateMap(std::string);
		void clear();
		
		//properties
		FragmentData at(std::string key) const{
			return fragmentMap.at(key);
		}
	};
	
	//!Used to keep track of fragment ions which were found in sequence analysis.
	class RichFragmentIon : public PeptideNamespace::FragmentIon, public FragmentData{
	public:
		//!default constructor
		RichFragmentIon() : FragmentIon(){
			_sequence = "";
		}
		//!constructor from FragmentIon
		RichFragmentIon(const PeptideNamespace::FragmentIon& f) :
			PeptideNamespace::FragmentIon(f){
			_sequence = "";
		}
		
		//modifers
		void calcSequence(const PeptideFragmentsMap&);
	};
	
	
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
			//!B or Y ions not containing modification
			AMB_FRAG,
			//!B or Y fragments with modification containing amb residue
			AMB_MOD_FRAG,
			//!Modification determining NL fragments
			DET_NL_FRAG,
			//!NL fragments not contatining modification
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
		void initModLocs(const char* diffmods = "*");
		static void addChar(std::string, std::string&, std::string fragDelim = FRAG_DELIM);
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
		PeptideStats(const PeptideNamespace::Peptide& p){
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
		~PeptideStats(){
			//delete _scan;
		}
		
		//modifers
		//void setScan(Dtafilter::Scan)
		void addSeq(const IonFinder::RichFragmentIon&, const std::string&);
		static std::string ionTypeToStr(const IonType&);
	};
	
	inline PeptideStats::IonType operator++(PeptideStats::IonType& x ){
		return x = (PeptideStats::IonType)(((int)(x) + 1));
	}
}

#endif /* datProc_hpp */
