//
// datProc.hpp
// ionFinder
// -----------------------------------------------------------------------------
// MIT License
// Copyright 2020 Aaron Maurais
// -----------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -----------------------------------------------------------------------------
//

#ifndef datProc_hpp
#define datProc_hpp

#include <cassert>
#include <map>
#include <utility>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <stdexcept>
#include <set>
#include <cmath>
#include <limits>

#include <constants.hpp>
#include <ionFinder/ionFinder.hpp>
#include <ionFinder/params.hpp>
#include <dtafilter.hpp>
#include <fastaFile.hpp>
#include <peptide.hpp>
#include <scanData.hpp>
#include <msInterface.hpp>
#include <ms2Spectrum.hpp>

namespace IonFinder{
	
	class FragmentData;
	class RichFragmentIon;
	class PeptideStats;
	class PeptideFragmentsMap;
	
	const std::string FRAG_DELIM = ";";
	int const N_ION_TYPES = 5;
	const std::string ION_TYPES_STR [] = {"frag", "det", "amb", "detNL", "artNL"};
	const std::string CONTAINS_CIT_STR [] {"false", "ambiguous", "likely", "true"};
	
	//!Progress bar sleep time in seconds
	int const PROGRESS_SLEEP_TIME = 1;
	//!Max iterations of progress bar loop with no progress before quitting
	int const MAX_PROGRESS_ITTERATIONS = 600;
	//!Progress bar width in chars
	int const PROGRESS_BAR_WIDTH = 60;

	bool findFragmentsParallel(std::vector<Dtafilter::Scan>&,
							   std::vector<PeptideNamespace::Peptide>&,
							   const IonFinder::Params&);

    void findFragments_(std::vector<Dtafilter::Scan>& scans,
                        size_t beg, size_t end,
                        std::vector<PeptideNamespace::Peptide>& peptides,
                        const IonFinder::Params& pars,
                        bool* success, std::atomic<size_t>& scansIndex);

    void findFragments_threadSafe(std::vector<Dtafilter::Scan>& scans,
                                  size_t beg, size_t end,
                                  ms2::MsInterface& msInterface,
                                  std::vector<PeptideNamespace::Peptide>& peptides,
                                  const IonFinder::Params& pars,
                                  bool* success, std::atomic<size_t>& scansIndex);

	void findFragmentsProgress(std::atomic<size_t>& scansIndex, size_t count,
							   const std::string& message,
							   int sleepTime = PROGRESS_SLEEP_TIME);
	
	bool findFragments(std::vector<Dtafilter::Scan>& scans,
					   std::vector<PeptideNamespace::Peptide>& peptides,
					   IonFinder::Params& pars);
	
	bool analyzeSequences(std::vector<Dtafilter::Scan>&,
						  const std::vector<PeptideNamespace::Peptide>&,
						  std::vector<PeptideStats>&,
						  const IonFinder::Params&);

	bool printFragmentIntensities(const std::vector<PeptideStats>&, std::string, std::string = "");
	
	bool printPeptideStats(const std::vector<PeptideStats>&,
						   const IonFinder::Params&);
	
	bool allignSeq(const std::string& ref, const std::string& query, size_t& beg, size_t& end);

	class FragmentIon{
	private:
		std::string _ionStr;
		double _intensity;
	public:
		FragmentIon() { _ionStr = ""; _intensity = 0.0; }
		FragmentIon(std::string ionStr, double intensity) {
			_ionStr = std::move(ionStr); _intensity = intensity;
        }

		//! less than for std::set
		bool operator < (const FragmentIon& rhs) const {
            return _ionStr < rhs._ionStr;
        }
		std::string getIonStr() const {
            return _ionStr;
        }
        double getIntensity() const {
            return _intensity;
        }
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
			DET,
			//!B or Y ions not containing modification or ambiguous residue
			AMB,
			//!Modification determining NL fragments
			DET_NL,
			//!NL fragments not containing modification
			ART_NL,
			Last,
			First = FRAG
		};

		enum class ContainsCitType {FALSE = 0, AMBIGUOUS = 1, LIKELY = 2, TRUE = 3};
	private:
		
		typedef std::set<IonFinder::FragmentIon> IonStrings;
		typedef std::map<IonType, IonStrings> IonTypesCountType;
		IonTypesCountType ionTypesCount;
		
		//! Does the overall peptide contain cit?
		ContainsCitType containsCit;

        //! Does the site contain cit?
        ContainsCitType thisContainsCit;

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
		//!Position of this modification
		size_t modIndex;

		//!unique identifier of peptide
		unsigned int _id;
		
		//!Positions of modified residues on protein
		std::string modResidues;
		
		//!pointer to corresponding scan object
		Dtafilter::Scan* _scan;
		
		void initStats();
		bool containsAmbResidues(const std::string& ambResidues, std::string fragSeq) const;
		void removeBelowIntensity(double intensity);
        void removeBelowIntensity(IonType ionType, double intensity);
		void calcContainsCit(bool includeCTermMod);
		void addMod(std::string mod);
	
	public:		
		PeptideStats(){
			_scan = new Dtafilter::Scan;
			initStats();
			_fragDelim = FRAG_DELIM;
            containsCit = ContainsCitType::FALSE;
            thisContainsCit = ContainsCitType::FALSE;
            modIndex = std::string::npos;

			//peptide data
			sequence = "";
			fullSequence = "";
			charge = 0;
			mass = 0;
			_id = -1;
		}
		explicit PeptideStats(const PeptideNamespace::Peptide& p){
			//PeptideStats data
			_scan = new Dtafilter::Scan;
			_fragDelim = FRAG_DELIM;
            containsCit = ContainsCitType::FALSE;
            thisContainsCit = ContainsCitType::FALSE;
            modIndex = std::string::npos;
			initStats();

			//Peptide data
			sequence = p.getSequence();
			fullSequence = scanData::removeStaticMod(p.getFullSequence(), false);
			charge = p.getCharge();
			mass = p.getMass();
			modLocs.clear();
			modLocs.insert(modLocs.begin(), p.getModLocs().begin(), p.getModLocs().end());
			modIndex = std::string::npos;
			_id = p.getID();
		}
		PeptideStats(const PeptideStats&);

		~PeptideStats() = default;

		//properties
		bool canConsolidate(const PeptideStats& rhs) const {
			return _id == rhs._id;
		}
		double totalFragmentIntensity() const;
        double totalFragmentIntensity(double min, double max = std::numeric_limits<double>::max()) const;
		double fragmentIntensity(IonType) const;
        double fragmentIntensity(IonType, double min, double max = std::numeric_limits<double>::max()) const;
        double calcIntCO(double fractionArtifact) const;
        void printFragmentStats(std::ostream& out) const;

		//modifiers
		PeptideStats& operator = (const PeptideStats&);
		void addSeq(const PeptideNamespace::FragmentIon&, size_t modLoc, const std::string&);
		static std::string ionTypeToStr(const IonType&);
		static std::string containsCitToStr(const ContainsCitType&);
		void consolidate(const PeptideStats&);
	};
	
	inline PeptideStats::IonType operator++(PeptideStats::IonType& x ){
		return x = (PeptideStats::IonType)(((int)(x) + 1));
	}
}

#endif /* datProc_hpp */
