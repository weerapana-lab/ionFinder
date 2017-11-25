//
//  aaDB.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/6/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef aaDB_hpp
#define aaDB_hpp

#include <iostream>
#include <map>
#include "../include/utils.hpp"

using namespace std;

namespace aaDB{
	
	string const SMOD_END_TAG = "</staticModifications>";
	string const SMOD_BEGIN_TAG = "<staticModifications>";
	int const MAX_PARAM_ITERATIONS = 1000;
	
	class AminoAcid;
	class AADB;
	
	typedef map<string, AminoAcid> aminoAcidsDBType;
	
	class AminoAcid{
	private:
		string symbol;
		double mass;
		double modification;
	
	public:
		AminoAcid(){
			symbol = ""; mass = 0; modification = 0;
		}
		AminoAcid(string);
		AminoAcid(string _symbol, double _mass, double _modification = 0){
			symbol = _symbol;
			mass = _mass;
			modification = _modification;
		}
		~AminoAcid() {};
		
		//modifers
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
		string getSymbol() const{
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
		
		//modifers
		bool readInAADB(string);
		bool readInModDB(string, aminoAcidsDBType&);
		void addStaticMod(const aminoAcidsDBType&, bool);
		
	public:
		//constructor
		AADB(){}
		~AADB(){}
		
		//modifers
		bool initalize(string aaDBLoc, string modDBLoc, bool showWarnings = true);
		bool initalize(string aaDBLoc, const aminoAcidsDBType&, bool showWarnings = true);
		
		//properties
		double calcMW(string sequence, bool addNTerm = true, bool addCTerm = true) const;
		double getMW(string) const;
		double getMW(char) const;
	};//end of class
	
}//end of namespace

#endif /* aaDB_hpp */
