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
	
	//forward class declarations
	class Species;
	class Ion;
	class PeptideIon;
	class Peptide;
	class FragmentIon;
	
	typedef std::vector<PeptideIon> PepIonVecType;
	typedef PepIonVecType::const_iterator PepIonIt;
	
	//forward function declarations
	double calcMass(double mz, int charge);
	double calcMZ(double mass, int charge);
	double calcMass(std::string sequence);
	double calcMass(const PepIonVecType& sequence,
					PepIonIt begin, PepIonIt end);
	double calcMass(const std::vector<PeptideIon>& sequence);
	std::string concatMods(const PepIonVecType& vec,
						   PepIonIt begin, PepIonIt end);
	void initAminoAcidsMasses(const base::ParamsBase& pars, std::string seqParFname, aaDB::AADB&);
	void initAminoAcidsMasses(const base::ParamsBase&, aaDB::AADB&);
	
	//class declarations
	
	///base class for peptide species
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
	
	///base class for all ions
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
			modified = true;
		}
		
		std::string makeModLable() const;
		double getModMass() const{
			return modMass;
		}
		double getTotalMass() const{
			return modMass + mass;
		}
		char getMod() const{
			return mod;
		}
		bool isModified() const{
			return modified;
		}
	};
	
	///Used to represent b and y peptide ions
	class FragmentIon : public Ion{
	public:
		enum class IonType{B, Y, B_NL, Y_NL};
		
	protected:
		char b_y;
		int num;
		//!Represents all modifications found in fragment.
		 /** i.e. if two modifications are present, mod will be "**" */
		std::string mod;
		bool found;
		IonType _ionType;
		 //!Represents neutral loss mass
		double nlMass;
		
	public:
		///blank constructor
		FragmentIon() : Ion(){
			b_y = '\0';
			num = 0;
			mod = "";
			found = false;
			_ionType = IonType::B;
			nlMass = 0.0;
		}
		FragmentIon(char _b_y, int _num, int _charge, double _mass,
					double _modMass, std::string _mod) : Ion() {
			b_y = _b_y;
			num = _num;
			mod = _mod;
			nlMass = _modMass;
			initalizeFromMass(_mass, _charge);
			found = false;
			_ionType = strToIonType(_b_y);
		}
		FragmentIon(IonType ionType, int _num, int _charge, double _mass,
					double _modMass, std::string _mod) : Ion() {
			num = _num;
			mod = _mod;
			nlMass = _modMass;
			initalizeFromMass(_mass, _charge);
			found = false;
			_ionType = ionType;
			b_y = ionTypeToStr()[0];
		}
		///copy constructor
		FragmentIon(const FragmentIon& rhs){
			b_y = rhs.b_y;
			num = rhs.num;
			mod = rhs.mod;
			found = rhs.found;
			_ionType = rhs._ionType;
			nlMass = rhs.nlMass;
			charge = rhs.charge;
			mass = rhs.mass;
		}
		~FragmentIon() {}
		
		void setFound(bool boo){
			found = boo;
		}
		void setIonType(IonType it){
			_ionType = it;
		}
		
		//properties
		double getMZ() const{
			if(b_y == 'b')
				return calcMZ(mass - 1, charge);
			return Ion::getMZ(charge);
		}
		std::string getIonStr(bool includeMod = true) const;
		std::string getFormatedLabel() const;
		char getBY() const{
			return b_y;
		}
		int getNum() const{
			return num;
		}
		std::string getMod() const{
			return mod;
		}
		bool getFound() const{
			return found;
		}
		IonType getIonType() const{
			return _ionType;
		}
		static IonType strToIonType(std::string);
		static IonType strToIonType(char c){
			return strToIonType(std::string(1, c));
		}
		std::string ionTypeToStr() const;
		std::string getNLStr() const;
		bool isModified() const{
			return mod == "";
		}
		bool isNL() const{
			return _ionType == IonType::B_NL || _ionType == IonType::Y_NL;
		}
	};
	
	///Used to store fragment data for each peptide.
	class Peptide : public Ion{
	private:
		typedef std::vector<FragmentIon> FragmentIonType;
		typedef FragmentIonType::const_iterator FragmentIonItType;
		
		std::string sequence;
		std::string fullSequence;
		std::vector<PeptideIon> aminoAcids;
		bool initialized;
		FragmentIonType fragments;
		int nMod; //number of modified residues
		
		void fixDiffMod(const aaDB::AADB& aminoAcidsMasses,
						const char* diffmods = "*");
	public:
		//constructors
		Peptide() : Ion(){
			sequence = "";
			fullSequence = sequence;
			initialized = false;
			nMod = 0;
		}
		Peptide(std::string _sequence) : Ion(){
			sequence = _sequence;
			fullSequence = sequence;
			initialized = false;
			nMod = 0;
		}
		~Peptide() {}
		
		//modifers
		void initialize(const base::ParamsBase&, const aaDB::AADB& aadb,
						bool _calcFragments = true);
		void calcFragments(int minCharge, int maxCharge,
						   const aaDB::AADB& aminoAcidsMasses);
		void addNeutralLoss(const std::vector<double>&);
		double calcMass(const aaDB::AADB& aminoAcidsMasses);
		void printFragments(std::ostream&) const;
		
		void setFound(size_t i, bool boo){
			fragments[i].setFound(boo);
		}
		void removeUnlabeledFrags();
		
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
		double getFragmentMZ(size_t i) const{
			return fragments[i].getMZ();
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
		bool getFound(size_t i) const{
			return fragments[i].getFound();
		}
		const FragmentIon& getFragment(size_t i) const{
			return fragments[i];
		}
		int getNumMod() const{
			return nMod;
		}
	};//end of class
	
}//end of namespace

#endif /* peptide_hpp */
