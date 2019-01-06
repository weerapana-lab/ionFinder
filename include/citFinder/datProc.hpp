//
//  datProc.hpp
//  citFinder
//
//  Created by Aaron Maurais on 12/10/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#ifndef datProc_hpp
#define datProc_hpp

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <set>

#include <citFinder/citFinder.hpp>
#include <citFinder/params.hpp>
#include <dtafilter.hpp>
#include <fastaFile.hpp>

#include <peptide.hpp>
#include <ms2.hpp>

namespace CitFinder{

	class RichFragmentIon;
	class PeptideStats;
	class PeptideFragmentsMap;
	
	const std::string FRAG_DELIM = "|";
	int const N_ION_TYPES = 6;
	const std::string ION_TYPES_STR [] = {"frag", "detFrag", "ambModFrag",
		"detNLFrag", "ambFrag", "artNLFrag"};
	
	int const PROGRESS_SLEEP_TIME = 1;
	int const MAX_PROGRESS_ITTERATIONS = 10;
	int const PROGRESS_BAR_WIDTH = 60;
	typedef std::map<std::string, ms2::Ms2File> Ms2Map;
	
	bool findFragmentsParallel(const std::vector<Dtafilter::Scan>&,
							   std::vector<PeptideNamespace::Peptide>&,
							   const CitFinder::Params&);
	
	void findFragments_threadSafe(const std::vector<Dtafilter::Scan>& scans,
								  size_t beg, size_t end,
								  const Ms2Map& ms2Map,
								  std::vector<PeptideNamespace::Peptide>& peptides,
								  const CitFinder::Params& pars,
								  bool* success, std::atomic<size_t>& scansIndex);
	
	void findFragmentsProgress(std::atomic<size_t>& scansIndex, size_t count,
							   unsigned int nThread,
							   int sleepTime = PROGRESS_SLEEP_TIME);
	
	bool readMs2s(Ms2Map&, const std::vector<Dtafilter::Scan>&, const CitFinder::Params&);
	
	bool findFragments(const std::vector<Dtafilter::Scan>& scans,
					   std::vector<PeptideNamespace::Peptide>& peptides,
					   CitFinder::Params& pars);
	
	/*void analyzeSequencesParallel(std::vector<Dtafilter::Scan>& scans,
								  const std::vector<PeptideNamespace::Peptide>& peptides,
								  std::vector<PeptideStats>& peptideStats,
								  const CitFinder::Params& pars);*/
	
	bool analyzeSequences(std::vector<Dtafilter::Scan>&,
						  const std::vector<PeptideNamespace::Peptide>&,
						  std::vector<PeptideStats>&,
						  const CitFinder::Params&);
	
	bool printPeptideStats(const std::vector<PeptideStats>&,
						   std::string);
	
	bool allignSeq(const std::string& ref, const std::string& query, size_t& beg, size_t& end);
	
	///Used to map fragment ions to a peptide sequence
	class PeptideFragmentsMap{
	private:
		std::string _sequence;
		std::map<std::string, std::string> fragmentMap;
		
	public:
		///blank constructor
		PeptideFragmentsMap() {
			_sequence = "";
		}
		///Constructor from string
		PeptideFragmentsMap(std::string sequence){
			_sequence = sequence;
			populateMap(_sequence);
		}
		~PeptideFragmentsMap() {}
		
		//modifers
		void populateMap(std::string);
		void clear();
		
		//properties
		std::string getIonSeq(std::string) const;
		std::string getIonSeq(char, int) const;
	};
	
	///Used to keep track of fragment ions which were found in sequence analysis.
	class RichFragmentIon : public PeptideNamespace::FragmentIon{
	private:
		std::string sequence;
	public:
		///Blank constructor
		RichFragmentIon() : FragmentIon(){
			sequence = "";
		}
		///constructor from FragmentIon
		RichFragmentIon(const PeptideNamespace::FragmentIon& f) :
			PeptideNamespace::FragmentIon(f){
			sequence = "";
		}
		
		//modifers
		void calcSequence(const PeptideFragmentsMap&);
		
		//properties
		std::string getSequence() const{
			return sequence;
		}
	};
	
	
	class PeptideStats{
	public:
		friend bool analyzeSequences(std::vector<Dtafilter::Scan>&,
									 const std::vector<PeptideNamespace::Peptide>&,
									 std::vector<PeptideStats>&,
									 const CitFinder::Params&);
		
		friend bool printPeptideStats(const std::vector<PeptideStats>&,
									  std::string);
		enum class IonType{
			//!All fragments identified
			FRAG,
			//!B or Y fragments with modification not containing N or Q
			DET_FRAG,
			//!B or Y fragments with modification containing N or Q
			AMB_MOD_FRAG,
			//!Cit determining NL fragments
			DET_NL_FRAG,
			//!B or Y ions not containing modification
			AMB_FRAG,
			 //!NL fragments not contatining modification
			ART_NL_FRAG,
			Last,
			First = FRAG,
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
			fullSequence = p.getFullSequence();
			charge = p.getCharge();
			mass = p.getMass();
			initModLocs();
		}
		~PeptideStats(){
			//delete _scan;
		}
		
		//modifers
		//void setScan(Dtafilter::Scan)
		void addSeq(const CitFinder::RichFragmentIon&, const std::string&);
		static std::string ionTypeToStr(const IonType&);
	};
	
	inline PeptideStats::IonType operator++(PeptideStats::IonType& x ){
		return x = (PeptideStats::IonType)(((int)(x) + 1));
	}
}

#endif /* datProc_hpp */
