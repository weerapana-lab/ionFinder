//
//  scanData.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/23/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef scanData_hpp
#define scanData_hpp

#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>

#include <utils.hpp>

namespace scanData{
	
	class Scan;
	class PrecursorScan;

	typedef std::vector<Scan> scansType;
	std::string const OF_EXT = ".spectrum";
	const char MOD_CHAR = '*';
	
	std::string removeStaticMod(std::string s, bool lowercase = true);
	std::string removeDynamicMod(std::string s, bool lowercase = true);

    class PrecursorScan{
    private:
        std::string _mz;
        std::string _scan;
        double _rt;
        std::string _file;
        int _charge;
        double _intensity;

    public:
        PrecursorScan(){
            _mz = "";
            _scan = "";
            _rt = 0;
            _file = "";
            _charge = 0;
            _intensity = 0;
        }

        PrecursorScan& operator = (const PrecursorScan& rhs){
            _mz = rhs._mz;
            _scan = rhs._scan;
            _rt = rhs._rt;
            _file = rhs._file;
            _charge = 0;
            _intensity = 0;
            return *this;
        }

        //modifers
        void setMZ(const std::string &mz);
        void setScan(const std::string &scan);
        void setRT(double rt);
        void setFile(const std::string &file);
        void setCharge(int);
        void setIntensity(double);
        void clear();

        //properties
        const std::string &getMZ() const;
        const std::string &getScan() const;
        double getRT() const;
        const std::string &getFile() const;
        double getIntensity() const;
        int getCharge() const;
    };

	class Scan{
	protected:
		size_t _scanNum;
        std::string _sequence;
		std::string _fullSequence;
		std::string _xcorr;
		bool _modified;
		int _spectralCounts;

        PrecursorScan _precursor;
		
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
		void setPrecursor(const PrecursorScan& rhs){
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
		//!Does the peptide contain a dynamic modification?
		bool isModified() const{
			return _modified;
		}
		std::string getOfNameBase(std::string, std::string) const;
		std::string getOfname() const;
        const PrecursorScan& getPrecursor() const;
        PrecursorScan& getPrecursor();
	};
}

#endif /* scanData_hpp */
