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
#include <citFinder/dtafilter.hpp>

#include <peptide.hpp>
#include <ms2.hpp>

namespace CitFinder{

	bool calcFragments(std::vector<PeptideNamespace::Peptide>&,
					   const std::vector<CitFinder::Scan>&,
					   CitFinder::Params&);
}

#endif /* datProc_hpp */
