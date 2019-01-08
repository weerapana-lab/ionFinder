//
//  dtafilterFiles.hpp
//  citFinder
//
//  Created by Aaron Maurais on 1/2/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#ifndef dtafilterFiles_hpp
#define dtafilterFiles_hpp

#include <dtafilter.hpp>
#include <ionFinder/params.hpp>
#include <scanData.hpp>
#include <utils.hpp>

namespace Dtafilter{
	bool readFilterFiles(const IonFinder::Params&,
						 std::vector<Dtafilter::Scan>&);
}

#endif /* dtafilterFile_hpp */
