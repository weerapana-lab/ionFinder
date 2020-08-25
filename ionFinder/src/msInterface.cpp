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
        ms2Map[it] = std::make_shared<utils::Ms2File>();
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
    std::vector<std::string> fnames;
    getUniqueFileList(fnames, begin, end);
    return readMs2s(_ms2Map, begin, end);
}

bool ms2::MsInterface::getScan(utils::Scan& scan, std::string fname, size_t scanNum)
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
                                  std::vector<Dtafilter::Scan>::const_iterator end)
{
    fnames.clear();
    for(auto scan = begin; scan != end; scan++){
        if(std::find(fnames.begin(), fnames.end(),
                     scan->getPrecursor().getFile()) == fnames.end()){
            fnames.push_back(scan->getPrecursor().getFile());
        }
    }
}

