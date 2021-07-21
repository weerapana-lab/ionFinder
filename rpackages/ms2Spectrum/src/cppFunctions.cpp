
#include <Rcpp.h>
#include <string>
#include "utils.hpp"

using namespace Rcpp;

const char* DIFFMODS = "*";
StringVector makeIonLables(std::string, int, char);
StringVector makeIonLabelsExp(std::string, int, char);
bool isArg(std::string);
std::string fixOD(std::string);

// [[Rcpp::export]]
std::string getSubscriptNum(std::string str) {
  return utils::getSubscriptNum(std::stoi(str));
}

// [[Rcpp::export]]
StringVector makeIonLabels(std::string letter, int count, char dir) {
  StringVector ret;
  if(dir == 'f') {
    for(int i = 1; i <= count; i++)
      ret.push_back(letter + std::to_string(i));
  }
  else if(dir == 'r') {
    for(int i = count; i > 0; i--)
      ret.push_back(letter + std::to_string(i));
  }
  else throw std::runtime_error("Invalid choice for dir. Only f or r allowed.");
  return ret;
}

// [[Rcpp::export]]
StringVector makeIonLabelsExp(std::string letter, int count, char dir) {
  StringVector ret;
  if(dir == 'f') {
    for(int i = 1; i <= count; i++)
      ret.push_back(letter + "[" + std::to_string(i) + "]");
  }
  else if(dir == 'r') {
    for(int i = count; i > 0; i--)
      ret.push_back(letter + "[" + std::to_string(i) + "]");
  }
  else throw std::runtime_error("Invalid choice for dir. Only f or r allowed.");
  return ret;
}

// [[Rcpp::export]]
StringVector makeSeqVec(std::string seq, bool fixDiffMods = true) {
  bool modFound = false;
  char mod = '\0';
  StringVector ret;

  for(size_t i = 0; i < seq.length(); i++) {
    if(fixDiffMods) {
      if((i + 1) < seq.length()) {
        //iterate through diffmods
        for(const char* p = DIFFMODS; *p; p++) {
          //check if next char in sequence is diff mod char
          if(seq[i + 1] == *p) {
            modFound = true;
            mod = seq[i + 1];
            seq.erase(i + 1, 1);
            break;
          }//end of if
        }//end of for
      }//end of if
    }//end if
    if(modFound) {
      ret.push_back(seq[i] + std::string(1, mod));
      modFound = false;
    }
    else ret.push_back(seq[i]);
  }//end of for
  return ret;
}

// [[Rcpp::export]]
bool isArg(std::string arg) {
  return !(arg[0] == '-');
}

// [[Rcpp::export]]
std::string fixOD(std::string str) {
  if(str[str.length() - 1] != '/')
    str += '/';
  return str;
}

// [[Rcpp::export]]
Rcpp::CharacterVector getFoundIons(Rcpp::CharacterVector ionTypes, Rcpp::CharacterVector ionNums) {
  Rcpp::CharacterVector ret;

  size_t len = ionTypes.size();
  if(len != ionNums.size())
    throw std::runtime_error("ionTypes and ionNums must be the same length!");

  for(size_t i = 0; i < len; i++)
    ret.push_back(std::string(1, ionTypes[i][0]) + std::string(ionNums[i]));


  return ret;
}


