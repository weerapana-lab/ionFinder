//
//  ms2_annotator.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/17/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef ms2_annotator_hpp
#define ms2_annotator_hpp

//deal with older c++ compilers
#if (__cplusplus == 199711L || __cplusplus == 1)
	#define nullptr NULL
#endif

//make sure this value is defined
#ifndef PATH_MAX
	#define PATH_MAX 1024
#endif

#ifndef BIN_VERSION
	#define BIN_VERSION 1.0
#endif

#ifndef OUT_DELIM
#define OUT_DELIM '\t'
#endif

#ifndef IN_DELIM
#define IN_DELIM '\t'
#endif

#ifndef NEW_LINE
#define NEW_LINE '\n'
#endif

#include <iostream>

#endif /* ms2_annotator_hpp */
