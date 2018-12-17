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

#include <citFinder/citFinder.hpp>
#include <citFinder/params.hpp>
#include <dtafilter.hpp>

#include <peptide.hpp>
#include <ms2.hpp>

namespace CitFinder{

	class RichFragmentIon;
	class PeptideStats;
	class PeptideFragmentsMap;
	
	const std::string FRAG_DELIM = "|";
	
	bool findFragments(const std::vector<Dtafilter::Scan>&,
					   std::vector<PeptideNamespace::Peptide>&,
					   CitFinder::Params&);
	
	void analyzeSequences(std::vector<Dtafilter::Scan>&,
						  const std::vector<PeptideNamespace::Peptide>&,
						  std::vector<PeptideStats>&,
						  const CitFinder::Params&);
	
	void printPeptideStats(const std::vector<PeptideStats>&,
						   std::ostream&);
	
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
		friend void analyzeSequences(std::vector<Dtafilter::Scan>&,
									 const std::vector<PeptideNamespace::Peptide>&,
									 std::vector<PeptideStats>&,
									 const CitFinder::Params&);
		
		friend void printPeptideStats(const std::vector<PeptideStats>&,
									  std::ostream&);
	private:
		//!Total fragment ions
		int nFrag;
		//!Number fragments without modification
		int nAmbFrag;
		//!Number fragments with modification
		int nDetFrag;
		//!Number ambiguous NL fragments
		int nAmbNLFrag;
		//!Number determining NL fragments
		int nDetNLFrag;
		//!Number of artifact NL fragments
		int nArtNLFrag;
		
		//!Fragment ions
		std::string frag;
		//!Fragments without modification
		std::string ambFrag;
		//!Fragments with modification
		std::string detFrag;
		//!Ambiguous NL fragments
		std::string ambNLFrag;
		//!Determining NL fragments
		std::string detNLFrag;
		//!Artifact NL fragments
		std::string artNLFrag;
		
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
		
		//!pointer to corresponding scan object
		Dtafilter::Scan* _scan;
		
		void initStats();
		void initModLocs(const char* diffmods = "*");
		void addChar(std::string, std::string&);
		bool containsAmbResidues(const std::string& ambResidues, std::string fragSeq) const;
		void calcContainsCit();
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
	};
}

#endif /* datProc_hpp */
