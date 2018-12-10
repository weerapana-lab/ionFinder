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
#include <string>

#include <utils.hpp>
#include <aaDB.hpp>
#include <ms2_annotator/params.hpp>
#include <sequestParams.hpp>

namespace PeptideNamespace{
	
	//foward class declarations
	class Species;
	class Ion;
	class PeptideIon;
	class Peptide;
	class FragmentIon;
	
	typedef std::vector<PeptideIon> PepIonVecType;
	typedef PepIonVecType::const_iterator PepIonIt;
	
	//foward function declarations
	double calcMass(double mz, int charge);
	double calcMZ(double mass, int charge);
	double calcMass(std::string sequence);
	double calcMass(const PepIonVecType& sequence,
					PepIonIt begin, PepIonIt end);
	double calcMass(const std::vector<PeptideIon>& sequence);
	
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
			mass = calcMass(_mz, _charge);
			charge = _charge;
		}
		void initalizeFromMass(double _mass, int _charge = 1){
			mass = _mass;
			charge = _charge;
		}
		
		double getMZ(int _charge) const{
			return calcMZ(mass, _charge);
		}
		double getMZ() const{
			return getMZ(charge);
		}
		int getCharge() const{
			return charge;
		}
		std::string makeChargeLable() const;
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
		}
		
		std::string makeModLable() const;
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
		//size_t mz_int;
		std::string mod;
		
	public:
		FragmentIon(char _b_y, int _num, int _charge, double _mass, std::string _mod) : Ion() {
			b_y = _b_y;
			num = _num;
			mod = _mod;
			initalizeFromMass(_mass, _charge);
			//mz_int = utils::round(getMZ());
		}
		~FragmentIon() {}
		
		//properties
		std::string getIonStr() const;
		std::string getFormatedLabel() const;
		//size_t getMZ_int() const{
		//	return mz_int;
		//}
		char getBY() const{
			return b_y;
		}
	};
	
	class Peptide : public Ion{
	private:
		typedef std::vector<FragmentIon> FragmentIonType;
		typedef FragmentIonType::const_iterator FragmentIonItType;
		
		std::string sequence;
		std::string fullSequence;
		std::vector<PeptideIon> aminoAcids;
		bool initalized;
		static aaDB::AADB* aminoAcidMasses;
		static bool aminoAcidMassesInitilized;
		FragmentIonType fragments;
		
		void fixDiffMod(const char* diffmods = "*");
	
	public:
		//constructors
		Peptide() : Ion(){
			sequence = "";
			fullSequence = sequence;
			initalized = false;
		}
		Peptide(std::string _sequence) : Ion(){
			sequence = _sequence;
			fullSequence = sequence;
			initalized = false;
		}
		~Peptide() {}
		
		void initalize(const base::ParamsBase&, bool _calcFragments = true);
		void calcFragments(int, int);
		double calcMass();
		void printFragments(std::ostream&) const;
		
		//properties
		std::string getSequence() const{
			return sequence;
		}
		std::string getFullSequence() const{
			return fullSequence;
		}
		size_t getNumFragments() const{
			return fragments.size();
		}
		//size_t getFragmentMZ_int(size_t i) const{
		//	return fragments[i].getMZ_int();
		//}
		double getFragmentMZ(size_t i) const{
			return fragments[i].getMZ(fragments[i].getCharge());
		}
		std::string getFragmentLabel(size_t i) const{
			return fragments[i].getIonStr();
		}
		std::string getFormatedLabel(size_t i) const{
			return fragments[i].getFormatedLabel();
		}
		template<typename _Tp>
		std::string getFormatedLabel(size_t i, _Tp num) const{
			return fragments[i].getFormatedLabel(num);
		}
		char getBY(size_t i) const{
			return fragments[i].getBY();
		}
	};//end of class
	
	
}//end of namespace

#endif /* peptide_hpp */
