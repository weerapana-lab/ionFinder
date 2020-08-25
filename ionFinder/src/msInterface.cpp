//
// msInterface.hpp
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

#include <msInterface.hpp>

#ifdef USE_MSTOOLKIT_LIB

// bool ms2::MsInterface::read(InputScanList::const_iterator begin, InputScanList::const_iterator end) {
//     _msReader.addFilter(MSToolkit::MS2);
//     _msReader.addFilter(MSToolkit::MSX);
//
//     // getUniqueScanList(_scanList, begin, end);
//     return true;
// }
//
// bool ms2::MsInterface::getScan(utils::Scan& scan, std::string fname, size_t scanNum)
// {
//     std::cout << fname << std::endl;
//     MSToolkit::Spectrum _spectrum;
//     if(!_msReader.readFile(fname.c_str(), _spectrum, scanNum)) {
//         std::cerr << NEW_LINE << "ERROR: Scan " + std::to_string(scanNum) + " not found in file: " + fname << NEW_LINE;
//         return false;
//     }
//
//     if(_spectrum.getCentroidStatus() == 0) {
//         std::cerr << NEW_LINE << "ERROR: Scan:" + std::to_string(scanNum) + " is not in centroid mode!" << NEW_LINE;
//         return false;
//     }
//
//     msToolkitSpectrumToScan(scan, _spectrum);
//
//     return true;
// }

void ms2::MsInterface::msToolkitSpectrumToScan(utils::Scan& scan, MSToolkit::Spectrum spectrum)
{
    scan.clear();

    scan.setScanNum(spectrum.getScanNumber());

    //copy ions
    int size = spectrum.size();
    for(int i = 0; i < size; i++) {
        scan.add(spectrum[i].mz, spectrum[i].intensity);
    }
    scan.updateRanges();
}


#endif

bool ms2::MsInterface::readMs2s(ms2::MsInterface::Ms2Map& ms2Map,
                         std::vector<Dtafilter::Scan>::const_iterator begin,
                         std::vector<Dtafilter::Scan>::const_iterator end)
{
    //first get unique names of ms2 files to read
    std::vector<std::string> fileNamesList;
    ms2::MsInterface::getUniqueFileList(fileNamesList, begin, end);

    //read ms2 files
    ms2Map.clear();
    bool allSucess = true;
    for(auto & it : fileNamesList)
    {
        ms2Map[it] = new utils::Ms2File();
        if(!ms2Map[it]->read(it)){
            std::cerr << "\n\tFailed to read: " << it << NEW_LINE;
            std::cerr << "\t\tNo file found at: " << utils::absPath(it) << NEW_LINE;
            allSucess = false;
        }
    }

    if(!allSucess){
        std::cerr << "Error reading ms2 files!" << NEW_LINE;
        return false;
    }

    return true;
}

bool ms2::MsInterface::read(InputScanList::const_iterator begin, InputScanList::const_iterator end){
#ifdef USE_MSTOOLKIT_LIB
    std::set<std::string> ms2_fnames;
    initScanMap(_scanList, begin, end);

    for(auto file_it = _scanList.begin(); file_it != _scanList.end(); ++file_it) {
        FileType thisFileType = getFileType(file_it->first);
        if(thisFileType == FileType::MZXML || thisFileType == FileType::MZML)
        {
            // init reader
            MSToolkit::MSReader reader;
            reader.addFilter(MSToolkit::MS2);
            MSToolkit::Spectrum spec;
            reader.readFile(file_it->first.c_str(), spec);

            // Iterate through the entire file because random access doesn't work the way it is
            // suppose to with the mstoolkit library
            // TODO: write my own mzxml reader which actually works the way it is suppose to
            do {
                auto scan_it = file_it->second.find(spec.getScanNumber());
                if(scan_it != file_it->second.end()) {
                    msToolkitSpectrumToScan(scan_it->second, spec);
                }
            } while(reader.nextSpectrum(spec));
        }
        else if(thisFileType == FileType::MS2) {
            //Use my ms2 reader because the mstoolkit one doesn't work and throws random seg faults
            utils::Ms2File ms2File(file_it->first);
            ms2File.read();
            for(auto scan_it = file_it->second.begin(); scan_it != file_it->second.end(); ++scan_it) {
                if(!ms2File.getScan(scan_it->first, scan_it->second)) {
                    std::cerr << NEW_LINE << "Scan " <<
                        scan_it->first << " not found in " <<
                        file_it->first << "!" << NEW_LINE;
                    return false;
                }
            }
        }
        else{
             std::cerr << NEW_LINE << "Could not determine the file type of " << file_it->first << NEW_LINE;
             return false;
        } // end if thisFileType
    } //end for file_it
    return true;
#else
    std::vector<std::string> fnames;
    getUniqueFileList(fnames, begin, end);
    return readMs2s(_ms2Map, begin, end);
#endif
}

bool ms2::MsInterface::getScan(utils::Scan& scan, std::string fname, size_t scanNum)
{
#ifdef USE_MSTOOLKIT_LIB
    auto file_it = _scanList.find(fname);
    if(file_it == _scanList.end()) {
        std::cerr << NEW_LINE << "Key error in MsInterface::_scanList: " << fname << NEW_LINE;
        return false;
    }
    auto scan_it = file_it->second.find(scanNum);
    if(scan_it == file_it->second.end()) {
        std::cerr << NEW_LINE << "Scan " << scanNum << " not found in " << fname << "!" << NEW_LINE;
        return false;
    }
    scan = scan_it->second;
#else
    //load spectrum
    auto ms2FileIt = _ms2Map.find(fname);
    if(ms2FileIt == _ms2Map.end()){
        std::cerr << NEW_LINE << "Key error in Ms2Map!" << NEW_LINE;
        return false;
    }
    if(!ms2FileIt->second->getScan(scanNum, scan)){
        std::cerr << NEW_LINE << "Error reading scan!" << NEW_LINE;
        return false;
    }
#endif
    return true;
}

//! Get a list of unique file names between \p begin and \p end
void ms2::MsInterface::getUniqueFileList(std::vector<std::string> &fnames,
                                  std::vector<Dtafilter::Scan>::const_iterator begin,
                                  std::vector<Dtafilter::Scan>::const_iterator end)
{
    fnames.clear();
    for(auto scan = begin; scan != end; scan++){
        if(std::find(fnames.begin(),
                     fnames.end(),
                     scan->getPrecursor().getFile()) == fnames.end()){
            fnames.push_back(scan->getPrecursor().getFile());
        }
    }
}

//! Get a list of unique file names between \p begin and \p end
void ms2::MsInterface::initScanMap(ms2::MsInterface::ScanMap& scanMap,
                                  std::vector<Dtafilter::Scan>::const_iterator begin,
                                  std::vector<Dtafilter::Scan>::const_iterator end)
{
    scanMap.clear();
    for(auto scan = begin; scan != end; scan++)
    {
        std::string file_temp = scan->getPrecursor().getFile();
        auto it = scanMap.find(file_temp);
        if(it == scanMap.end())
            scanMap[file_temp] = std::map<size_t, utils::Scan>();
        scanMap[file_temp][scan->getScanNum()] = utils::Scan();
    }
}

ms2::MsInterface::FileType ms2::MsInterface::getFileType(std::string fname)
{
    std::string ext = utils::toLower(utils::getExtension(fname));
    if(ext == "ms2")
        return ms2::MsInterface::FileType::MS2;
    else if(ext == "mzxml")
        return ms2::MsInterface::FileType::MZXML;
    else if(ext == "mzml")
        return ms2::MsInterface::FileType::MZXML;
    else return ms2::MsInterface::FileType::UNKNOWN;
}

