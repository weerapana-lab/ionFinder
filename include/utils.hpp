//
//  utils.hpp
//  costom general utils library
//
//  Created by Aaron Maurais on 8/31/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp

#include <iostream>
#include <cassert>
#include <sstream>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <stdexcept>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <ctime>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <string>
#include <cctype>
#include <type_traits>

#ifndef PATH_MAX
	#define PATH_MAX 1024
#endif

#ifndef IN_DELIM
	#define IN_DELIM '\t'
#endif
#ifndef OUT_DELIM
	#define OUT_DELIM '\t'
#endif
#ifndef NEW_LINE
#define NEW_LINE '\n'
#endif

namespace utils{
	
	/******************************/
	/* namespace scoped constants */
	/*****************************/
	
	std::string const WHITESPACE = " \f\n\r\t\v";
	std::string const COMMENT_SYMBOL = "#";
	bool const IGNORE_HIDDEN_FILES = true; //ignore hidden files in utils::ls
	int const PROGRESS_BAR_WIDTH = 60;
	
	/*************/
	/* functions */
	/*************/
	
	//file utils
	void readBuffer(std::string fname, char** buffer, size_t& size);
	bool dirExists(const char*);
	bool dirExists(std::string);
	bool fileExists(const char*);
	bool fileExists(std::string);
	bool isDir(const char*);
	bool isDir(std::string);
	std::string pwd();
	std::string absPath(std::string);
	std::string absPath(const char*);
	bool ls(const char*, std::vector<std::string>&);
	bool ls(const char*, std::vector<std::string>&, std::string);
	bool mkdir(std::string);
	bool mkdir(const char*);
	void systemCommand(std::string command);
	std::string baseName(std::string path, const std::string& delims = "/\\");
	std::string dirName(std::string path, const std::string& delims = "/\\");
	std::string parentDir(std::string path, char delim = '/');
	std::string removeExtension(const std::string&);
	std::string getExtension(const std::string&);
	std::istream& safeGetline(std::istream& is, std::string& t);
	std::istream& safeGetline(std::istream& is, std::string& t, std::streampos& oldPos);
		
	//std::string utils
	bool strContains(std::string, std::string);
	bool strContains(char, std::string);
	bool startsWith(std::string whithinStr, std::string findStr);
	bool endsWith(std::string whithinStr, std::string findStr);
	void split (const std::string&, const char, std::vector<std::string>&);
	std::string trimTraling(const std::string&);
	std::string trimLeading(const std::string&);
	std::string trim(const std::string&);
	void trimAll(std::vector<std::string>&);
	bool isCommentLine(std::string);
	std::string removeSubstr(std::string,std::string);
	std::string removeChars(char,std::string);
	std::string toLower(std::string);
	std::string repeat(std::string, size_t);
	size_t offset(const char* buf, size_t len, const char* str);
	size_t offset(const char* buf, size_t len, std::string s);
	void removeEmptyStrings(std::vector<std::string>&);
	
	//other
	bool isInteger(const std::string & s);
	bool isFlag(const char*);
	bool isArg(const char*);
	void printProgress(float progress, std::ostream& out, int barWidth = PROGRESS_BAR_WIDTH);
	void printProgress(float progress, int barWidth = PROGRESS_BAR_WIDTH);
	std::string ascTime();
	template <typename _Tp> int round(_Tp num){
		return floor(num + 0.5);
	}
	
	/**
	 Get number of digits in an integer.
	 \param num number to count digits of
	 \pre \p num must be an integer type
	 \return number of digits in \p num
	 */
	template <typename _Tp> int numDigits(_Tp num){
		static_assert(std::is_integral<_Tp>::value, "num must be integer");
		int count = 0;
		while (num != 0) {
			count++;
			num /= 10;
		}
		return count;
	}
	
	/**
	 Determine whether compare is between compare - range and compare + range.
	 \param value reference value
	 \param compare value to search for in range
	 \param range the range
	 \return True if compare is in range
	 */
	template<typename _Tp> bool inRange(_Tp value, _Tp compare, _Tp range){
		return abs(value - compare) <= range;
	}
	
	/**
	 Determine whether comp is beg <= comp and comp <= end
	 \param beg beginning of range
	 \param end end of range
	 \param comp the value to search for in range
	 \return True if comp is in range
	 */
	template<typename _Tp> bool inSpan(_Tp beg, _Tp end, _Tp comp){
		return beg <= comp && comp <= end;
	}
	int getInt(int min, int max);
	
	/**
	 Get underlying integer value for enum class
	 \param value enum class value
	 \return integer value of \p value
	 */
	template <typename Enumeration>
	auto as_integer(Enumeration const value)-> typename std::underlying_type<Enumeration>::type
	{
		return static_cast<typename std::underlying_type<Enumeration>::type>(value);
	}
}

#endif /* utils_hpp */

