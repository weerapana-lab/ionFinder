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

#ifndef ionfinder_msinterface_hpp
#define ionfinder_msinterface_hpp

#include <string>
#include <vector>
#include <map>
#include <memory>

#include <dtafilter.hpp>
#include <msInterface/msInterface.hpp>
#include <msInterface/msScan.hpp>
#include <msInterface/ms2File.hpp>
#include <msInterface/mzXMLFile.hpp>
#include <msInterface/mzMLFile.hpp>

namespace ms2 {
    class MsInterface;


    class MsInterface {
        typedef utils::msInterface::Ms2File MsFile;
        typedef std::map<std::string, std::shared_ptr<utils::msInterface::MsInterface> > Ms2Map;
        typedef std::vector<Dtafilter::Scan> InputScanList;

        Ms2Map _ms2Map;
        void getUniqueFileList(std::vector<std::string>& fnames,
                               std::vector<Dtafilter::Scan>::const_iterator begin,
                               std::vector<Dtafilter::Scan>::const_iterator end) const;
    public:
        MsInterface(){
            _ms2Map = Ms2Map();
        }
        ~MsInterface(){}

        bool read(InputScanList::const_iterator begin, InputScanList::const_iterator end, bool showProgress = true);
        bool getScan(utils::msInterface::Scan&, std::string fname, size_t scanNum) const;
    };

}

#endif //ionfinder_msinterface_hpp
