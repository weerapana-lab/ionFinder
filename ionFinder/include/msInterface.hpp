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

#include <dtafilter.hpp>
#include <msInterface/msScan.hpp>

#ifdef USE_MSTOOLKIT_LIB
    #include <MSToolkitTypes.h>
    #include <MSReader.h>
    #include <Spectrum.h>
#endif
    #include <msInterface/ms2File.hpp>

namespace ms2 {
    class MsInterface;


    class MsInterface {
        typedef utils::Ms2File MsFile;
        typedef std::map<std::string, utils::MsInterface*> Ms2Map;
        typedef std::vector<Dtafilter::Scan> InputScanList;
        typedef std::map<std::string, std::map<size_t, utils::Scan> > ScanMap;
        typedef std::map<std::string, std::vector<size_t> > ScanList;
        enum class FileType {MS2, MZXML, MZML, UNKNOWN};

#ifdef USE_MSTOOLKIT_LIB
        ScanMap _scanList;

        static void msToolkitSpectrumToScan(utils::Scan&, MSToolkit::Spectrum);
#else
        Ms2Map _ms2Map;
#endif
        bool readMs2s(Ms2Map&,
                  std::vector<Dtafilter::Scan>::const_iterator begin,
                  std::vector<Dtafilter::Scan>::const_iterator end);

        static void getUniqueFileList(std::vector<std::string>& fnames,
                                      std::vector<Dtafilter::Scan>::const_iterator begin,
                                      std::vector<Dtafilter::Scan>::const_iterator end);
        static void initScanMap(ScanMap& scanMap,
                                std::vector<Dtafilter::Scan>::const_iterator begin,
                                std::vector<Dtafilter::Scan>::const_iterator end);
        static FileType getFileType(std::string fname);
    public:
        MsInterface(){};

        bool read(InputScanList::const_iterator begin, InputScanList::const_iterator end);
        bool getScan(utils::Scan&, std::string fname, size_t scanNum);
    };

}

#endif //ionfinder_msinterface_hpp
