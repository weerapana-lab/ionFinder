
#ifndef spectrum_hpp
#define spectrum_hpp

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
#include <Rcpp.h>

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

//type conversions
template <typename _Tp> inline string toString(_Tp);
inline int toInt(string);
inline double toDouble(string);
bool isInteger(string);

//string utils
inline void split (const string&, const char, vector<string>&);
string trimTraling(const string&);
string trimLeading(const string&);
string trim(const string&);
void trimAll(vector<string>&);
bool isCommentLine(string);
inline void getLineTrim(istream& is, string& line,
                        char delim = DEFAULT_LINE_DELIM, size_t beginLine = DEFAULT_BEGIN_LINE);
inline void getLine(istream& is, string& line,
                    char delim = DEFAULT_LINE_DELIM, size_t beginLine = DEFAULT_BEGIN_LINE);
inline void getLine(const char* buffer, string& line, char delim = DEFAULT_LINE_DELIM);
size_t offset(const char* buf, size_t len, const char* str);
}

std::string const BEGIN_META_DATA = "<metaData>";
std::string const END_META_DATA = "</metaData>";
std::string const BEGIN_SPECTRUM = "<spectrum>";
std::string const END_SPECTRUM = "</spectrum>";

Rcpp::DataFrame getSpectrum(utils::File& file);
Rcpp::List getMetaData(utils::File& file);
Rcpp::List getSpectrum(std::string);

#endif
