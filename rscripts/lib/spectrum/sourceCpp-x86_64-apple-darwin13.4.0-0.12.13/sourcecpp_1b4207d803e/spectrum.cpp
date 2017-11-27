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
#include <cassert>
#include <Rcpp.h>

#define IN_DELIM '\t'

using namespace std;
using namespace Rcpp;

const char* END_LINES = "\r\n";
string const WHITESPACE = " \f\n\r\t\v";
string const COMMENT_SYMBOL = "#";
const char* BEGIN_METADATA = "<metadata>";
const char* END_METADATA = "</metadata>";
const char* BEGIN_SPECTRUM = "<spectrum>";
const char* END_SPECTRUM = "</spectrum>";

void readBuffer(string, char*&);
List getSpectrum(string);
inline void split (const string&, const char, vector<string>&);
string trimTraling(const string&);
string trimLeading(const string&);
string trim(const string&);
void trimAll(vector<string>&);
bool isCommentLine(string);
template <typename _Tp> inline string toString(_Tp);
inline int toInt(string);
inline double toDouble(string);
bool isInteger(string);

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

void readBuffer(string fname, char*& buffer)
{
  ifstream inF(fname.c_str());
  
  if(!inF)
    throw runtime_error("Could not open " + fname);
  
  inF.seekg(0, inF.end);
  size_t size = inF.tellg();
  inF.seekg(0, inF.beg);
  buffer = new char [size];
  
  if(!inF.read(buffer, size))
    throw runtime_error("Could not read " + fname);
}

// [[Rcpp::export]]
List getSpectrum(string fname)
{
  char* buffer = NULL;
  readBuffer(fname, buffer);
  
  string scanNum, parentFile, sequence, precursorCharge;
  NumericVector mz;
  NumericVector intensity;
  CharacterVector label;
  LogicalVector includeLabel;
  CharacterVector ionType;
  CharacterVector formatedLabel;
  NumericVector labelX;
  NumericVector labelY;
  
  //NumericVector arrowEndX;
  //NumericVector arrowEndY;
  
  string line;
  vector<string> elems;
  char* tok = strtok(buffer, END_LINES);
  while(tok != NULL)
  {
    if(!strcmp(tok, BEGIN_METADATA))
    {
      tok = strtok(NULL, END_LINES);
      while(tok != NULL)
      {
        if(!strcmp(END_METADATA, tok))
          break;
        
        line = string(tok);
        if(isCommentLine(line))
          continue;
        
        split(line, IN_DELIM, elems);
        trimAll(elems);
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
        else if(elems[0] == "precursorCharge")
        {
          precursorCharge = elems[1];
        }
        tok = strtok(NULL, END_LINES);
      }
    }
    else if(!strcmp(tok, BEGIN_SPECTRUM))
    {
      tok = strtok(NULL, END_LINES);
      while(tok != NULL)
      {
        if(!strcmp(END_SPECTRUM, tok))
          break;
        
        line = string(tok);
        if(isCommentLine(line))
        {
          tok = strtok(NULL, END_LINES);
          continue;
        }
        
        split(line, IN_DELIM, elems);
        trimAll(elems);
        if(elems[0] == "mz") //if header line, continue
        {
          tok = strtok(NULL, END_LINES);
          continue;
        }
        else{
          assert(elems.size() == 8);
          
          mz.push_back(toDouble(elems[0]));
          intensity.push_back(toDouble(elems[1]));
          label.push_back(elems[2]);
          includeLabel.push_back(toInt(elems[3]));
          ionType.push_back(elems[4]);
          formatedLabel.push_back(elems[5]);
          labelX.push_back(toDouble(elems[6]));
          labelY.push_back(toDouble(elems[7]));
          
          //arrowEndX.push_back(elems[8]);
          //arrowEndY.push_back(elems[9]);
        }
        tok = strtok(NULL, END_LINES);
      }
    }
    tok = strtok(NULL, END_LINES);
  }
  
  return List::create(Named("metaData") = (List::create(
    Named("sequence") = sequence,
    Named("scanNum") = scanNum,
    Named("parentFile") = parentFile,
    Named("precursorCharge") = precursorCharge)),
    Named("spectrum") = DataFrame::create(Named("mz") = mz,
                           Named("intensity") = intensity,
                           Named("label") = label,
                           Named("includeLabel") = includeLabel,
                           Named("ionType") = ionType,
                           Named("formatedLabel") = formatedLabel,
                           Named("labelX") = labelX,
                           Named("labelY") = labelY));
}




#include <Rcpp.h>
// getSpectrum
List getSpectrum(string fname);
RcppExport SEXP sourceCpp_1_getSpectrum(SEXP fnameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< string >::type fname(fnameSEXP);
    rcpp_result_gen = Rcpp::wrap(getSpectrum(fname));
    return rcpp_result_gen;
END_RCPP
}
