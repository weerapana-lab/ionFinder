//
//  peptide.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 3/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef peptide_hpp
#define peptide_hpp

#include <map>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cassert>
#include "../include/utils.hpp"
#include "../include/aaDB.hpp"
#include "../include/params.hpp"

using namespace std;

namespace peptide{
	
	//namsepace scoped constants
	const char* DIFFMODS = "*";
	
	//foward class declarations
	class Species;
	class Ion;
	class PeptideIon;
	class Peptide;
	class FragmentIon;
	
	typedef vector<PeptideIon> PepIonVecType;
	typedef PepIonVecType::const_iterator PepIonIt;
	
	//foward function declarations
	double calcMass(double mz, int charge);
	double calcMZ(double mass, int charge);
	double calcMass(string sequence);
	double calcMass(const PepIonVecType& sequence,
					PepIonIt begin, PepIonIt end);
	double calcMass(const vector<PeptideIon>& sequence);
	
	class Species{
	protected:
		double mass;
		
	public:
		Species(){
			mass = 0;
		}
		
		//properties
		double getMass() const{
			return mass;
		}
	};
	
	//class declarations
	class Ion : public Species{
	protected:
		int charge;
	public:
		//constructors
		Ion() : Species(){
			charge = 0;
		}
		~Ion(){}
		
		//modifers
		void initializeFromMZ(double _mz, int _charge){
			mass = peptide::calcMass(_mz, _charge);
			charge = _charge;
		}
		void initalizeFromMass(double _mass, int _charge = 1){
			mass = _mass;
			charge = _charge;
		}
		
		double getMZ(int _charge) const{
			return peptide::calcMZ(mass, _charge);
		}
		double getMZ() const{
			return getMZ(charge);
		}
		int getCharge() const{
			return charge;
		}
		string makeChargeLable() const;
	};//end of class
	
	class PeptideIon : public Ion{
	protected:
		bool modified;
		char mod;
		double modMass;
	public:
		PeptideIon() : Ion() {
			modified = false;
			modMass = 0;
			mod = '\0';
		}
		PeptideIon(double _mass) : Ion() {
			initalizeFromMass(_mass);
			modified = false;
			modMass = 0;
			mod = '\0';
		}
		
		void setMod(char _mod, double _modMass){
			mod = _mod;
			modMass = _modMass;
			modified = true;
		}
		
		string makeModLable() const;
		double getModMass() const{
			return modMass;
		}
		double getTotalMass() const{
			return modMass + mass;
		}
	};
	
	class FragmentIon : public Ion{
	private:
		char b_y;
		size_t num;
		size_t mz_int;
		string mod;
		
	public:
		FragmentIon(char _b_y, int _num, int _charge, double _mass, string _mod) : Ion() {
			b_y = _b_y;
			num = _num;
			mod = _mod;
			initalizeFromMass(_mass, _charge);
			mz_int = utils::round(getMZ());
		}
		~FragmentIon() {}
		
		//properties
		string getIonStr() const;
		string getFormatedLabel() const;
		template <typename _Tp>	string getFormatedLabel(_Tp) const;
		size_t getMZ_int() const{
			return mz_int;
		}
		char getBY() const{
			return b_y;
		}
	};
	
	class Peptide : public Ion{
	private:
		typedef vector<FragmentIon> FragmentIonType;
		typedef FragmentIonType::const_iterator FragmentIonItType;
		
		string sequence;
		vector<PeptideIon> aminoAcids;
		bool initalized;
		aaDB::AADB aminoAcidMasses;
		FragmentIonType fragments;
		
		void fixDiffMod();
	
	public:
		//constructors
		Peptide() : Ion(){
			sequence = "";
			initalized = false;
		}
		Peptide(string _sequence) : Ion(){
			sequence = _sequence;
			initalized = false;
		}
		~Peptide() {}
		
		void initalize(const params::Params&, bool _calcFragments = true);
		void calcFragments(int, int);
		double calcMass();
		void printFragments(ostream&) const;
		
		//properties
		string getSequence() const{
			return sequence;
		}
		size_t getNumFragments() const{
			return fragments.size();
		}
		size_t getFragmentMZ_int(size_t i) const{
			return fragments[i].getMZ_int();
		}
		double getFragmentMZ(size_t i) const{
			return fragments[i].getMZ(fragments[i].getCharge());
		}
		string getFragmentLabel(size_t i) const{
			return fragments[i].getIonStr();
		}
		string getFormatedLabel(size_t i) const{
			return fragments[i].getFormatedLabel();
		}
		template<typename _Tp>
		string getFormatedLabel(size_t i, _Tp num) const{
			return fragments[i].getFormatedLabel(num);
		}
		char getBY(size_t i) const{
			return fragments[i].getBY();
		}
	};//end of class
	
	
}//end of namespace

#endif /* peptide_hpp */
