//
//  utils.hpp
//  costom general utils library
//
//  Created by Aaron Maurais on 8/31/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp

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

#ifndef PATH_MAX
	#define PATH_MAX 1024
#endif

#ifndef IN_DELIM
	#define IN_DELIM '\t'
#endif
#ifndef OUT_DELIM
	#define OUT_DELIM '\t'
#endif

namespace utils{
	
	using namespace std;
	
	/******************************/
	/* namespace scoped constants */
	/*****************************/
	
	string const WHITESPACE = " \f\n\r\t\v";
	string const COMMENT_SYMBOL = "#";
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
		string fname;
		size_t beginLine;
		stringstream ss;
		unsigned long slen;
	public:
		File(){
			buffer = nullptr;
			slen = 0;
		}
		File(string str){
			buffer = nullptr;
			fname = str;
			slen = 0;
		}
		~File(){}
		
		//modifers
		bool read(string);
		bool read();
		inline string getLine();
		inline string getLine_skip();
		inline string getLine_trim();
		inline string getLine_skip_trim();
		inline string getLine_trim_skip();
		
		//properties
		inline bool end(){
			return (ss.tellg() >= slen);
		}
		inline string getFname() const{
			return fname;
		}
		inline char getDelim() const{
			return delim;
		}
		inline newline_type getNewLineType() const{
			return delimType;
		}
	};
	
	/*************/
	/* functions */
	/*************/
	
	//file utils
	char getDelim(newline_type);
	string getDelimStr(newline_type);
	newline_type detectLineEnding_killStream(ifstream&);
	newline_type detectLineEnding(ifstream&);
	bool dirExists(const char*);
	bool dirExists(string);
	bool fileExists(const char*);
	bool fileExists(string);
	string pwd();
	string absPath(string);
	string absPath(const char*);
	bool ls(const char*, vector<string>&);
	bool ls(const char*, vector<string>&, string);
	bool mkdir(string);
	bool mkdir(const char*);
	void systemCommand(string command);
	template<class _Tp> _Tp baseName(const _Tp& path, const _Tp& delims = "/\\");
	template<class _Tp> _Tp removeExtension(const _Tp&);
	template<class _Tp> _Tp getExtension(const _Tp&);
	
	//type conversions
	template <typename _Tp> inline string toString(_Tp);
	inline int toInt(string);
	inline double toDouble(string);
	bool isInteger(string);
	
	//string utils
	inline bool strContains(string, string);
	inline bool strContains(char, string);
	inline bool startsWith(string whithinStr, string findStr);
	inline bool endsWith(string whithinStr, string findStr);
	inline void split (const string&, const char, vector<string>&);
	string trimTraling(const string&);
	string trimLeading(const string&);
	string trim(const string&);
	void trimAll(vector<string>&);
	bool isCommentLine(string);
	inline string removeSubstr(string, string);
	inline string removeChars(char, string);
	string toLower(string);
	string repeat(string, size_t);
	inline void getLineTrim(istream& is, string& line,
							char delim = DEFAULT_LINE_DELIM, size_t beginLine = DEFAULT_BEGIN_LINE);
	inline void getLine(istream& is, string& line,
						char delim = DEFAULT_LINE_DELIM, size_t beginLine = DEFAULT_BEGIN_LINE);
	inline void getLine(const char* buffer, string& line, char delim = DEFAULT_LINE_DELIM);
	size_t offset(const char* buf, size_t len, const char* str);
	
	
	//other
	bool isFlag(const char*);
	bool isArg(const char*);
	string ascTime();
	template <typename _Tp> int round(_Tp);
	template <typename _Tp> _Tp numDigits(_Tp);
	template <typename _Tp, size_t N> _Tp* begin(_Tp(&arr)[N]) {
		return &arr[0];
	}
	template <typename _Tp, size_t N> _Tp* end(_Tp(&arr)[N]) {
		return &arr[0]+N;
	}
	template<typename _Tp> bool inRange(_Tp, _Tp, _Tp);
}

#endif /* utils_hpp */

