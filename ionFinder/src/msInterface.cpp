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

bool ms2::MsInterface::read(InputScanList::const_iterator begin,
                            InputScanList::const_iterator end,
                            bool showProgress)
{
    //first get unique names of ms2 files to read
    std::vector<std::string> fileNamesList;
    ms2::MsInterface::getUniqueFileList(fileNamesList, begin, end);

    //read ms2 files
    _ms2Map.clear();
    bool allSucess = true;
    size_t len = fileNamesList.size();
    std::cout << NEW_LINE << "Reading MS files..." << NEW_LINE;
    for(size_t i = 0; i < len; i++)
    {
        if(showProgress)
            utils::printProgress(float(i) / float(len));
        std::string _fname = fileNamesList[i];
        utils::msInterface::MsInterface::FileType fileType = utils::msInterface::MsInterface::getFileType(_fname);
        if(fileType == utils::msInterface::MsInterface::FileType::MS2)
            _ms2Map[_fname] = std::make_shared<utils::msInterface::Ms2File>();
        else if(fileType == utils::msInterface::MsInterface::FileType::MZXML)
            _ms2Map[_fname] = std::make_shared<utils::msInterface::MzXMLFile>();
        else if(fileType == utils::msInterface::MsInterface::FileType::MZML)
            _ms2Map[_fname] = std::make_shared<utils::msInterface::MzMLFile>();
        else {
            std::cerr << "Unknown file type for file " << _fname << NEW_LINE;
            return false;
        }

        if(!_ms2Map[_fname]->read(_fname)){
            std::cerr << "\n\tFailed to read: " << _fname << NEW_LINE;
            std::cerr << "\t\tNo file found at: " << utils::absPath(_fname) << NEW_LINE;
            allSucess = false;
        }
    }
    if(showProgress) {
        utils::printProgress(1.0);
        std::cout << NEW_LINE;
    }

    if(!allSucess){
        std::cerr << "Error reading ms2 files!" << NEW_LINE;
        return false;
    }

    return true;
}

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
        if(std::find(fnames.begin(), fnames.end(),
                     scan->getPrecursor().getFile()) == fnames.end()){
            fnames.push_back(scan->getPrecursor().getFile());
        }
    }
}

