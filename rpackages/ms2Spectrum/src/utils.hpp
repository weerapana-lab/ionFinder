
#include <sstream>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <cassert>
#include <cctype>
#include <stdexcept>
#include <cstdlib>

#ifndef utils_hpp
#define utils_hpp

#ifndef IN_DELIM
#define IN_DELIM '\t'
#endif

namespace utils{

  std::string const SUBSCRIPT_MAP [] = {"\u2080", "\u2081", "\u2082", "\u2083", "\u2084", "\u2085", "\u2086", "\u2087", "\u2088", "\u2089"};
  std::string const WHITESPACE = " \f\n\r\t\v\b";
  std::string const COMMENT_SYMBOL = "#";

  std::istream& safeGetline(std::istream&, std::string&);
  void split (const std::string&, const char, std::vector<std::string>&);
  std::string trimTraling(const std::string&);
  std::string trimLeading(const std::string&);
  std::string trim(const std::string&);
  const char* trim(const char* str);
  void trimAll(std::vector<std::string>&);
  bool isCommentLine(std::string);

  std::string getSubscriptNum(int);
}

#endif
