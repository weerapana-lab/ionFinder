//
//  dtafilter.hpp
//  ionFinder
//
//  Created by Aaron Maurais on 12/9/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#ifndef dtafilter_hpp
#define dtafilter_hpp

#include <iostream>
#include <fstream>

#include <paramsBase.hpp>
#include <scanData.hpp>
#include <utils.hpp>

namespace Dtafilter{
	class Scan;
	
	std::string const REVERSE_MATCH = "reverse_";
	
	bool readFilterFile(std::string fname, std::string sampleName,
						std::vector<Dtafilter::Scan>& scans,
						bool skipReverse = false, int modFilter = 1);
	
	class Scan : public scanData::Scan{
		friend bool readFilterFile(std::string, std::string,
								   std::vector<Dtafilter::Scan>&,
								   bool, int);
	public:
		enum class MatchDirection{FORWARD, REVERSE};
		static MatchDirection strToMatchDirection(std::string);
		
	private:
	    std::string _formula;
		std::string _parentProtein;
		std::string _parentID;
		std::string _parentDescription;
		MatchDirection _matchDirection;
		std::string _sampleName;
		bool _unique;
		
		bool parse_matchDir_ID_Protein(std::string);
		
	public:
		Scan() : scanData::Scan(){
		    _formula = "";
			_parentProtein = "";
			_parentID = "";
			_parentDescription = "";
			_unique = false;
			_matchDirection = MatchDirection::REVERSE;
		}
		
		//modifiers
		void operator = (const Scan&);
        void setFormula(std::string s){
            _formula = s;
        }
		void setParentProtein(std::string s){
			_parentProtein = s;
		}
		void setParentID(std::string s){
			_parentID = s;
		}
		void setMatchDirection(MatchDirection m){
			_matchDirection = m;
		}
		void setSampleName(std::string s){
			_sampleName = s;
		}
		void setParentDescription(std::string s){
			_parentDescription = s;
		}
		void setUnique(bool boo){
			_unique = boo;
		}
		
		//properties
        std::string getFormula() const{
            return _formula;
        }
		std::string getParentProtein() const{
			return _parentProtein;
		}
		std::string getParentID() const{
			return _parentID;
		}
		MatchDirection getMatchDirection() const{
			return _matchDirection;
		}
		std::string getSampleName() const{
			return _sampleName;
		}
		std::string getParentDescription() const{
			return _parentDescription;
		}
		bool getUnique() const{
			return _unique;
		}
	};
}

#endif /* dtafilter_hpp */
