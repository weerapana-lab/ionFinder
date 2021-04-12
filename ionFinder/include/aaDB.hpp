//
// aaDB.hpp
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

#ifndef aaDB_hpp
#define aaDB_hpp

#include <iostream>
#include <map>
#include <string>

#include <utils.hpp>

namespace aaDB{
	
	std::string const SMOD_END_TAG = "</staticModifications>";
	std::string const SMOD_BEGIN_TAG = "<staticModifications>";
	
	class AminoAcid;
	class AADB;
	
	typedef std::map<std::string, AminoAcid> aminoAcidsDBType;
	
	class AminoAcid{
	private:
		std::string symbol;
		double mass;
		double modification;
	
	public:
		AminoAcid(){
			symbol = ""; mass = 0; modification = 0;
		}
		AminoAcid(std::string line);
		AminoAcid(std::string _symbol, double _mass, double _modification = 0){
			symbol = _symbol;
			mass = _mass;
			modification = _modification;
		}
		~AminoAcid() {};
		
		//modifiers
		void operator += (double mod){
			modification += mod;
		}
		void operator = (const AminoAcid& _add){
			symbol = _add.symbol;
			mass = _add.mass;
			modification = _add.modification;
		}
		void setMass(double _mass){
			mass = _mass;
		}
		
		//properties
		std::string getSymbol() const{
			return symbol;
		}
		double getMass() const{
			return mass + modification;
		}
		
	};//end of class

	class AADB{
	private:
		typedef aminoAcidsDBType::const_iterator itType;
		aminoAcidsDBType aminoAcidsDB;
		
		//modifiers
		void initAADB();
		bool readInModDB(std::string, aminoAcidsDBType&);
		void addStaticMod(const aminoAcidsDBType&, bool);
		
	public:
		//constructor
		AADB(){}
		~AADB(){}
		
		//modifiers
		bool initialize(std::string modDBLoc, bool showWarnings = true);
		bool initialize(const aminoAcidsDBType&, bool showWarnings = true);
		
		//properties
		double calcMW(std::string sequence, bool addNTerm = true, bool addCTerm = true) const;
		double getMW(std::string) const;
		double getMW(char) const;
		bool empty() const{
			return aminoAcidsDB.empty();
		}
	};//end of class
	
}//end of namespace

#endif /* aaDB_hpp */
