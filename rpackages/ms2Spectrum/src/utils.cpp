
#include <Rcpp.h>
#include "utils.hpp"

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
  return str.substr(0, str.find_last_not_of(utils::WHITESPACE) + 1);
}

//remove leading WHITESPACE
std::string utils::trimLeading(const std::string& str)
{
  if(str.empty())
    return "";
  return str.substr(str.find_first_not_of(utils::WHITESPACE));
}

//remove trailing and leading WHITESPACE
std::string utils::trim(const std::string& str)
{
  if(str.empty())
    return "";
  return utils::trimLeading(trimTraling(str));
}

const char* utils::trim(const char* str)
{
  return utils::trim(std::string(str)).c_str();
}

void utils::trimAll(std::vector<std::string>& elems)
{
  for(std::vector<std::string>::iterator it = elems.begin(); it != elems.end(); ++it)
    (*it) = trim((*it));
}

//returns true if line begins with COMMENT_SYMBOL, ignoring leading whitespace
bool utils::isCommentLine(std::string line)
{
  line = utils::trimLeading(line);
  return line.substr(0, utils::COMMENT_SYMBOL.length()) == utils::COMMENT_SYMBOL;
}

std::string utils::getSubscriptNum(int num)
{
  std::string ret = "";
  std::string num_str = std::to_string(num);
  for(int i = 0; i < num_str.length(); i++)
    ret += utils::SUBSCRIPT_MAP[int(num_str[i]) - 48];
  return ret;
}

std::istream& utils::safeGetline(std::istream& is, std::string& t)
{
  t.clear();

  // The characters in the stream are read one-by-one using a std::streambuf.
  // That is faster than reading them one-by-one using the std::istream.
  // Code that uses streambuf this way must be guarded by a sentry object.
  // The sentry object performs various tasks,
  // such as thread synchronization and updating the stream state.

  std::istream::sentry se(is, true);
  std::streambuf* sb = is.rdbuf();

  for(;;) {
    int c = sb->sbumpc();
    switch (c) {
    case '\n':
      return is;
    case '\r':
      if(sb->sgetc() == '\n')
        sb->sbumpc();
      return is;
      //case std::streambuf::traits_type::eof():
    case -1:
      // Also handle the case when the last line has no line ending
      if(t.empty())
        is.setstate(std::ios::eofbit);
      return is;
    default:
      t += (char)c;
    }
  }
}
