//
// scanData.hpp
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

#ifndef scanData_hpp
#define scanData_hpp

#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>

#include <constants.hpp>
#include <utils.hpp>
#include <msInterface/msScan.hpp>

namespace scanData{
	class Scan;

	typedef std::vector<Scan> scansType;
	std::string const OF_EXT = ".spectrum";
	const char MOD_CHAR = constants::MOD_CHAR;
	
	std::string removeStaticMod(std::string s, bool lowercase = true);
	std::string removeDynamicMod(std::string s, bool lowercase = true);

	class Scan{
	protected:
		size_t _scanNum;
        std::string _sequence;
		std::string _fullSequence;
		std::string _xcorr;
		bool _modified;
		int _spectralCounts;

        utils::msInterface::PrecursorScan _precursor;
		
		void initilizeFromLine(std::string);
		std::string makeSequenceFromFullSequence(std::string) const;
		std::string makeOfSequenceFromSequence(std::string) const;
	public:
		Scan(): _precursor(){
			_scanNum = 0;
			_sequence = "";
			_modified = false;
			_spectralCounts = 0;
		}
		Scan(const std::string& sequence, size_t scanNum, const std::string& parentFile): _precursor(){
			_sequence = makeSequenceFromFullSequence(sequence);
			_fullSequence = sequence;
			_scanNum = scanNum;
			_modified = utils::strContains(MOD_CHAR, _sequence);
			_spectralCounts = 0;
			_precursor.setFile(parentFile);
		}
		
		Scan(std::string line){
			initilizeFromLine(std::move(line));
		}
		~Scan() = default;

        virtual void clear();
	
		Scan& operator = (const Scan& rhs){
			_scanNum = rhs._scanNum;
			_sequence = rhs._sequence;
			_fullSequence = rhs._fullSequence;
			_xcorr = rhs._xcorr;
			_spectralCounts = rhs._spectralCounts;
			_precursor = rhs._precursor;
			return *this;
		}
		
		void setSequence(std::string seq){
			_sequence = seq;
		}
		void setIsModified(bool rhs) {
            _modified = rhs;
        }
		void setFullSequence(std::string s, bool resetSequence = false){
			_fullSequence = s;
			if(resetSequence)
				_sequence = makeOfSequenceFromSequence(s);
		}
		void setScanNum(size_t s){
			_scanNum = s;
		}
		void setXcorr(std::string s){
			_xcorr = s;
		}
		void setSpectralCounts(int sc){
			_spectralCounts = sc;
		}
		void setPrecursor(const utils::msInterface::PrecursorScan& rhs){
			_precursor = rhs;
		}

		//properties
		size_t getScanNum() const{
			return _scanNum;
		}
		std::string getSequence() const{
			return _sequence;
		}
		std::string getFullSequence() const{
			return _fullSequence;
		}
		std::string getXcorr() const{
			return _xcorr;
		}
		int getSpectralCounts() const{
			return _spectralCounts;
		}
		bool checkIsModified() const;
		//!Does the peptide contain a dynamic modification?
		bool isModified() const{
			return _modified;
		}
		std::string getOfNameBase(std::string, std::string) const;
		std::string getOfname() const;
        const utils::msInterface::PrecursorScan& getPrecursor() const{
            return _precursor;
        }
        utils::msInterface::PrecursorScan& getPrecursor(){
            return _precursor;
        }
	};
}

#endif /* scanData_hpp */
