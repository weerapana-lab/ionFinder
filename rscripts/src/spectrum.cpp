//
//  spectrum.cpp
//  spectrumReader
//
//  Created by Aaron Maurais on 11/24/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <vector>
#include <sstream>
#include <Rcpp.h>

#define IN_DELIM '\t'

const char* END_LINES = "\r\n";
std::string const WHITESPACE = " \f\n\r\t\v\b";
std::string const COMMENT_SYMBOL = "#";
const char* BEGIN_METADATA = "<metadata>";
const char* END_METADATA = "</metadata>";
const char* BEGIN_SPECTRUM = "<spectrum>";
const char* END_SPECTRUM = "</spectrum>";

std::istream& safeGetline(std::istream&, std::string&);
Rcpp::List getSpectrum(std::string);
inline void split (const std::string&, const char, std::vector<std::string>&);
std::string trimTraling(const std::string&);
std::string trimLeading(const std::string&);
std::string trim(const std::string&);
void trimAll(std::vector<std::string>&);
bool isCommentLine(std::string);
inline int toInt(std::string);
inline double toDouble(std::string);
bool isInteger(std::string);

//split str by delim and populate each split into elems
inline void split (const std::string& str, const char delim, std::vector<std::string>& elems)
{
  elems.clear();
  std::stringstream ss (str);
  std::string item;
  
  while(getline(ss, item, delim)) {
    elems.push_back(item);
  }
}

//remove trailing WHITESPACE
std::string trimTraling(const std::string& str)
{
  if(str.empty())
    return "";
  return str.substr(0, str.find_last_not_of(WHITESPACE) + 1);
}

//remove leading WHITESPACE
std::string trimLeading(const std::string& str)
{
  if(str.empty())
    return "";
  return str.substr(str.find_first_not_of(WHITESPACE));
}

//remove trailing and leading WHITESPACE
std::string trim(const std::string& str)
{
  if(str.empty())
    return "";
  return trimLeading(trimTraling(str));
}

const char* trim(const char* str)
{
  return trim(std::string(str)).c_str();
}

void trimAll(std::vector<std::string>& elems)
{
  for(std::vector<std::string>::iterator it = elems.begin(); it != elems.end(); ++it)
    (*it) = trim((*it));
}

//returns true if line begins with COMMENT_SYMBOL, ignoring leading whitespace
bool isCommentLine(std::string line)
{
  line = trimLeading(line);
  return line.substr(0, COMMENT_SYMBOL.length()) == COMMENT_SYMBOL;
}

//converts std::string to int because atoi does not work with stl 98
//Pre: str must be a std::string with a valid interger conversion
inline int toInt(std::string str)
{
  if(!isInteger(str))
    throw std::runtime_error("!isInteger(str)");
  
  int num;
  std::stringstream convert;
  
  convert << str;
  convert >> num;
  
  return num;
}

//return true if str can be converted to an int
bool isInteger(std::string str)
{
  if(str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+')))
    return false ;
  
  char * p ;
  strtol(str.c_str(), &p, 10) ;
  
  return (*p == 0) ;
}

//return true if str can be converted to a double
bool isDouble(std::string str)
{
  if(str.empty() || (!isdigit(str[0]) && (str[0] != '-') && (str[0] != '+') && (str[0] != '.')))
    return false ;
  
  char * p ;
  strtod(str.c_str(), &p);
  
  return !(*p != '\0' || p == str);
}

//converts std::string to int because stod does not work with some c++ compilers
//Precondition: str must be a std::string with a valid double conversion
double toDouble(std::string str)
{
  if(!(isDouble(str)))
       throw std::runtime_error("!(isDouble(str)");
  double num;
  std::stringstream convert;
  
  convert << str;
  convert >> num;
  
  return num;
}

std::istream& safeGetline(std::istream& is, std::string& t)
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

// [[Rcpp::export]]
Rcpp::List getSpectrum(std::string fname)
{
  std::ifstream inF(fname.c_str());
  if(!inF)
    throw std::runtime_error("Could not read " + fname);
  
  std::string scanNum, parentFile, sequence, fullSequence, precursorCharge;
  Rcpp::NumericVector mz;
  Rcpp::NumericVector intensity;
  Rcpp::CharacterVector label;
  Rcpp::LogicalVector includeLabel;
  Rcpp::CharacterVector ionType;
  Rcpp::CharacterVector ionNum;
  Rcpp::CharacterVector formatedLabel;
  Rcpp::NumericVector labelX;
  Rcpp::NumericVector labelY;
  Rcpp::LogicalVector includeArrow;
  Rcpp::NumericVector arrowBegX;
  Rcpp::NumericVector arrowBegY;
  Rcpp::NumericVector arrowEndX;
  Rcpp::NumericVector arrowEndY;
  
  std::string line;
  std::vector<std::string> elems;
  
  while(!safeGetline(inF, line).eof())
  {
    line = trim(line);
    
    if(line == BEGIN_METADATA)
    {
      while(!safeGetline(inF, line).eof())
      {
        line = trim(line);
        if(line == END_METADATA)
          break;
        
        if(isCommentLine(line))
          continue;
        
        split(line, IN_DELIM, elems);
        trimAll(elems);
        if(elems.size() < 2)
          throw std::runtime_error("bad metadata!");
        if(elems[0] == "scanNumber")
        {
          scanNum = elems[1];
        }
        else if(elems[0] == "precursorFile")
        {
          parentFile = elems[1];
        }
        else if(elems[0] == "sequence")
        {
          sequence = elems[1];
        }
        else if(elems[0] == "fullSequence")
        {
          fullSequence = elems[1];
        }
        else if(elems[0] == "precursorCharge")
        {
          precursorCharge = elems[1];
        }
      }
    }
    else if(line == BEGIN_SPECTRUM)
    {
      while(!safeGetline(inF, line).eof())
      {
        line = trim(line);
        if(line == END_SPECTRUM)
          break;
        
        if(isCommentLine(line) || line.empty())
          continue;
       
       
        
        split(line, IN_DELIM, elems);
        trimAll(elems);
        if(elems[0] == "mz") //if header line, continue
        {
          continue;
        }
        else {
          if(elems.size() != 14)
            throw std::runtime_error("elems.size() != 13");
          
          mz.push_back(toDouble(elems[0]));
          intensity.push_back(toDouble(elems[1]));
          label.push_back(elems[2]);
          includeLabel.push_back(toInt(elems[3]));
          ionType.push_back(elems[4]);
          ionNum.push_back(elems[5]);
          formatedLabel.push_back(elems[6]);
          labelX.push_back(toDouble(elems[7]));
          labelY.push_back(toDouble(elems[8]));
          includeArrow.push_back(toInt(elems[9]));
          arrowBegX.push_back(toDouble(elems[10]));
          arrowBegY.push_back(toDouble(elems[11]));
          arrowEndX.push_back(toDouble(elems[12]));
          arrowEndY.push_back(toDouble(elems[13]));
        }
      }
    }
  }
  
  return Rcpp::List::create(Rcpp::Named("metaData") = (Rcpp::List::create(
    Rcpp::Named("sequence") = sequence,
    Rcpp::Named("fullSequence") = fullSequence,
    Rcpp::Named("scanNum") = scanNum,
    Rcpp::Named("parentFile") = parentFile,
    Rcpp::Named("precursorCharge") = precursorCharge)),
    Rcpp::Named("spectrum") = Rcpp::DataFrame::create(Rcpp::Named("mz") = mz,
                           Rcpp::Named("intensity") = intensity,
                           Rcpp::Named("label") = label,
                           Rcpp::Named("includeLabel") = includeLabel,
                           Rcpp::Named("ionType") = ionType,
                           Rcpp::Named("ionNum") = ionNum,
                           Rcpp::Named("formatedLabel") = formatedLabel,
                           Rcpp::Named("labelX") = labelX,
                           Rcpp::Named("labelY") = labelY,
                           Rcpp::Named("includeArrow") = includeArrow,
                           Rcpp::Named("arrowBegX") = arrowBegX,
                           Rcpp::Named("arrowBegY") = arrowBegY, 
                           Rcpp::Named("arrowEndX") = arrowEndX,
                           Rcpp::Named("arrowEndY") = arrowEndY));
}


