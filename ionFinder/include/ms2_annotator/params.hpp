//
// params.hpp
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

#ifndef params_hpp
#define params_hpp

#include <iostream>
#include <string>

#include <paramsBase.hpp>
#include <utils.hpp>
#include <ms2_annotator/ms2_annotator.hpp>

namespace Ms2_annotator{
	
	//program file locations
	std::string const PROG_HELP_FILE = base::PROG_DB + "/ms2_annotator/helpFile.man";
	std::string const PROG_USAGE_FNAME = base::PROG_DB + "/ms2_annotator/usage.txt";
	
	class InFile;
	class Params;
	
	class InFile{
		friend class Params;
	private:
		std::string infile;
		std::string sequence;
		int scan;
		
	public:
		InFile() {
			infile = "";
			sequence = "";
			scan = 0;
		}
		~InFile() {}
		
		std::string getInfile() const{
			return infile;
		}
		std::string getSeq() const{
			return sequence;
		}
		int getScan() const{
			return scan;
		}
	};
	
	class Params : public base::ParamsBase{
	private:
		bool force;
		int inputMode;
		
		bool seqSpecified;
		bool ms2Specified, scanSpecified;
		bool dtaSpecified;
		
		//program parameters
		
		bool checkParams() const;
		
	public:
		InFile inFile;
		
		Params() : ParamsBase(PROG_USAGE_FNAME, PROG_HELP_FILE) {
			force = false;
			inputMode = 0;
			wdSpecified = false;
			sequestParamsFname = "";
			ofname = "";
			
			seqSpecified = false;
			dtaSpecified = false;
			smodSpecified = false;
			scanSpecified = false;
			ms2Specified = false;
			verbose = true; //set verbose to true by default
		}
		
		bool getArgs(int, const char* const[]);
		
		//properties
		bool getForce() const{
			return force;
		}
		int getInputMode() const{
			return inputMode;
		}
	};
}

#endif /* params_hpp */
