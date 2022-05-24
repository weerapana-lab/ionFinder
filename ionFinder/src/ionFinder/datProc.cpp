//
// datProc.cpp
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

#include <ionFinder/datProc.hpp>

//!Copy constructor
IonFinder::PeptideStats::PeptideStats(const IonFinder::PeptideStats& rhs) {
    _fragDelim = rhs._fragDelim;
    ionTypesCount = rhs.ionTypesCount;
    containsCit = rhs.containsCit;
    thisContainsCit = rhs.thisContainsCit;
    sequence = rhs.sequence;
    modLocs = rhs.modLocs;
    modIndex = rhs.modIndex;
    modResidues = rhs.modResidues;
    _id = rhs._id;
    charge = rhs.charge;
    fullSequence = rhs.fullSequence;
    mass = rhs.mass;
    _scan = new Dtafilter::Scan;
    _scan = rhs._scan;
}

//!Copy assignment
IonFinder::PeptideStats& IonFinder::PeptideStats::operator = (const IonFinder::PeptideStats& rhs) = default;

void IonFinder::PeptideStats::consolidate(const PeptideStats& rhs)
{
    if(_id != rhs._id)
        throw std::runtime_error("Can not combine PeptideStats! IDs do not match.");

    //combined contains cit
    containsCit = std::min(thisContainsCit, rhs.thisContainsCit);
    addMod(rhs.modResidues);

    //combine ionTypesCount
    for(auto it = PeptideStats::IonType::First; it != PeptideStats::IonType::Last; ++it) {
        ionTypesCount.at(it).insert(rhs.ionTypesCount.at(it).begin(), rhs.ionTypesCount.at(it).end());
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
 * Get total intensity for ions of type \p ionType.
 * \param ionType Type.
 * \return Sum of intensities for all ions classified as \p ionType.
 */
double IonFinder::PeptideStats::fragmentIntensity(IonType ionType) const
{
    double sum = 0.0;
    auto _ionType = ionTypesCount.find(ionType);
    if(_ionType != ionTypesCount.end())
        for(auto it = _ionType->second.begin(); it != _ionType->second.end(); ++it)
            sum += it->getIntensity();
    return sum;
}

/**
 * Get total intensities for all classified fragment ions.
 * \return Sum of intensities.
 */
double IonFinder::PeptideStats::totalFragmentIntensity() const {
    return totalFragmentIntensity(0);
}

/**
 * Get total intensity for ions of type \p ionType.
 * \param ionType Type.
 * \param min Minimum (non inclusive) intensity to consider.
 * \param max Maximum (inclusive) intensity to consider.
 * \return Sum of intensities for all ions classified as \p ionType.
 */
double IonFinder::PeptideStats::fragmentIntensity(IonType ionType, double min, double max) const
{
    double sum = 0.0;
    auto _ionType = ionTypesCount.find(ionType);
    if(_ionType != ionTypesCount.end())
        for(auto it = _ionType->second.begin(); it != _ionType->second.end(); ++it)
            if(it->getIntensity() > min && it->getIntensity() <= max)
                sum += it->getIntensity();
    return sum;
}

/**
 * Get total intensities for all classified fragment ions.
 * \param min Minimum (non inclusive) intensity to consider.
 * \param max Maximum (inclusive) intensity to consider.
 * \return Sum of intensities.
 */
double IonFinder::PeptideStats::totalFragmentIntensity(double min, double max) const
{
    double sum = 0.0;
    for(auto it = IonType::First; it != IonType::Last; ++it)
        if(it != IonType::FRAG)
            sum += fragmentIntensity(it, min, max);
    return sum;
}

/**
 Add fragment sequence to PeptideStats.
 \pre \p seq._sequence is in *this->sequence
 \param seq fragment ion to add
 \param modLoc Location of modification to add for.
 \param ambResidues ambiguous residues to search for.
 */
void IonFinder::PeptideStats::addSeq(const PeptideNamespace::FragmentIon& seq,
                                     size_t modLoc, const std::string& ambResidues)
{
	//first check that seq is found in *this sequence
	assert(utils::strContains(seq.getSequence(), sequence));
	
	//increment total fragment ions found
	IonFinder::FragmentIon ionStr = IonFinder::FragmentIon(seq.getLabel(true), seq.getFoundIntensity());
	ionTypesCount[IonType::FRAG].insert(ionStr);
	
    //check if in span
    if(utils::inSpan(seq.getBegin(), seq.getEnd(), modLoc))
    {
        //check if NL
        if(seq.isNL()){
            //check multiple of neutral loss
            if(seq.getNumNl() == seq.getNumMod()){ //if equal to number of modifications, determining NL
                ionTypesCount[IonType::DET_NL].insert(ionStr);
            }
            else{ //if not equal, ambiguous NL fragment
                //std::cout << seq.getLabel() << NEW_LINE;
                ionTypesCount[IonType::AMB].insert(ionStr);
            }
        }
        else{
            if(containsAmbResidues(ambResidues, seq.getSequence())){ //is ambModFrag
                ionTypesCount[IonType::AMB].insert(ionStr);
            }
            else{ //is detFrag
                ionTypesCount[IonType::DET].insert(ionStr);
            }
        }
    }
    else{
        if(seq.isNL()){ //is artifact NL frag
            if(seq.isModified() && (seq.getNumNl() <= seq.getNumMod()))
                ionTypesCount[IonType::AMB].insert(ionStr);
            else ionTypesCount[IonType::ART_NL].insert(ionStr);
        }
        else{ //is amg frag
            ionTypesCount[IonType::AMB].insert(ionStr);
        }//end of else
    }//end of else
}//end of fxn

/**
 * Remove all ions from ionTypesCount below \p intensity.
 * \param intensity
 */
void IonFinder::PeptideStats::removeBelowIntensity(double intensity)
{
    for(auto ionType = ionTypesCount.begin(); ionType != ionTypesCount.end(); ++ionType) {
        removeBelowIntensity(ionType->first, intensity);
    } //end for ionType
}

//! Remove ions of type \p ionType from ionTypesCount which are <= \p intensity.
void IonFinder::PeptideStats::removeBelowIntensity(IonFinder::PeptideStats::IonType ionType, double intensity)
{
    auto _ionType = ionTypesCount.find(ionType);
    if(_ionType != ionTypesCount.end())
        for(auto it = _ionType->second.begin(); it != _ionType->second.end();) {
            if(it->getIntensity() <= intensity || utils::almostEqual(it->getIntensity(), intensity))
                it = _ionType->second.erase(it);
            else ++it;
        } //end for it
}

void IonFinder::PeptideStats::printFragmentStats(std::ostream& out) const
{
    out << fullSequence << OUT_DELIM
        << _scan->getPrecursor().getCharge() << OUT_DELIM
        << _scan->getScanNum() << OUT_DELIM;

    for(auto it = IonType::First; it != IonType::Last; ++it) {
        if(it == IonType::First)
            out << fragmentIntensity(it);
        else out << OUT_DELIM << fragmentIntensity(it);
    }
    out << NEW_LINE;
}

/**
 * Calculate intensity cutoff to achieve a less than \p fractionArtifact of
 * total ion intensity from artifact neutral loss labeledIons.
 * \param fractionArtifact Fraction of ion intensity which should be from artifact labeledIons.
 * \return Intensity cutoff.
 */
double IonFinder::PeptideStats::calcIntCO(double fractionArtifact) const
{
    //if(ionTypesCount.at(IonType::ART_NL).size() > 1)
    //    std::cout << "Found!\n";

    std::vector<double> art_ints;
    art_ints.push_back(0);
    for(auto it : ionTypesCount.at(IonType::ART_NL))
        art_ints.push_back(it.getIntensity());
    std::sort(art_ints.begin(), art_ints.end());

    double current_fractionArtifact;
    double numerator, denominator;
    for(auto cutoff : art_ints)
    {
        //first calculate the fraction of ion intensity which comes from artifact ions
        numerator = fragmentIntensity(IonType::ART_NL, cutoff);
        denominator = totalFragmentIntensity(cutoff);
        current_fractionArtifact = numerator / denominator;

        if(std::isnan(current_fractionArtifact) ||
           current_fractionArtifact <= fractionArtifact)
            return cutoff;
    }
    std::cerr << "WARN: Returning maximum cutoff intensity!" << NEW_LINE;
    return std::numeric_limits<double>::max();
}

/**
 * Analyze the fragment ions found in the context of the peptide sequence to determine
 * whether the peptide is likely to be modified.
 *
 * \param scans Populated vector of scans.
 * \param peptides Populated vector of peptides.
 * \param peptideStats Empty vector of peptideStats.
 * \param pars Populated Params object.
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

		std::vector<size_t> modLocsTemp;
		if(it->isModified())
			modLocsTemp = it->getModLocs();
		else modLocsTemp.push_back(std::string::npos);

        for(auto mod_it = modLocsTemp.begin(); mod_it != modLocsTemp.end(); ++mod_it)
		{
            // initialize new pepStat object
            this_stats.emplace_back(*it);
            // this_stats.push_back(IonFinder::PeptideStats(
            this_stats.back()._scan = &scans[it - peptides.begin()]; //add pointer to scan
            size_t nFragments = it->getNumFragments();
            this_stats.back().modIndex = *mod_it;

            // iterate through ion fragments
            for (size_t i = 0; i < nFragments; i++) {
                //skip if not found
                if (it->getFragment(i).getFound()) {
                    this_stats.back().addSeq(it->getFragment(i), *mod_it, pars.getAmbigiousResidues());
                } //end of if
            }//end of for i

            // if(it->getFullSequence() == "LEYQWTNNIGDAHTIGTR*PDNGMLSLGVSYR")
            // if(this_stats.back()._scan->getScanNum() == 23248)
            //     std::cout << "Found" << std::endl;

            // Filter to remove Artifact ions
            double int_co = this_stats.back().calcIntCO(pars.getArtifactNLIntFrac());
            this_stats.back().removeBelowIntensity(int_co);

            this_stats.back().calcContainsCit(pars.getIncludeCTermMod());

            if(addModResidues && *mod_it != std::string::npos) {
                bool found; //set to true if peptide and protein sequences are found in FastaFile
                std::string modTemp = seqFile.getModifiedResidue(this_stats.back()._scan->getParentID(),
                                                                 this_stats.back().sequence, int(*mod_it),
                                                                 pars.getVerbose(), found);
                this_stats.back().addMod(modTemp);
                if (!found)
                    nSeqNotFound++;
            }
        }//end for mod_it

        assert(pars.getGroupMod() == 0 || pars.getGroupMod() == 1);
        if(pars.getGroupMod() == 0)
        {
            PeptideStats::ContainsCitType cc = PeptideStats::ContainsCitType::TRUE;
            for (const auto &s:this_stats) {
                cc = std::min(cc, s.thisContainsCit);
            }

            for(auto & this_stat : this_stats) {
                this_stat.containsCit = cc;
                peptideStats.push_back(this_stat);
            }
        }
        else {
            for(auto s = this_stats.begin(); s != this_stats.end(); ++s){
                if(s == this_stats.begin()) {
                    peptideStats.push_back(*s);
                    peptideStats.back().containsCit = s->thisContainsCit;
                }
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
 * Print total intensities for each fragment to \p fname. (Mainly for debugging)
 * \param stats Populated list of PeptideStats.
 * \param fname File path.
 * \param id Optional id for last column.
 * \return true if all file I/O was successful.
 */
bool IonFinder::printFragmentIntensities(const std::vector<PeptideStats>& stats,
                                         std::string fname, std::string id)
{
	std::ofstream outF(fname);
    if(!outF) return false;

    outF << "sequence\tcharge\tscan";
    for(auto it = PeptideStats::IonType::First; it != PeptideStats::IonType::Last; ++it)
        outF << OUT_DELIM << PeptideStats::ionTypeToStr(it) << "_intensity";
    if(!id.empty())
        outF << OUT_DELIM << "id";
    outF << NEW_LINE;

    for(auto it = stats.begin(); it != stats.end(); ++it)
        it->printFragmentStats(outF);

    return true;
}

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

    // read ms files
    ms2::MsInterface msInterface;
    // msInterface.read(scans.begin(), scans.end());

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
		// threads.emplace_back(IonFinder::findFragments_, std::ref(scans), begNum, endNum,
		//						  std::ref(splitPeptides[threadIndex]), std::ref(pars),
		//						  sucsses + threadIndex, std::ref(scansIndex));
		threads.emplace_back(IonFinder::findFragments_threadSafe, std::ref(scans), begNum, endNum,
									  std::ref(msInterface),
									  std::ref(splitPeptides[threadIndex]), std::ref(pars),
									  sucsses + threadIndex, std::ref(scansIndex));
		threadIndex++;
	}

	//spawn progress function
    std::string progress_messge = "\nSearching ms2s for fragment ions using " + std::to_string(nThread) + " thread(s)...";
	if(!pars.getVerbose())
		threads.emplace_back(IonFinder::findFragmentsProgress, std::ref(scansIndex), nScans,
									  std::ref(progress_messge), PROGRESS_SLEEP_TIME);

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
									  const std::string& message, int sleepTime)
{
	size_t lastIndex = scansIndex.load();
	size_t curIndex = lastIndex;
	int noChangeIterations = 0;

	std::cout << message << NEW_LINE;
	while(scansIndex < count)
	{
		curIndex = scansIndex.load();

		if(lastIndex == curIndex)
			noChangeIterations++;
		else noChangeIterations = 0;

		if(noChangeIterations > IonFinder::MAX_PROGRESS_ITTERATIONS)
			throw std::runtime_error("Thread timeout!");

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
	IonFinder::findFragments_(scans, 0, scans.size(),
                              peptides, pars,
							  success, scansIndex);

	bool ret = *success;
	delete success;
	return ret;
}


void IonFinder::findFragments_(std::vector<Dtafilter::Scan>& scans,
                               const size_t beg, const size_t end,
                               std::vector<PeptideNamespace::Peptide>& peptides,
                               const IonFinder::Params& pars,
                               bool* success, std::atomic<size_t>& scansIndex)
{
    // read ms files
    ms2::MsInterface msInterface;
    msInterface.read(scans.begin() + beg, scans.begin() + end);

    IonFinder::findFragments_threadSafe(scans, beg, end, msInterface,
                                        peptides, pars, success, scansIndex);
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
                                         ms2::MsInterface& msInterface,
										 std::vector<PeptideNamespace::Peptide>& peptides,
										 const IonFinder::Params& pars,
										 bool* success, std::atomic<size_t>& scansIndex)
{
	*success = false;
	std::string curSample;
	std::string curWD;
	std::string spFname;
	aaDB::AADB aminoAcidMasses;
	bool aaDBInit = false;
	ms2::Spectrum spectrum;

	for(size_t i = beg; i < end; i++)
	{
		if((pars.getInputMode() == DTAFILTER_INPUT_STR && curSample != scans[i].getSampleName()) || !aaDBInit)
		{
			//re-init Peptide::AminoAcidMasses for each sample
			curWD = utils::dirName(scans[i].getPrecursor().getFile());
			spFname = curWD + "/sequest.params";
			
			//read sequest params file and init aadb
			aminoAcidMasses.clear();
			if(pars.getInputMode() == DTAFILTER_INPUT_STR)
                PeptideNamespace::initAminoAcidsMasses(pars, spFname, aminoAcidMasses);
			else {
                PeptideNamespace::initAminoAcidsMasses(pars, aminoAcidMasses);
                if(!pars.getSmodFileSpecified() && pars.getModMass() != 0)
                    aminoAcidMasses.addMod(aaDB::AminoAcid(std::string(1, constants::MOD_CHAR), pars.getModMass()));
            }
		}//end if
		curSample = scans[i].getSampleName();
		
		//initialize peptide object for current scan
		peptides.emplace_back(scans[i].getSequence());
		peptides.back().initialize(pars, aminoAcidMasses);
		
		//add neutral loss fragments to current peptide
		if(pars.getCalcNL()){
			peptides.back().addNeutralLoss(pars.getNeutralLossMass(), pars.getLabelArtifactNL());
		}
        
        // std::cout << scans[i].getPrecursor().getFile() << " -> " << scans[i].getScanNum() << NEW_LINE;
        // peptides.back().printFragments(std::cout, false);

		if(!msInterface.getScan(spectrum,
                                scans[i].getPrecursor().getFile(),
                                scans[i].getScanNum()))
            throw std::runtime_error("Failed to retrieve scan " +
                                     std::to_string(scans[i].getScanNum()) + " from file " +
                                     scans[i].getPrecursor().getFile());

        spectrum.setScanData(&scans[i]);

        //set all precursor info except file
        scans[i].getPrecursor().setMZ(spectrum.getPrecursor().getMZ());
        scans[i].getPrecursor().setScan(spectrum.getPrecursor().getScan());
        scans[i].getPrecursor().setRT(spectrum.getPrecursor().getRT());
        scans[i].getPrecursor().setCharge(spectrum.getPrecursor().getCharge());
        scans[i].getPrecursor().setIntensity(spectrum.getPrecursor().getIntensity());

        //remove ions below specified intensity
        spectrum.normalizeIonInts(100);
        if(pars.getMinIntensitySpecified())
            spectrum.removeIntensityBelow(pars.getMinIntensity());
		// spectrum.labelSpectrum(peptides.back(), pars, true); //removes unlabeled ions from peptide

        // label spectrum
        spectrum.labelSpectrum(peptides.back(), pars);

        //Filter ion intensities
        if(pars.getMinLabelIntensity() > 0)
            peptides.back().removeLabelIntensityBelow(pars.getMinLabelIntensity(), false, false);
        if(pars.getNlIntCo() > 0)
            peptides.back().removeLabelIntensityBelow(pars.getNlIntCo(), true, false);

		//print spectra file
		if(pars.getPrintSpectraFiles())
		{
			std::string dirNameTemp = (pars.getInDirSpecified() ? pars.getWD() : curWD) + "/spectraFiles";
			if(!utils::dirExists(dirNameTemp))
				if(!utils::mkdir(dirNameTemp.c_str(), "-p")){
					throw std::runtime_error("\nFailed to make dir: " + dirNameTemp);
				}

			//spectrum.normalizeIonInts(100);
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

void IonFinder::PeptideStats::calcContainsCit(bool includeCTermMod)
{
	thisContainsCit = ContainsCitType::FALSE;
	
	//is the peptide modified?
	if(modLocs.empty()) return;
	
	//Is cit modification on the c terminus?
	//modLocs.back() works because modLocs are added in the order
	//they appear in the sequence
	if(!includeCTermMod)
        if(modLocs.back() == sequence.length() - 1) return;
	
	//is there 2 or more determining NLs?
	if(ionTypesCount[IonType::DET_NL].size() >= 2){
		thisContainsCit = ContainsCitType::TRUE;
		return;
	}
	
	//are there 1 or more determining NLs or determining frags?
	if(!ionTypesCount[IonType::DET_NL].empty() ||
       !ionTypesCount[IonType::DET].empty()){
		thisContainsCit = ContainsCitType::LIKELY;
		return;
	}
	
	//are there 1 more ambiguous fragments?
	if(!ionTypesCount[IonType::AMB].empty()){
		thisContainsCit = ContainsCitType::AMBIGUOUS;
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
		case IonType::DET: return ION_TYPES_STR[1];
			break;
		case IonType::AMB: return ION_TYPES_STR[2];
			break;
        case IonType::DET_NL: return ION_TYPES_STR[3];
            break;
		case IonType::ART_NL: return ION_TYPES_STR[4];
			break;
		case IonType::Last: return "Last";
			break;
		default:
			throw std::runtime_error("No a valid option!");
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
        default:
            throw std::runtime_error("No a valid option!");
    }
}

/**
 Prints peptide stats to file.
 \param stats Peptide stats to print.
 \param pars initialized IonFinder::Params object
 \return true if successful.
 */
bool IonFinder::printPeptideStats(const std::vector<PeptideStats>& stats,
								  const IonFinder::Params& pars)
{
	//assert(outF);
	std::ofstream outF (pars.makeOfname());
	if(!outF) return false;
	
	typedef IonFinder::PeptideStats::IonType itcType;
	//build stat names vector
	std::vector<std::string> statNames;
	
	if(pars.getCalcNL())
        statNames.emplace_back("contains_Cit");
	else statNames.emplace_back("contains_mod");
	if(pars.getGroupMod() == 0){
	    std::string temp = pars.getCalcNL() ? "this_contains_Cit" : "this_contains_mod";
	    statNames.emplace_back(temp);
	    statNames.emplace_back("mod_index");
	}
	
	//determine when to stop printing peptide stats based on analysis performed
	std::vector<itcType> _pepStats; //used to store relevant peptide stats based on params
	//defaults
	_pepStats.push_back(itcType::FRAG);
	_pepStats.push_back(itcType::DET);
	_pepStats.push_back(itcType::AMB);
	//conditional stats
	if(pars.getCalcNL()){
		_pepStats.push_back(itcType::DET_NL);
		_pepStats.push_back(itcType::ART_NL);
	}
	
	//append peptide stats names to headers
	int statLen = 0;
	for(auto & _pepStat : _pepStats){
		statNames.push_back("n" +
							std::string(1, (char)std::toupper(PeptideStats::ionTypeToStr(_pepStat)[0])) +
							PeptideStats::ionTypeToStr(_pepStat).substr(1));
        statLen++;
	}
    statNames.insert(statNames.end(), ION_TYPES_STR, ION_TYPES_STR + statLen);

	if(pars.getPrintIonIntensity()) {
        // add intensity headers
        for (auto &_pepStat: _pepStats)
            statNames.push_back("int_" + PeptideStats::ionTypeToStr(_pepStat));
        for (auto &_pepStat: _pepStats)
            statNames.push_back("totalInt_" + PeptideStats::ionTypeToStr(_pepStat));
    }

	std::string otherHeaders = "protein_ID parent_protein protein_description full_sequence sequence formula parent_mz is_modified modified_residue charge unique xCorr spectral_counts scan precursor_scan precursor_rt parent_file sample_name";
	std::vector<std::string> oHeaders;
	utils::split(otherHeaders, ' ', oHeaders);
	std::vector<std::string> headers;
	if(pars.getPrintPeptideUID())
		headers.emplace_back("peptide_unique_ID");
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
		if(pars.getPrintPeptideUID())
			outF << stat._id << OUT_DELIM;

        outF << stat._scan->getParentID() <<
			OUT_DELIM << stat._scan->getParentProtein() <<
			OUT_DELIM << stat._scan->getParentDescription() <<
			OUT_DELIM << stat._scan->getFullSequence() <<
			OUT_DELIM << scanData::removeStaticMod(stat._scan->getSequence()) <<
			OUT_DELIM << stat._scan->getFormula() <<
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
			outF << (stat.ionTypesCount.at(itcType::DET).size() > 0);
		}
		if(pars.getGroupMod() == 0){
            outF << OUT_DELIM;
            if(pars.getCalcNL())
                outF << PeptideStats::containsCitToStr(stat.thisContainsCit);
            else{
                outF << (stat.ionTypesCount.at(itcType::DET).size() > 0);
            }
            outF << OUT_DELIM << stat.modIndex;
		}

		// ion counts
		for(auto & _pepStat : _pepStats)
			outF << OUT_DELIM << stat.ionTypesCount.at(_pepStat).size();

		// list individual ions
		for(auto & _pepStat : _pepStats){
			outF << OUT_DELIM;
            for(auto it = stat.ionTypesCount.at(_pepStat).begin();
                it != stat.ionTypesCount.at(_pepStat).end();
                ++it)
            {
                if(it == stat.ionTypesCount.at(_pepStat).begin())
                    outF << it->getIonStr();
                else outF << stat._fragDelim << it->getIonStr();
            }
        }

		if(pars.getPrintIonIntensity()) {
            // list individual ion intensities
            for (auto &_pepStat : _pepStats) {
                outF << OUT_DELIM;
                for (auto it = stat.ionTypesCount.at(_pepStat).begin();
                     it != stat.ionTypesCount.at(_pepStat).end();
                     ++it) {
                    if (it == stat.ionTypesCount.at(_pepStat).begin())
                        outF << it->getIntensity();
                    else outF << stat._fragDelim << it->getIntensity();
                }
            }

            // total intensities
            for (auto &_pepStat : _pepStats)
                outF << OUT_DELIM << stat.fragmentIntensity(_pepStat);
        }

		outF << NEW_LINE;
	}
	return true;
}

