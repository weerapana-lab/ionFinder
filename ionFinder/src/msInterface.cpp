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

/**
 * Read an individual MS file. If the file has already been read, the file will simply return true.
 * This function is thread safe.
 * @param fname Path to file to read.
 * @return true if all file I/O was successful.
 */
bool ms2::MsInterface::read(std::string fname)
{
    // Check if file already exists and exit if it does.
    if(_ms2Map.find(fname) != _ms2Map.end()) return true;

    auto _file = std::shared_ptr<utils::msInterface::MsInterface>();
    utils::msInterface::MsInterface::FileType fileType = utils::msInterface::MsInterface::getFileType(fname);
    if(fileType == utils::msInterface::MsInterface::FileType::MS2)
        _file = std::make_shared<utils::msInterface::Ms2File>();
    else if(fileType == utils::msInterface::MsInterface::FileType::MZXML)
        _file = std::make_shared<utils::msInterface::MzXMLFile>();
    else if(fileType == utils::msInterface::MsInterface::FileType::MZML)
        _file = std::make_shared<utils::msInterface::MzMLFile>();
    else {
        std::cerr << "Unknown file type for file " << fname << NEW_LINE;
        return false;
    }
    if(!_file->read(fname)) {
        std::cerr << "\n\tFailed to read: " << fname << NEW_LINE;
        std::cerr << "\t\tNo file found at: " << utils::absPath(fname) << NEW_LINE;
        return false;
    }

    // lock mutex
    std::lock_guard<std::mutex> lock (mutex);
    _ms2Map[fname] = _file;
    return true;
}

/**
 * Read MS files from a range of Dtafilter::Scan iterators.
 * If a file name occurs more than once, it will only be read once.
 * This function is thread safe.
 * @param begin Starting iterator
 * @param end Ending iterator
 * @return true if all file I/O was successful.
 */
bool ms2::MsInterface::read(InputScanList::const_iterator begin, InputScanList::const_iterator end)
{
    //first get unique names of ms2 files to read
    std::vector<std::string> fileNamesList;
    ms2::MsInterface::getUniqueFileList(fileNamesList, begin, end);

    //read ms2 files
    bool allSucess = true;
    size_t len = fileNamesList.size();
    for(size_t i = 0; i < len; i++) {
        std::string _fname = fileNamesList[i];
        bool fileRead = _ms2Map.find(_fname) == _ms2Map.end();
        if(fileRead) {
            if(!read(_fname)) allSucess = false;
        }
    }

    if(!allSucess){
        std::cerr << "Error reading ms2 files!" << NEW_LINE;
        return false;
    }

    return true;
}

/**
 * Retrieve a Scan. If \p fname has not been read, the file will be read and stored before being parsed.
 * Because this function is not const, if the file has to be read, the MsInterface object will be modified.
 * This function is thread save.
 * @param scan Empty Scan object to populate.
 * @param fname MS file name.
 * @param scanNum Scan number to retrieve.
 * @return True if parsing scan was successful.
 */
bool ms2::MsInterface::getScan(utils::msInterface::Scan& scan, std::string fname, size_t scanNum)
{
    auto ms2FileIt = _ms2Map.find(fname);
    if(ms2FileIt == _ms2Map.end()){
        if(!read(fname)) return false;
        ms2FileIt = _ms2Map.find(fname);
    }
    if(!ms2FileIt->second->getScan(scanNum, scan)){
        std::cerr << NEW_LINE << "Error reading scan!" << NEW_LINE;
        return false;
    }
    return true;
}

/**
 * const qualified version of getScan. If \p fname has not been read, the function will return false.
 * @param scan Empty Scan object to populate.
 * @param fname MS file name.
 * @param scanNum Scan number to retrieve.
 * @return True if parsing scan was successful.
 */
bool ms2::MsInterface::getScan(utils::msInterface::Scan& scan, std::string fname, size_t scanNum) const
{
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
    return true;
}

//! Get a list of unique file names between \p begin and \p end
void ms2::MsInterface::getUniqueFileList(std::vector<std::string> &fnames,
                                         std::vector<Dtafilter::Scan>::const_iterator begin,
                                         std::vector<Dtafilter::Scan>::const_iterator end) const
{
    fnames.clear();
    for(auto scan = begin; scan != end; scan++){
        if(std::find(fnames.begin(), fnames.end(), scan->getPrecursor().getFile()) == fnames.end()){
            fnames.push_back(scan->getPrecursor().getFile());
        }
    }
}

