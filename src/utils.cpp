//
//  utils.cpp
//  costom general utils library
//
//  Created by Aaron Maurais on 8/31/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "../include/utils.hpp"

namespace utils{
	
	/*############# File ###############*/
	
	bool File::read(string _fname)
	{
		fname = _fname;
		return read();
	}
	
	bool File::read()
	{
		if(fname.empty())
			throw runtime_error("File must be specified!");
		
		ifstream inF(fname.c_str());
		
		if(!inF)
			return false;
		
		delimType = detectLineEnding(inF);
		if(delimType == unknown)
			throw runtime_error("Invalid delimiter in file: " + fname + "!");
		delim = utils::getDelim(delimType);
		
		inF.seekg(0, inF.end);
		const long size = inF.tellg();
		inF.seekg(0, inF.beg);
		buffer = (char*) calloc(size, sizeof(char));
		
		if(inF.read(buffer, size))
		{
			ss << buffer;
			if(delimType == crlf)
				beginLine = 1;
			else beginLine = 0;
			free(buffer);
			slen = ss.str().length();
			return true;
		} else {
			free(buffer);
			return false;
		}
	}
	
	inline string File::getLine()
	{
		string ret;
		utils::getLine(ss, ret, delim, beginLine);
		return ret;
	}
	
	inline string File::getLine_skip()
	{
		string ret;
		do{
			utils::getLine(ss, ret, delim, beginLine);
		} while ((isCommentLine(ret) || ret.empty()) && ss);
		return ret;
	}
	
	inline string File::getLine_trim()
	{
		string ret;
		utils::getLineTrim(ss, ret, delim, beginLine);
		return ret;
	}
	
	inline string File::getLine_skip_trim()
	{
		string ret;
		do{
			utils::getLineTrim(ss, ret, delim, beginLine);
		} while ((isCommentLine(ret) || ret.empty()) && ss);
		return ret;
	}
	
	inline string File::getLine_trim_skip()
	{
		return getLine_skip_trim();
	}
	
	/*############# functions ###############*/
	
	/*******************/
	/*  file utilities */
	/*******************/
	
	newline_type detectLineEnding_killStream(ifstream& inF) {
		char tmp;
		while(inF){
			inF.get(tmp);
			if(tmp == inF.widen('\r')) {	// old Mac or Windows
					inF.get(tmp);
				if(tmp == inF.widen('\n'))	// Windows
					return crlf;
				return cr;	// old Macs
			}
			if(tmp == inF.widen('\n'))	// Unix and modern Macs
				return lf;
		}
		return unknown;
	}
	
	newline_type detectLineEnding(ifstream& inF)
	{
		if(!inF)
			throw runtime_error("Bad file stream!");
		const streampos p = inF.tellg();
		const newline_type ret = detectLineEnding_killStream(inF);
		inF.seekg(p);
		return ret;
	}
	
	char getDelim(newline_type type)
	{
		switch(type){
			case lf : return '\n';
				break;
			case crlf : return '\r';
				break;
			case cr : return '\r';
				break;
			case unknown : return '\n';
				break;
			default : throw runtime_error("Invalid type!");
				break;
		}
	}
	
	string getDelimStr(newline_type type)
	{
		switch(type){
				case lf : return "\n";
					break;
				case crlf : return "\r\n";
					break;
				case cr : return "\r";
					break;
				case unknown : return "\n";
					break;
			default : throw runtime_error("Invalid type!");
				break;
		}
	}
	
	//returns true if folder at end of path exists and false if it does not
	bool dirExists (const char* path)
	{
		struct stat buffer;
		return stat(path, &buffer) == 0 && S_ISDIR(buffer.st_mode);
	}
	
	//returns true if file at end of path exists and false if it does not
	bool fileExists(const char* path)
	{
		struct stat buffer;
		return stat(path, &buffer) == 0;
	}
	
	bool fileExists(string path)
	{
		return fileExists(path.c_str());
	}
	
	bool dirExists(string path)
	{
		return dirExists(path.c_str());
	}
	
	//returns dirrectory from which program is run
	string pwd()
	{
		char temp[PATH_MAX + 1];
		return (getcwd(temp, PATH_MAX) ? string(temp) : string(""));
	}
	
	//resolves relative and symbolic file references
	string absPath(const char* _fname)
	{
		char fbuff [PATH_MAX + 1];
		realpath(_fname, fbuff);
		return string(fbuff);
	}
	
	//resolves relative and symbolic file references
	string absPath(string _fname)
	{
		return(absPath(_fname.c_str()));
	}
	
	bool ls(const char* path, vector<string>& files)
	{
		files.clear();
		DIR* dirFile = opendir(path);
		if(!dirFile)
		return false;
		else{
			struct dirent* hFile;
			while((hFile = readdir(dirFile)))
			{
				//skip . and ..
				if(!strcmp(hFile->d_name, ".") || !strcmp(hFile->d_name, ".."))
				continue;
				
				//skip hidden files
				if(IGNORE_HIDDEN_FILES && (hFile->d_name[0] == '.'))
				continue;
				
				//add to files
				files.push_back(hFile->d_name);
			}
			closedir(dirFile);
		}
		return true;
	}
	
	bool ls(const char* path, vector<string>& files, string extension)
	{
		files.clear();
		vector<string> allFiles;
		if(!ls(path, allFiles))
		return false;
		
		for(vector<string>::iterator it = allFiles.begin(); it != allFiles.end(); ++it)
		if(endsWith(*it, extension))
		files.push_back(*it);
		return true;
	}
	
	//exicutes string arg as bash command
	void systemCommand(string command)
	{
		system(command.c_str());
	}
	
	//make dir.
	//returns true if sucessful
	bool mkdir(const char* path)
	{
		//get abs path and make sure that it dosen't already exist
		//return false if it does
		string rpath = absPath(path);
		if(dirExists(rpath))
		return false;
		
		//make sure that parent dir exists
		//return false if not
		size_t pos = rpath.find_last_of("/");
		assert(pos != string::npos);
		string parentDir = rpath.substr(0, pos);
		if(!dirExists(parentDir))
		return false;
		
		//make dir
		systemCommand("mkdir " + rpath);
		
		//test that new dir exists
		return dirExists(rpath);
	}
	
	template<class _Tp>
	_Tp baseName(const _Tp& path, const _Tp& delims)
	{
		return path.substr(path.find_last_of(delims) + 1);
	}
	template<class _Tp>
	_Tp removeExtension(const _Tp& filename)
	{
		typename _Tp::size_type const p(filename.find_last_of('.'));
		return p > 0 && p != _Tp::npos ? filename.substr(0, p) : filename;
	}
	template<class _Tp>
	_Tp getExtension(const _Tp& filename)
	{
		typename _Tp::size_type const p(filename.find_last_of('.'));
		return p > 0 && p != _Tp::npos ? filename.substr(p) : filename;
	}
	
	/*********************/
	/*  type conversions */
	/*********************/
	
	//converts num to string because to_string does not work with stl 98
	template <typename _Tp>
	inline string toString(_Tp num)
	{
		string str;
		stringstream convert;
		
		convert << num;
		convert >> str;
		
		return str;
	}
	
	//converts string to int because atoi does not work with stl 98
	//Pre: str must be a string with a valid interger conversion
	inline int toInt(string str)
	{
		assert(isInteger(str));
		
		int num;
		stringstream convert;
		
		convert << str;
		convert >> num;
		
		return num;
	}
	
	//return true if str can be converted to an int
	bool isInteger(string str)
	{
		if(str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+')))
		return false ;
		
		char * p ;
		strtol(str.c_str(), &p, 10) ;
		
		return (*p == 0) ;
	}
	
	//return true if str can be converted to a double
	bool isDouble(string str)
	{
		if(str.empty() || (!isdigit(str[0]) && (str[0] != '-') && (str[0] != '+') && (str[0] != '.')))
		   return false ;
		
		char * p ;
		strtod(str.c_str(), &p);
		
		return !(*p != '\0' || p == str);
	}
	
	//converts string to int because stod does not work with some c++ compilers
	//Precondition: str must be a string with a valid double conversion
	double toDouble(string str)
	{
		assert(isDouble(str));
		double num;
		stringstream convert;
		
		convert << str;
		convert >> num;
		
		return num;
	}
	
	/*****************/
	/*  string utils */
	/*****************/
	
	//returns true if findTxt is found in whithinTxt and false if it it not
	inline bool strContains(string findTxt, string whithinTxt)
	{
		return whithinTxt.find(findTxt) != string::npos;
	}
	
	//overloaded version of strContains, handels findTxt as char
	inline bool strContains(char findTxt, string whithinTxt)
	{
		return strContains(string(1, findTxt), whithinTxt);
	}
	
	inline bool startsWith(string whithinStr, string findStr)
	{
		return (whithinStr.find(findStr) == 0);
	}
	
	inline bool endsWith(string whithinStr, string findStr)
	{
		size_t pos = whithinStr.rfind(findStr);
		if(pos == string::npos)
		return false;
		
		return (whithinStr.substr(pos) == findStr);
	}
	
	//split str by delim and populate each split into elems
	inline void split (const string& str, const char delim, vector<string>& elems)
	{
		elems.clear();
		stringstream ss (str);
		string item;
		
		while(getline(ss, item, delim)) {
			elems.push_back(item);
		}
	}
	
	//remove trailing WHITESPACE
	string trimTraling(const string& str)
	{
		if(str.empty())
		return "";
		return str.substr(0, str.find_last_not_of(WHITESPACE) + 1);
	}
	
	//remove leading WHITESPACE
	string trimLeading(const string& str)
	{
		if(str.empty())
		return "";
		return str.substr(str.find_first_not_of(WHITESPACE));
	}
	
	//remove trailing and leading WHITESPACE
	string trim(const string& str)
	{
		if(str.empty())
		return "";
		return trimLeading(trimTraling(str));
	}
	
	void trimAll(vector<string>& elems)
	{
		for(vector<string>::iterator it = elems.begin(); it != elems.end(); ++it)
			(*it) = trim((*it));
	}
	
	//returns true if line begins with COMMENT_SYMBOL, ignoring leading whitespace
	bool isCommentLine(string line)
	{
		line = trimLeading(line);
		return line.substr(0, COMMENT_SYMBOL.length()) == COMMENT_SYMBOL;
	}
	
	//gets new line from is and removes trailing and leading whitespace
	inline void getLineTrim(istream& is, string& line, char delim, size_t beginLine)
	{
		utils::getLine(is, line, delim, beginLine);
		line = trim(line);
	}
	
	//gets new line from is and handels non zero start to line
	inline void getLine(istream& is, string& line, char delim, size_t beginLine)
	{
		getline(is, line, delim);
		if(beginLine > 0)
		line = line.substr(beginLine);
	}
	
	//removes findStr from whithinStr and returns whithinStr
	inline string removeSubstr(string findStr, string whithinStr)
	{
		string::size_type i = whithinStr.find(findStr);
		
		if(i != string::npos)
		whithinStr.erase(i, findStr.length());
		
		return whithinStr;
	}
	
	inline string removeChars(char findChar, string wStr)
	{
		wStr.erase(remove(wStr.begin(), wStr.end(), findChar), wStr.end());
		return wStr;
	}
	
	string toLower(string str)
	{
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}
	
	string repeat(string str, size_t numTimes)
	{
		string ret = "";
		assert(!str.empty());
		for(int i = 0; i < numTimes; i++)
		ret += str;
		return ret;
	}
	
	//Find string 'str' in data buffer 'buf' of length 'len'.
	size_t offset(const char* buf, size_t len, const char* str)
	{
		return std::search(buf, buf + len, str, str + strlen(str)) - buf;
	}
	
	/*********/
	/* other */
	/*********/
	
	bool isFlag(const char* tok)
	{
		if(tok == nullptr)
		return false;
		else return tok[0] == '-';
	}
	
	bool isArg(const char* tok)
	{
		if(tok == nullptr)
		return false;
		else return !isFlag(tok);
	}
	
	string ascTime()
	{
		//get current time
		const char* curTime;
		time_t rawTime;
		struct tm * timeInfo;
		time(&rawTime);
		timeInfo = localtime(&rawTime);
		curTime = asctime(timeInfo);
		return(string(curTime));
	}
	
	template <typename _Tp>
	int round(_Tp num)
	{
		return floor(num + 0.5);
	}
	
	template <typename _Tp>
	_Tp numDigits (_Tp num)
	{
		int count = 0;
		while (num != 0) {
			count++;
			num /= 10;
		}
		return count;
	}
	
	template<typename _Tp>
	bool inRange(_Tp value, _Tp compare, _Tp range)
	{
		return abs(value - compare) <= range;
	}
}



