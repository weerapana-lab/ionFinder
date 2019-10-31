//
//  datProc.cpp
//  ionFinder
//
//  Created by Aaron Maurais on 12/10/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#include <ionFinder/datProc.hpp>

//!Copy constructor
IonFinder::PeptideStats::PeptideStats(const IonFinder::PeptideStats& rhs) {
    _fragDelim = rhs._fragDelim;
    ionTypesCount = rhs.ionTypesCount;
    containsCit = rhs.containsCit;
    sequence = rhs.sequence;
    modLocs = rhs.modLocs;
    modResidues = rhs.modResidues;
    _id = rhs._id;
    charge = rhs.charge;
    fullSequence = rhs.fullSequence;
    mass = rhs.mass;
    _scan = new Dtafilter::Scan;
    _scan = rhs._scan;
}

//!Copy assignment
IonFinder::PeptideStats& IonFinder::PeptideStats::operator=(const IonFinder::PeptideStats& rhs)
{
    _fragDelim = rhs._fragDelim;
    ionTypesCount = rhs.ionTypesCount;
    containsCit = rhs.containsCit;
    sequence = rhs.sequence;
    modLocs = rhs.modLocs;
    modResidues = rhs.modResidues;
    _id = rhs._id;
    charge = rhs.charge;
    fullSequence = rhs.fullSequence;
    mass = rhs.mass;
    _scan = rhs._scan;

    return *this;
}

void IonFinder::PeptideStats::consolidate(const PeptideStats& rhs)
{
    if(_id != rhs._id)
        throw std::runtime_error("Can not combine PeptideStats! IDs do not match.");

    //combined contains cit
    containsCit = std::min(containsCit, rhs.containsCit);
    addMod(rhs.modResidues);

    //combine ionTypesCount
    for(PeptideStats::IonType it = PeptideStats::IonType::First;
        it != PeptideStats::IonType::Last;
        ++it)
    {
        //utils::addChar(rhs.ionTypesCount.at(it).first, ionTypesCount.at(it).first, _fragDelim);
        ionTypesCount.at(it).insert(rhs.ionTypesCount.at(it).begin(),
                                          rhs.ionTypesCount.at(it).end());
//        ionTypesCount.at(it).second += rhs.ionTypesCount.at(it).second;
    }
}


void IonFinder::PeptideStats::initStats()
{
	for(IonType i = IonType::First; i != IonType::Last; ++i){
		ionTypesCount[i] = IonStrings();
	}

	containsCit = ContainsCitType::FALSE;
}

/**
 Add modified residue to PeptideStats::modResidues
 \param mod Modified residue in the form <residue><number> as in C57
 */
void IonFinder::PeptideStats::addMod(std::string mod){
	utils::addChar(mod, modResidues, _fragDelim);
}

/**
 Tests whether sequence contains ambiguous residues.
 \param ambResidues ambiguous residues to search for
 \param fragSeq sequence of fragment to search for ambiguous residues.
 \return True if an ambiguous residue is found.
*/
bool IonFinder::PeptideStats::containsAmbResidues(const std::string& ambResidues,
												  std::string fragSeq) const
{
	size_t len = fragSeq.length();
	for(size_t i = 0; i < len; i++)
		for(char ambResidue : ambResidues)
			if(fragSeq[i] == ambResidue)
				return true;
	return false;
}

/**
 Add fragment sequence to PeptideStats.
 \pre \p seq._sequence is in *this->sequence
 \param seq fragment ion to add
 \param modLoc Location of modification to add for.
 \param ambResidues ambiguous residues to search for.
 */
void IonFinder::PeptideStats::addSeq(const PeptideNamespace::FragmentIon& seq,
                                     unsigned long modLoc,
									 const std::string& ambResidues)
{
	//first check that seq is found in *this sequence
	assert(utils::strContains(seq.getSequence(), sequence));
	
	//increment total fragment ions found
	std::string ionStr = seq.getLabel(true);
	ionTypesCount[IonType::FRAG].insert(ionStr);
	
	//check if seq spans any modified residues
	//for(unsigned long & modLoc : modLocs)
	//{
		//check if in span
		if(utils::inSpan(seq.getBegin(), seq.getEnd(), modLoc))
		{
			//check if NL
			if(seq.isNL()){
				//check multiple of neutral loss
				if(seq.getNumNl() == seq.getNumMod()){ //if equal to number of modifications, determining NL
					ionTypesCount[IonType::DET_NL_FRAG].insert(ionStr);
				}
				else{ //if not equal, artifiact fragment
					//std::cout << seq.getLabel() << NEW_LINE;
					ionTypesCount[IonType::AMB_NL_FRAG].insert(ionStr);
				}
			}
			else
			{
				if(containsAmbResidues(ambResidues, seq.getSequence())){ //is ambModFrag
					ionTypesCount[IonType::AMB_FRAG].insert(ionStr);
				}
				else{ //is detFrag
					ionTypesCount[IonType::DET_FRAG].insert(ionStr);
				}
			}
		}
		else{
			if(seq.isNL()){ //is artifact NL frag
				ionTypesCount[IonType::ART_NL_FRAG].insert(ionStr);
			}
			else{ //is amg frag
				ionTypesCount[IonType::AMB_FRAG].insert(ionStr);
			}//end of else
		}//end of else
	//}//end of for
}//end of fxn

/**
 
 \param peptides vector of peptides to analyze
 */
bool IonFinder::analyzeSequences(std::vector<Dtafilter::Scan>& scans,
								 const std::vector<PeptideNamespace::Peptide>& peptides,
								 std::vector<PeptideStats>& peptideStats,
								 const IonFinder::Params& pars)
{
	bool allSucess = true;
	bool addModResidues = !pars.getFastaFile().empty();
	utils::FastaFile seqFile;
	int nSeqNotFound = 0;
	if(addModResidues){
		std::cout << "\nReading FASTA file...";
		if(!seqFile.read(pars.getFastaFile())) return false;
		std::cout << "Done!" << NEW_LINE;
	}

	for(auto it = peptides.begin(); it != peptides.end(); ++it)
	{
		std::vector<IonFinder::PeptideStats> this_stats;
	    for(auto mod_it = it->getModLocs().begin(); mod_it != it->getModLocs().end(); ++mod_it)
		{
            //initialize new pepStat object
            //IonFinder::PeptideStats pepStat(*it); //init pepStat
            this_stats.emplace_back(*it);
            this_stats.back()._scan = &scans[it - peptides.begin()]; //add pointer to scan
            size_t nFragments = it->getNumFragments();

            //iterate through ion fragmetns
            for (size_t i = 0; i < nFragments; i++) {
                //skip if not found
                if (it->getFragment(i).getFound()) {
                    this_stats.back().addSeq(it->getFragment(i), *mod_it, pars.getAmbigiousResidues());
                } //end of if
            }//end of for i
            this_stats.back().calcContainsCit();

            if (addModResidues) {
                    bool found; //set to true if peptide and prot sequences are found in FastaFile
                    std::string modTemp = seqFile.getModifiedResidue(this_stats.back()._scan->getParentID(),
                                                                     this_stats.back().sequence, int(*mod_it),
                                                                     pars.getVerbose(), found);
                    this_stats.back().addMod(modTemp);
                    if (!found)
                        nSeqNotFound++;
            }
        }//end for mod_it

        assert(pars.getGroupMod() == 0 || pars.getGroupMod() == 1);
        if(pars.getGroupMod() == 0) {

            PeptideStats::ContainsCitType cc = PeptideStats::ContainsCitType::TRUE;
            for (const auto &s:this_stats) {
                cc = std::min(cc, s.containsCit);
            }

            for(auto & this_stat : this_stats) {
                this_stat.containsCit = cc;
                peptideStats.push_back(this_stat);
            }
        }
        else {
            for(auto s = this_stats.begin(); s != this_stats.end(); ++s){
                if(s == this_stats.begin())
                    peptideStats.push_back(*s);
                else peptideStats.back().consolidate(*s);
            }
        }
	    
	}//end if for it
	if(nSeqNotFound > 0){
		std::cerr << NEW_LINE << nSeqNotFound << " protein sequences not found in " <<
		pars.getFastaFile() << NEW_LINE;
	}

	return allSucess;
}//end of fxn

/**
 Search parent ms2 files in \p scans for predicted fragment ions. <br><br>
 Analysis is performed in parallel in number of threads in Params::_numThread. <br>
 \p scans is split up evenly across each thread.
 
 \param scans populated list of identified ms2 scans to search for
 \param peptides empty list of peptides to annotate
 \param pars Params object for information on how to perform analysis
 \return true is all file I/O was successful.
 */
bool IonFinder::findFragmentsParallel(std::vector<Dtafilter::Scan>& scans,
									  std::vector<PeptideNamespace::Peptide>& peptides,
									  const IonFinder::Params& pars)
{
	unsigned int const nThread = pars.getNumThreads();
	size_t const nScans = scans.size();
	size_t peptidePerThread = nScans / nThread;
	if(nScans % nThread != 0)
		peptidePerThread += 1;
	std::atomic<size_t> scansIndex(0); //used to update progress for findFragmentsProgress
	
	if(nScans == 0){
		std::cout << "No scans in input!\n";
		return false;
	}
	
	//init threads
	std::vector<std::thread> threads;
	bool* sucsses = new bool[nThread];
	
	//read ms2s
	std::cout << "Reading parent ms2 files...";
	Ms2Map ms2Map;
	if(!IonFinder::readMs2s(ms2Map, scans)) return false;
	std::cout << "Done!\n";
	
	//split up input data for each thread
	auto* splitPeptides = new std::vector<PeptideNamespace::Peptide>[nThread];
	size_t begNum, endNum ;
	unsigned int threadIndex = 0;
	for(size_t i = 0; i < nScans; i += peptidePerThread)
	{
		begNum = i;
		endNum = (begNum + peptidePerThread > nScans ? nScans : begNum + peptidePerThread);

		//spawn thread
		assert(threadIndex < nThread);
		splitPeptides[threadIndex] = std::vector<PeptideNamespace::Peptide>();
		threads.emplace_back(IonFinder::findFragments_threadSafe, std::ref(scans), begNum, endNum,
									  ms2Map,
									  std::ref(splitPeptides[threadIndex]), std::ref(pars),
									  sucsses + threadIndex, std::ref(scansIndex));
		threadIndex++;
	}
	
	//spawn progress function
	if(!pars.getVerbose())
		threads.emplace_back(IonFinder::findFragmentsProgress, std::ref(scansIndex), nScans,
									  nThread, PROGRESS_SLEEP_TIME);
	
	//join threads
	for(auto & thread : threads){
		thread.join();
	 }
	
	//concat split peptides into one vector
	peptides.clear();
	for(unsigned int i = 0; i < nThread; i++){
		if(!sucsses[i])
			return false;
		peptides.insert(peptides.end(), splitPeptides[i].begin(), splitPeptides[i].end());
	}
	
	delete [] splitPeptides;
	delete [] sucsses;
	return true;
}

/**
 Prints progress bar during findFragmentsParallel. <br>
 \p scansIndex is updated concurrently by each thread.
 \param scansIndex number of scans completed
 \param count total number of scans to search for
 \param sleepTime time before next update is printed (in seconds)
 */
void IonFinder::findFragmentsProgress(std::atomic<size_t>& scansIndex, size_t count,
									  unsigned int nThread, int sleepTime)
{
	size_t lastIndex = scansIndex.load();
	size_t curIndex = lastIndex;
	int noChangeIterations = 0;
	
	std::cout << "\nSearching ms2s for fragment ions using " << nThread << " thread(s)...\n";
	while(scansIndex < count)
	{
		curIndex = scansIndex.load();
		
		if(lastIndex == curIndex)
			noChangeIterations++;
		else noChangeIterations = 0;
		
		if(noChangeIterations > IonFinder::MAX_PROGRESS_ITTERATIONS)
			return;
		
		utils::printProgress(float(curIndex) / float(count));
		std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
		
		lastIndex = curIndex;
	}
	utils::printProgress(float(scansIndex.load()) / float(count));
	std::cout << NEW_LINE;
	std::cout << "Done!" << NEW_LINE;
}

/**
 Find peptide fragment ions in ms2 files.
 \param scans Populated vector of scan objects to search for
 \param peptides empty vector of peptides to be filled from data in scans.
 \param pars IonFinder params object.
 \return true if successful
 */
bool IonFinder::findFragments(std::vector<Dtafilter::Scan>& scans,
							  std::vector<PeptideNamespace::Peptide>& peptides,
							  IonFinder::Params& pars)
{
	bool* success = new bool(false);
	std::atomic<size_t> scansIndex;
	IonFinder::Ms2Map ms2Map;
	if(!readMs2s(ms2Map, scans)) return false;
	IonFinder::findFragments_threadSafe(scans, 0, scans.size(), ms2Map, peptides, pars,
										success, scansIndex);
	
	bool ret = *success;
	delete success;
	return ret;
}

bool IonFinder::readMs2s(IonFinder::Ms2Map& ms2Map,
						 const std::vector<Dtafilter::Scan>& scans)
{
	std::string curWD;
	
	//first get unique names of ms2 files to read
	size_t len = scans.size();
	std::vector<std::string> fileNamesList;
	for(size_t i = 0; i < len; i++){
		if(std::find(fileNamesList.begin(),
					 fileNamesList.end(),
					 scans[i].getPrecursor().getFile()) == fileNamesList.end()){
			fileNamesList.push_back(scans[i].getPrecursor().getFile());
		}
	}
		
	//read ms2 files
	ms2Map.clear();
	for(auto & it : fileNamesList)
	{
		ms2Map[it] = ms2::Ms2File();
		if(!ms2Map[it].read(it)){
			std::cerr << "Error reading ms2 files!" << NEW_LINE;
			return false;
		}
	}
	return true;
}

/**
 Find peptide fragment ions in ms2 files. <br>
 Function should not be called directly.
 Use IonFinder::findFragments or IonFinder::findFragmentsParallel instead.
 \param peptides empty vector of peptides to be filled from data in scans.
 \param beg index of beginning of scan vector
 \param end index of end of scan vector
 \param pars IonFinder params object.
 \param success set to true if function was successful
 */
void IonFinder::findFragments_threadSafe(std::vector<Dtafilter::Scan>& scans,
										 const size_t beg, const size_t end,
										 const IonFinder::Ms2Map& ms2Map,
										 std::vector<PeptideNamespace::Peptide>& peptides,
										 const IonFinder::Params& pars,
										 bool* success, std::atomic<size_t>& scansIndex)
{
	*success = false;
	std::string curSample;
	std::string curWD;
	std::string spFname;
	aaDB::AADB aminoAcidMasses;
	ms2::Spectrum spectrum;
	
	for(size_t i = beg; i < end; i++)
	{
		//first get current wd name
		curWD = utils::dirName(scans[i].getPrecursor().getFile());
		spFname = curWD + "/sequest.params";
		
		if(curSample != scans[i].getSampleName())
		{
			//re-init Peptide::AminoAcidMasses for each sample
			curWD = utils::dirName(scans[i].getPrecursor().getFile());
			spFname = curWD + "/sequest.params";
			
			//read sequest params file and init aadb
			PeptideNamespace::initAminoAcidsMasses(pars, spFname, aminoAcidMasses);
		}//end if
		curSample = scans[i].getSampleName();
		
		//initialize peptide object for current scan
		peptides.emplace_back(scans[i].getSequence());
		peptides.back().initialize(pars, aminoAcidMasses);
		
		//add neutral loss fragments to current peptide
		if(pars.getCalcNL()){
			peptides.back().addNeutralLoss(pars.getNeutralLossMass());
		}
		
		//load spectrum
		auto ms2FileIt = ms2Map.find(scans[i].getPrecursor().getFile());
		if(ms2FileIt == ms2Map.end()){
			throw std::out_of_range("\nKey error in Ms2Map!");
		}
		if(!ms2FileIt->second.getScan(scans[i].getScanNum(), spectrum)){
			throw std::runtime_error("\nError reading scan!");
			return;
		}
		scans[i].setPrecursor(spectrum.getPrecursor());

		spectrum.labelSpectrum(peptides.back(), pars, true); //removes unlabeled ions from peptide
        //spectrum.labelSpectrum(peptides.back(), pars);
        peptides.back().normalizeLabelIntensity(spectrum.getMaxIntensity() / 100);

        //probably temporary
        peptides.back().removeLabelIntensityBelow(pars.getNlIntCo(), true, true);

		//print spectra file
		if(pars.getPrintSpectraFiles())
		{
			std::string dirNameTemp = (pars.getInDirSpecified() ? pars.getWD() : curWD) + "/spectraFiles";
			if(!utils::dirExists(dirNameTemp))
				if(!utils::mkdir(dirNameTemp.c_str(), "-p")){
					throw std::runtime_error("\nFailed to make dir: " + dirNameTemp);
				}

			spectrum.normalizeIonInts(100);
			spectrum.calcLabelPos();

			std::string temp = dirNameTemp + "/" + utils::baseName(scans[i].getOfname());
			std::ofstream outF((temp).c_str());
			if(!outF){
				throw std::runtime_error("\nFailed to write spectrum!");
			}
			spectrum.printLabeledSpectrum(outF, true);
		}
		scansIndex++;
	} //end of for
	
	*success = true;
}

void IonFinder::PeptideStats::calcContainsCit()
{
	containsCit = ContainsCitType::FALSE;
	
	//is the peptide modified?
	if(modLocs.empty()) return;
	
	//Is cit modification on the c terminus?
	//modLocs.back() works because modLocs are added in the order
	//they appear in the sequence
	if(modLocs.back() == sequence.length() - 1) return;
	
	//is there more than 1 determining NLs?
	if(ionTypesCount[IonType::DET_NL_FRAG].size() > 1){
		containsCit = ContainsCitType::TRUE;
		return;
	}
	
	//are there 1 or more determining NLs or determining frags?
	if(ionTypesCount[IonType::DET_NL_FRAG].size() >= 1 ||
	   ionTypesCount[IonType::DET_FRAG].size() >= 1){
		containsCit = ContainsCitType::LIKELY;
		return;
	}
	
	//are there 1 more ambiguous fragments?
	if(ionTypesCount[IonType::AMB_FRAG].size() >= 1 ||
	    ionTypesCount[IonType::AMB_NL_FRAG].size()>= 1){
		containsCit = ContainsCitType::AMBIGUOUS;
		return;
	}
}

/**
 \brief Convert \p it into a string representation. <br>
 IonFinder::ION_TYPES_STR contains string representations used.
 \param it IonType to convert.
 \return string representation of \p it.
 */
std::string IonFinder::PeptideStats::ionTypeToStr(const IonType& it)
{
	switch(it){
		case IonType::FRAG: return ION_TYPES_STR[0];
			break;
		case IonType::DET_FRAG: return ION_TYPES_STR[1];
			break;
		case IonType::AMB_FRAG: return ION_TYPES_STR[2];
			break;
        case IonType::DET_NL_FRAG: return ION_TYPES_STR[3];
            break;
        case IonType::AMB_NL_FRAG: return ION_TYPES_STR[4];
            break;
		case IonType::ART_NL_FRAG: return ION_TYPES_STR[5];
			break;
		case IonType::Last: return "Last";
			break;
	}
}

std::string IonFinder::PeptideStats::containsCitToStr(const ContainsCitType& cc)
{
    switch(cc) {
        case ContainsCitType::FALSE: return CONTAINS_CIT_STR[0];
            break;
        case ContainsCitType::AMBIGUOUS: return CONTAINS_CIT_STR[1];
            break;
        case ContainsCitType::LIKELY: return CONTAINS_CIT_STR[2];
            break;
        case ContainsCitType::TRUE: return CONTAINS_CIT_STR[3];
            break;
    }
}

/**
 Prints peptide stats to out.
 \param stats Peptide stats to print.
 \param pars initialized IonFinder::Params object
 \return true if successful.
 */
bool IonFinder::printPeptideStats(const std::vector<PeptideStats>& stats,
								  const IonFinder::Params& pars)
{
	//assert(out);
	std::ofstream outF (pars.makeOfname());
	if(!outF) return false;
	
	typedef IonFinder::PeptideStats::IonType itcType;
	//build stat names vector
	std::vector<std::string> statNames;
	
	if(pars.getCalcNL())
        statNames.emplace_back("contains_Cit");
	else statNames.emplace_back("contains_mod");
	
	//determine when to stop printing peptide stats based on analysis performed
	std::vector<itcType> _pepStats; //used to store relevant peptide stats based on params
	//defaults
	_pepStats.push_back(itcType::FRAG);
	_pepStats.push_back(itcType::DET_FRAG);
	_pepStats.push_back(itcType::AMB_FRAG);
	//conditional stats
	if(pars.getCalcNL()){
		_pepStats.push_back(itcType::DET_NL_FRAG);
		_pepStats.push_back(itcType::AMB_NL_FRAG);
		_pepStats.push_back(itcType::ART_NL_FRAG);
	}
	
	//append peptide stats names to headers
	int statLen = 0;
	for(auto & _pepStat : _pepStats){
		statNames.push_back("n" +
							std::string(1, (char)std::toupper(ION_TYPES_STR[utils::as_integer(_pepStat)][0])) +
							ION_TYPES_STR[utils::as_integer(_pepStat)].substr(1));
		statLen++;
	}
	statNames.insert(statNames.end(), ION_TYPES_STR, ION_TYPES_STR + statLen);
	
	std::string otherHeaders = "peptide_unique_ID protein_ID parent_protein protein_description full_sequence sequence parent_mz is_modified modified_residue charge unique xCorr spectral_counts scan precursor_scan precursor_rt parent_file sample_name";
	std::vector<std::string> oHeaders;
	utils::split(otherHeaders, ' ', oHeaders);
	std::vector<std::string> headers;
	headers.insert(headers.end(), oHeaders.begin(), oHeaders.end());
	headers.insert(headers.end(), statNames.begin(), statNames.end());
	
	//print headers
	size_t len = headers.size();
	for(size_t i = 0; i < len; i++){
		if(i == 0)
			outF << headers[i];
		else outF << OUT_DELIM << headers[i];
	}
	outF << NEW_LINE;
	
	//print data
	for(const auto & stat : stats)
	{
		//scan data
		outF << stat._id <<
		    OUT_DELIM << stat._scan->getParentID() <<
		    OUT_DELIM << stat._scan->getParentProtein() <<
		    OUT_DELIM << stat._scan->getParentDescription() <<
		    OUT_DELIM << stat._scan->getFullSequence() <<
		    OUT_DELIM << scanData::removeStaticMod(stat._scan->getSequence()) <<
		    OUT_DELIM << stat._scan->getPrecursor().getMZ() <<
		    OUT_DELIM << (!stat.modLocs.empty()) <<
		    OUT_DELIM << stat.modResidues <<
		    OUT_DELIM << stat._scan->getPrecursor().getCharge() <<
		    OUT_DELIM << stat._scan->getUnique() <<
		    OUT_DELIM << stat._scan->getXcorr() <<
		    OUT_DELIM << stat._scan->getSpectralCounts() <<
		    OUT_DELIM << stat._scan->getScanNum() <<
		    OUT_DELIM << stat._scan->getPrecursor().getScan() <<
		    OUT_DELIM << stat._scan->getPrecursor().getRT() <<
		    OUT_DELIM << utils::baseName(stat._scan->getPrecursor().getFile()) <<
		    OUT_DELIM << stat._scan->getSampleName();
		
		//peptide analysis data
		outF << OUT_DELIM;
		if(pars.getCalcNL())
			 outF << PeptideStats::containsCitToStr(stat.containsCit);
		else{
			outF << (stat.ionTypesCount.at(itcType::DET_FRAG).size() > 0);
		}
		
		for(auto & _pepStat : _pepStats)
			outF << OUT_DELIM << stat.ionTypesCount.at(_pepStat).size();

		for(auto & _pepStat : _pepStats){
		    outF << OUT_DELIM;
            for(auto it = stat.ionTypesCount.at(_pepStat).begin();
                it != stat.ionTypesCount.at(_pepStat).end();
                ++it)
            {
                if(it == stat.ionTypesCount.at(_pepStat).begin())
                    outF << *it;
                else outF << stat._fragDelim << *it;
            }
        }
		outF << NEW_LINE;
	}
	return true;
}

