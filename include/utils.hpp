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
	enum newline_type {lf, crlf, cr, unknown};
	char const DEFAULT_LINE_DELIM = '\n';
	size_t const DEFAULT_BEGIN_LINE = 0;
	bool const IGNORE_HIDDEN_FILES = true; //ignore hidden files in utils::ls
	
	/******************************/
	/*     class definitions     */
	/*****************************/
	
	class File;
	
	//file class for reading in text files line by line
	//automatically detects and handles line return characters from different operating systems
	class File{
	private:
		char* buffer;
		char delim;
		newline_type delimType;
		std::string fname;
		size_t beginLine;
		std::stringstream ss;
		unsigned long slen;
	public:
		File(){
			buffer = nullptr;
			slen = 0;
		}
		File(std::string str){
			buffer = nullptr;
			fname = str;
			slen = 0;
		}
		~File(){}
		
		//modifers
		bool read(std::string);
		bool read();
		std::string getLine();
		std::string getLine_skip();
		std::string getLine_trim();
		std::string getLine_skip_trim();
		std::string getLine_trim_skip();
		
		//properties
		bool end(){
			return (ss.tellg() >= slen);
		}
		std::string getFname() const{
			return fname;
		}
		char getDelim() const{
			return delim;
		}
		newline_type getNewLineType() const{
			return delimType;
		}
	};
	
	/*************/
	/* functions */
	/*************/
	
	//file utils
	char getDelim(newline_type);
	std::string getDelimStr(newline_type);
	newline_type detectLineEnding_killStream(std::ifstream&);
	newline_type detectLineEnding(std::ifstream&);
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
	std::string removeExtension(const std::string&);
	std::string getExtension(const std::string&);
		
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
	void getLineTrim(std::istream& is,std::string& line,
							char delim = DEFAULT_LINE_DELIM, size_t beginLine = DEFAULT_BEGIN_LINE);
	void getLine(std::istream& is,std::string& line,
						char delim = DEFAULT_LINE_DELIM, size_t beginLine = DEFAULT_BEGIN_LINE);
	void getLine(const char* buffer,std::string& line, char delim = DEFAULT_LINE_DELIM);
	size_t offset(const char* buf, size_t len, const char* str);
	
	
	//other
	bool isInteger(const std::string & s);
	bool isFlag(const char*);
	bool isArg(const char*);
	std::string ascTime();
	template <typename _Tp> int round(_Tp num){
		return floor(num + 0.5);
	}
	template <typename _Tp> _Tp numDigits(_Tp num){
		int count = 0;
		while (num != 0) {
			count++;
			num /= 10;
		}
		return count;
	}
	template <typename _Tp, size_t N> _Tp* begin(_Tp(&arr)[N]) {
		return &arr[0];
	}
	template <typename _Tp, size_t N> _Tp* end(_Tp(&arr)[N]) {
		return &arr[0]+N;
	}
	//template<typename _Tp> bool inRange(_Tp, _Tp, _Tp);
	bool inRange(double, double, double);
	int getInt(int min, int max);
}

#endif /* utils_hpp */

