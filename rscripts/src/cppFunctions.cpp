
#include <Rcpp.h>
#include <sstream>
#include <cassert>
#include <cctype>
#include <stdexcept>

using namespace Rcpp;
//using namespace std;

std::string const SUBSCRIPT_MAP [] = {"\u2080", "\u2081", "\u2082", "\u2083", "\u2084", "\u2085", "\u2086", "\u2087", "\u2088", "\u2089"};
const char* DIFFMODS = "*";

bool isInteger(std::string);
template <typename _Tp> std::string toString(_Tp);
int toInt(std::string);
int toInt(char);
StringVector makeIonLables(std::string, int, char);
StringVector makeIonLabelsExp(std::string, int, char);
std::string getSubscriptNum(std::string);
std::string getSubscriptNum(int);
bool isArg(std::string);
std::string fixOD(std::string);

//return true if str can be converted to an int
bool isInteger(std::string str)
{
  if(str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+')))
    return false ;
  
  char * p ;
  strtol(str.c_str(), &p, 10) ;
  
  return (*p == 0) ;
}

//converts num to std::string because to_string does not work with stl 98
template <typename _Tp>
std::string toString(_Tp num)
{
  std::string str;
  std::stringstream convert;
  
  convert << num;
  convert >> str;
  
  return str;
}

//converts std::string to int because atoi does not work with stl 98
//Pre: str must be a std::string with a valid interger conversion
int toInt(std::string str)
{
  assert(isInteger(str));
  
  int num;
  std::stringstream convert;
  
  convert << str;
  convert >> num;
  
  return num;
}

int toInt(char cha)
{
  return toInt(std::string(1, cha));
}

// [[Rcpp::export]]
std::string getSubscriptNum(std::string str)
{
  return getSubscriptNum(toInt(str));
}

std::string getSubscriptNum(int num)
{
  std::string ret = "";
  std::string num_str = toString(num);
  for(int i = 0; i < num_str.length(); i++)
    ret += SUBSCRIPT_MAP[toInt(num_str[i])];
  return ret;
}

// [[Rcpp::export]]
StringVector makeIonLabels(std::string letter, int count, char dir)
{
  StringVector ret;
  if(dir == 'f')
  {
    for(int i = 1; i <= count; i++)
      ret.push_back(letter + toString(i));
  }
  else if(dir == 'r')
  {
    for(int i = count; i > 0; i--)
      ret.push_back(letter + toString(i));
  }
  else throw std::runtime_error("Invalid choice for dir. Only f or r allowed.");
  return ret;
}

// [[Rcpp::export]]
StringVector makeIonLabelsExp(std::string letter, int count, char dir)
{
  StringVector ret;
  if(dir == 'f')
  {
    for(int i = 1; i <= count; i++)
      ret.push_back(letter + "[" + toString(i) + "]");
  }
  else if(dir == 'r')
  {
    for(int i = count; i > 0; i--)
      ret.push_back(letter + "[" + toString(i) + "]");
  }
  else throw std::runtime_error("Invalid choice for dir. Only f or r allowed.");
  return ret;
}

// [[Rcpp::export]]
StringVector makeSeqVec(std::string seq, bool fixDiffMods = true)
{
  bool modFound = false;
  char mod = '\0';
  StringVector ret;
  
  for(size_t i = 0; i < seq.length(); i++)
  {
    if(fixDiffMods)
    {
      if((i + 1) < seq.length())
      {
        //iterate through diffmods
        for(const char* p = DIFFMODS; *p; p++)
        {
          //check if next char in sequence is diff mod char
          if(seq[i + 1] == *p)
          {
            modFound = true;
            mod = seq[i + 1];
            seq.erase(i + 1, 1);
            break;
          }//end of if
        }//end of for
      }//end of if
    }//end if
    if(modFound)
    {
      ret.push_back(seq[i] + std::string(1, mod));
      modFound = false;
    }
    else ret.push_back(seq[i]);
  }//end of for
  return ret;
}

// [[Rcpp::export]]
bool isArg(std::string arg)
{
  return !(arg[0] == '-');
}

// [[Rcpp::export]]
std::string fixOD(std::string str)
{
  if(str[str.length() - 1] != '/')
    str += '/';
  return str;
}

// [[Rcpp::export]]
Rcpp::CharacterVector getFoundIons(Rcpp::CharacterVector ionTypes, Rcpp::CharacterVector ionNums)
{
  Rcpp::CharacterVector ret;
  
  size_t len = ionTypes.size();
  if(len != ionNums.size())
    throw std::runtime_error("ionTypes and ionNums must be the same length!");
  
  for(size_t i = 0; i < len; i++)
    ret.push_back(std::string(1, ionTypes[i][0]) + std::string(ionNums[i]));
  
  
  return ret;
}


