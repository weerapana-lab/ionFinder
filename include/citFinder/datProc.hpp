//
//  datProc.hpp
//  citFinder
//
//  Created by Aaron Maurais on 12/10/18.
//  Copyright © 2018 Aaron Maurais. All rights reserved.
//

#ifndef datProc_hpp
#define datProc_hpp

#include <map>
#include <vector>
#include <iostream>

#include <citFinder/citFinder.hpp>
#include <citFinder/params.hpp>
#include <dtafilter.hpp>

#include <peptide.hpp>
#include <ms2.hpp>

namespace CitFinder{

	class AminoAcid;
	class Peptide;
	
	bool findFragments(std::vector<PeptideNamespace::Peptide>&,
					   const std::vector<Dtafilter::Scan>&,
					   CitFinder::Params&);
	
	///Class used to keep track of fragment ions which were found in sequence analysis.
	class AminoAcid : public PeptideNamespace::FragmentIon{
	private:
		
	public:
		
	};
}

#endif /* datProc_hpp */
