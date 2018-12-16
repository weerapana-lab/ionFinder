
#include <Rcpp.h>
#include <sstream>
#include <cassert>
#include <stdexcept>

using namespace Rcpp;
using namespace std;

string const SUBSCRIPT_MAP [] = {"\u2080", "\u2081", "\u2082", "\u2083", "\u2084", "\u2085", "\u2086", "\u2087", "\u2088", "\u2089"};
const char* DIFFMODS = "*";

bool isInteger(string);
template <typename _Tp> string toString(_Tp);
int toInt(string);
int toInt(char);
StringVector makeIonLables(string, int, char);
StringVector makeIonLabelsExp(string, int, char);
string getSubscriptNum(string);
string getSubscriptNum(int);
bool isArg(string);
string fixOD(string);

//return true if str can be converted to an int
bool isInteger(string str)
{
  if(str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+')))
    return false ;
  
  char * p ;
  strtol(str.c_str(), &p, 10) ;
  
  return (*p == 0) ;
}

//converts num to string because to_string does not work with stl 98
template <typename _Tp>
string toString(_Tp num)
{
  string str;
  stringstream convert;
  
  convert << num;
  convert >> str;
  
  return str;
}

//converts string to int because atoi does not work with stl 98
//Pre: str must be a string with a valid interger conversion
int toInt(string str)
{
  assert(isInteger(str));
  
  int num;
  stringstream convert;
  
  convert << str;
  convert >> num;
  
  return num;
}

int toInt(char cha)
{
  return toInt(string(1, cha));
}

// [[Rcpp::export]]
string getSubscriptNum(string str)
{
  return getSubscriptNum(toInt(str));
}

string getSubscriptNum(int num)
{
  string ret = "";
  string num_str = toString(num);
  for(int i = 0; i < num_str.length(); i++)
    ret += SUBSCRIPT_MAP[toInt(num_str[i])];
  return ret;
}

// [[Rcpp::export]]
StringVector makeIonLabels(string letter, int count, char dir)
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
  else throw runtime_error("Invalid choice for dir. Only f or r allowed.");
  return ret;
}

// [[Rcpp::export]]
StringVector makeIonLabelsExp(string letter, int count, char dir)
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
  else throw runtime_error("Invalid choice for dir. Only f or r allowed.");
  return ret;
}

// [[Rcpp::export]]
StringVector makeSeqVec(string seq, bool fixDiffMods = true)
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
      ret.push_back(seq[i] + string(1, mod));
      modFound = false;
    }
    else ret.push_back(seq[i]);
  }//end of for
  return ret;
}

// [[Rcpp::export]]
bool isArg(string arg)
{
  return !(arg[0] == '-');
}

// [[Rcpp::export]]
string fixOD(string str)
{
  if(str[str.length() - 1] != '/')
    str += '/';
  return str;
}


