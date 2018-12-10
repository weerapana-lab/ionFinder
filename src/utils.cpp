//
//  utils.cpp
//  costom general utils library
//
//  Created by Aaron Maurais on 8/31/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <utils.hpp>


/*############# File ###############*/

bool utils::File::read(std::string _fname)
{
	fname = _fname;
	return read();
}

bool utils::File::read()
{
	if(fname.empty())
		throw std::runtime_error("File must be specified!");
	
	std::ifstream inF(fname.c_str());
	
	if(!inF)
		return false;
	
	delimType = detectLineEnding(inF);
	if(delimType == unknown)
		throw std::runtime_error("Invalid delimiter in file: " + fname + "!");
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

std::string utils::File::getLine()
{
	std::string ret;
	utils::getLine(ss, ret, delim, beginLine);
	return ret;
}

std::string utils::File::getLine_skip()
{
	std::string ret;
	do{
		utils::getLine(ss, ret, delim, beginLine);
	} while ((isCommentLine(ret) || ret.empty()) && ss);
	return ret;
}

std::string utils::File::getLine_trim()
{
	std::string ret;
	utils::getLineTrim(ss, ret, delim, beginLine);
	return ret;
}

std::string utils::File::getLine_skip_trim()
{
	std::string ret;
	do{
		utils::getLineTrim(ss, ret, delim, beginLine);
	} while ((isCommentLine(ret) || ret.empty()) && ss);
	return ret;
}

std::string utils::File::getLine_trim_skip()
{
	return getLine_skip_trim();
}

/*############# functions ###############*/

/*******************/
/*  file utilities */
/*******************/

utils::newline_type utils::detectLineEnding_killStream(std::ifstream& inF) {
	char tmp;
	while(inF){
		inF.get(tmp);
		if(tmp == inF.widen('\r')) {	// old Mac or Windows
				inF.get(tmp);
			if(tmp == inF.widen('\n'))	// Windows
				return utils::crlf;
			return utils::cr;	// old Macs
		}
		if(tmp == inF.widen('\n'))	// Unix and modern Macs
			return lf;
	}
	return unknown;
}

utils::newline_type utils::detectLineEnding(std::ifstream& inF)
{
	if(!inF)
		throw std::runtime_error("Bad file stream!");
	const std::streampos p = inF.tellg();
	const utils::newline_type ret = detectLineEnding_killStream(inF);
	inF.seekg(p);
	return ret;
}

char utils::getDelim(utils::newline_type type)
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
		default : throw std::runtime_error("Invalid type!");
			break;
	}
}

std::string utils::getDelimStr(utils::newline_type type)
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
		default : throw std::runtime_error("Invalid type!");
			break;
	}
}

//returns true if folder at end of path exists and false if it does not
bool utils::dirExists (const char* path)
{
	struct stat buffer;
	return stat(path, &buffer) == 0 && S_ISDIR(buffer.st_mode);
}

//returns true if file at end of path exists and false if it does not
bool utils::fileExists(const char* path)
{
	struct stat buffer;
	return stat(path, &buffer) == 0;
}

bool utils::fileExists(std::string path)
{
	return fileExists(path.c_str());
}

bool utils::dirExists(std::string path)
{
	return dirExists(path.c_str());
}

//returns dirrectory from which program is run
std::string utils::pwd()
{
	char temp[PATH_MAX + 1];
	return (getcwd(temp, PATH_MAX) ? std::string(temp) : std::string(""));
}

//resolves relative and symbolic file references
std::string utils::absPath(const char* _fname)
{
	char fbuff [PATH_MAX + 1];
	realpath(_fname, fbuff);
	return std::string(fbuff);
}

//resolves relative and symbolic file references
std::string utils::absPath(std::string _fname)
{
	return(absPath(_fname.c_str()));
}

bool utils::ls(const char* path, std::vector<std::string>& files)
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

bool utils::ls(const char* path, std::vector<std::string>& files, std::string extension)
{
	files.clear();
	std::vector<std::string> allFiles;
	if(!ls(path, allFiles))
		return false;
	
	for(std::vector<std::string>::iterator it = allFiles.begin(); it != allFiles.end(); ++it)
		if(endsWith(*it, extension))
			files.push_back(*it);
	return true;
}

//exicutes std::string arg as bash command
void utils::systemCommand(std::string command)
{
	system(command.c_str());
}

//make dir.
//returns true if sucessful
bool utils::mkdir(const char* path)
{
	//get abs path and make sure that it dosen't already exist
	//return false if it does
	std::string rpath = absPath(path);
	if(dirExists(rpath))
	return false;
	
	//make sure that parent dir exists
	//return false if not
	size_t pos = rpath.find_last_of("/");
	assert(pos != std::string::npos);
	std::string parentDir = rpath.substr(0, pos);
	if(!dirExists(parentDir))
	return false;
	
	//make dir
	systemCommand("mkdir " + rpath);
	
	//test that new dir exists
	return dirExists(rpath);
}

std::string utils::baseName(std::string path, const std::string& delims)
{
	if(path[path.length() - 1] == '/')
		path = path.substr(0, path.length() - 1);
	return path.substr(path.find_last_of(delims) + 1);
}

std::string utils::removeExtension(const std::string& filename)
{
	typename std::string::size_type const p(filename.find_last_of('.'));
	return p > 0 && p != std::string::npos ? filename.substr(0, p) : filename;
}

std::string utils::getExtension(const std::string& filename)
{
	typename std::string::size_type const p(filename.find_last_of('.'));
	return p > 0 && p != std::string::npos ? filename.substr(p) : filename;
}

/*****************/
/* std::string utils */
/*****************/

//returns true if findTxt is found in whithinTxt and false if it it not
bool utils::strContains(std::string findTxt, std::string whithinTxt)
{
	return whithinTxt.find(findTxt) != std::string::npos;
}

//overloaded version of strContains, handels findTxt as char
bool utils::strContains(char findTxt, std::string whithinTxt)
{
	return strContains(std::string(1, findTxt), whithinTxt);
}

bool utils::startsWith(std::string whithinStr, std::string findStr)
{
	return (whithinStr.find(findStr) == 0);
}

bool utils::endsWith(std::string whithinStr, std::string findStr)
{
	size_t pos = whithinStr.rfind(findStr);
	if(pos == std::string::npos)
	return false;
	
	return (whithinStr.substr(pos) == findStr);
}

//split str by delim and populate each split into elems
void utils::split (const std::string& str, const char delim, std::vector<std::string>& elems)
{
	elems.clear();
	std::stringstream ss (str);
	std::string item;
	
	while(getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

//remove trailing WHITESPACE
std::string utils::trimTraling(const std::string& str)
{
	if(str.empty())
	return "";
	return str.substr(0, str.find_last_not_of(WHITESPACE) + 1);
}

//remove leading WHITESPACE
std::string utils::trimLeading(const std::string& str)
{
	if(str.empty())
	return "";
	return str.substr(str.find_first_not_of(WHITESPACE));
}

//remove trailing and leading WHITESPACE
std::string utils::trim(const std::string& str)
{
	if(str.empty())
	return "";
	return trimLeading(trimTraling(str));
}

void utils::trimAll(std::vector<std::string>& elems)
{
	for(std::vector<std::string>::iterator it = elems.begin(); it != elems.end(); ++it)
		(*it) = trim((*it));
}

//returns true if line begins with COMMENT_SYMBOL, ignoring leading whitespace
bool utils::isCommentLine(std::string line)
{
	line = trimLeading(line);
	return line.substr(0, COMMENT_SYMBOL.length()) == COMMENT_SYMBOL;
}

//gets new line from is and removes trailing and leading whitespace
void utils::getLineTrim(std::istream& is, std::string& line, char delim, size_t beginLine)
{
	utils::getLine(is, line, delim, beginLine);
	line = trim(line);
}

//gets new line from is and handels non zero start to line
void utils::getLine(std::istream& is, std::string& line, char delim, size_t beginLine)
{
	getline(is, line, delim);
	if(beginLine > 0)
	line = line.substr(beginLine);
}

//removes findStr from whithinStr and returns whithinStr
std::string utils::removeSubstr(std::string findStr, std::string whithinStr)
{
	std::string::size_type i = whithinStr.find(findStr);
	
	if(i !=std::string::npos)
	whithinStr.erase(i, findStr.length());
	
	return whithinStr;
}

std::string utils::removeChars(char findChar, std::string wStr)
{
	wStr.erase(remove(wStr.begin(), wStr.end(), findChar), wStr.end());
	return wStr;
}

std::string utils::toLower(std::string str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

std::string utils::repeat(std::string str, size_t numTimes)
{
	std::string ret = "";
	assert(!str.empty());
	for(int i = 0; i < numTimes; i++)
	ret += str;
	return ret;
}

//Find std::string 'str' in data buffer 'buf' of length 'len'.
size_t utils::offset(const char* buf, size_t len, const char* str)
{
	return std::search(buf, buf + len, str, str + strlen(str)) - buf;
}

/*********/
/* other */
/*********/

bool utils::isFlag(const char* tok)
{
	if(tok == nullptr)
	return false;
	else return tok[0] == '-';
}

bool utils::isArg(const char* tok)
{
	if(tok == nullptr)
	return false;
	else return !isFlag(tok);
}

std::string utils::ascTime()
{
	//get current time
	const char* curTime;
	time_t rawTime;
	struct tm * timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	curTime = asctime(timeInfo);
	return(std::string(curTime));
}

//template<typename _Tp>
bool utils::inRange(double value, double compare, double range)
{
	return abs(value - compare) <= range;
}

bool utils::isInteger(const std::string & s)
{
	if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;
	
	char * p ;
	strtol(s.c_str(), &p, 10) ;
	
	return (*p == 0) ;
}

//get int from std::cin between min and max
//continue asking for input untill user suplies valid value
int utils::getInt(int min, int max)
{
	std::string choice;
	int ret = min - 1;
	bool good;
	do{
		choice.clear();
		std::cin.sync();
		std::getline(std::cin, choice);
		choice = utils::trim(choice);
		if(utils::isInteger(choice))
		{
			ret = std::stoi(choice);
			if(ret >= min && ret <= max){
				good = true;
			}
			else{
				good = false;
				std::cout << "Invalid choice!" << std::endl << "Enter choice: ";
			}
		}
		else{
			good = false;
			std::cout << "Invalid choice!" << std::endl << "Enter choice: ";
		}
	} while(!good);
	return ret;
}//end of fxn



