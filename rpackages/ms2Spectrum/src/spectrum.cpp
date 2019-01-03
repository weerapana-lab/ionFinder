//
//  spectrum.cpp
//  spectrumReader
//
//  Created by Aaron Maurais on 11/24/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <Rcpp.h>
#include <fstream>
#include <string>

#include "utils.hpp"

const char* BEGIN_METADATA = "<metadata>";
const char* END_METADATA = "</metadata>";
const char* BEGIN_SPECTRUM = "<spectrum>";
const char* END_SPECTRUM = "</spectrum>";

Rcpp::List getSpectrum(std::string);

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

  while(!utils::safeGetline(inF, line).eof())
  {
    line = utils::trim(line);

    if(line == BEGIN_METADATA)
    {
      while(!utils::safeGetline(inF, line).eof())
      {
        line = utils::trim(line);
        if(line == END_METADATA)
          break;

        if(utils::isCommentLine(line))
          continue;

        utils::split(line, IN_DELIM, elems);
        utils::trimAll(elems);
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
      while(!utils::safeGetline(inF, line).eof())
      {
        line = utils::trim(line);
        if(line == END_SPECTRUM)
          break;

        if(utils::isCommentLine(line) || line.empty())
          continue;

        utils::split(line, IN_DELIM, elems);
        utils::trimAll(elems);
        if(elems[0] == "mz") //if header line, continue
        {
          continue;
        }
        else {
          if(elems.size() != 14)
            throw std::runtime_error("elems.size() != 13");

          mz.push_back(std::stod(elems[0]));
          intensity.push_back(std::stod(elems[1]));
          label.push_back(elems[2]);
          includeLabel.push_back(std::stoi(elems[3]));
          ionType.push_back(elems[4]);
          ionNum.push_back(elems[5]);
          formatedLabel.push_back(elems[6]);
          labelX.push_back(std::stod(elems[7]));
          labelY.push_back(std::stod(elems[8]));
          includeArrow.push_back(std::stoi(elems[9]));
          arrowBegX.push_back(std::stod(elems[10]));
          arrowBegY.push_back(std::stod(elems[11]));
          arrowEndX.push_back(std::stod(elems[12]));
          arrowEndY.push_back(std::stod(elems[13]));
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


