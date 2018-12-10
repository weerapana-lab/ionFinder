//
//  dtafilter.hpp
//  citFinder
//
//  Created by Aaron Maurais on 12/9/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#ifndef dtafilter_hpp
#define dtafilter_hpp

#include <iostream>
#include <fstream>

#include <citFinder/params.hpp>
#include <scanData.hpp>
#include <utils.hpp>

namespace CitFinder{
	class Scan;
	
	std::string const REVERSE_MATCH = "reverse_";
	
	bool readFilterFile(std::string fname, std::string sampleName,
						std::vector<CitFinder::Scan>& scans,
						bool skipReverse = true);
	bool readFilterFiles(const CitFinder::Params&,
						 std::vector<CitFinder::Scan>&);
	
	class Scan : public scanData::Scan{
		friend bool readFilterFile(std::string, std::string,
								   std::vector<CitFinder::Scan>&, bool);
	public:
		enum class MatchDirection{FORWARD, REVERSE};
		
	private:
		std::string _parentProtein;
		std::string _parentID;
		MatchDirection _matchDirection;
		std::string _sampleName;
		
		MatchDirection strToMatchDirection(std::string) const;
		bool parse_matchDir_ID_Protein(std::string);
		
	public:
		Scan() : scanData::Scan(){
			_parentProtein = "";
			_parentID = "";
			_matchDirection = MatchDirection::REVERSE;
		}
		
		//modifers
		void operator = (const Scan&);
		
		//properties
		std::string getParentProtein() const{
			return _parentProtein;
		}
		std::string getParentID() const{
			return _parentID;
		}
		MatchDirection getMatchDirection() const{
			return _matchDirection;
		}
	};
}

#endif /* dtafilter_hpp */
