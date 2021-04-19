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

  std::string scanNum, parentFile, ofname, sequence, fullSequence, precursorCharge;
  double plotWidth, plotHeight;
  Rcpp::NumericVector mz;
  Rcpp::NumericVector intensity;
  Rcpp::CharacterVector label;
  Rcpp::CharacterVector color;
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

        if(utils::isCommentLine(line) || utils::trim(line).empty())
          continue;

        utils::split(line, IN_DELIM, elems);
        utils::trimAll(elems);
        std::string key = elems[0];
        std::string value = "";
        if(elems.size() > 1) value = elems[1];
        if(key == "scanNumber"){
          scanNum = value;
        }
        else if(key == "precursorFile"){
          parentFile = value;
        }
        else if(key == "ofname"){
          if(value.empty()) throw std::runtime_error("No value for required metadata entry 'ofname'");
          ofname = value;
        }
        else if(key == "sequence"){
          sequence = value;
        }
        else if(key == "fullSequence"){
          if(value.empty()) throw std::runtime_error("No value for required metadata entry 'fullSequence'");
          fullSequence = value;
        }
        else if(key == "precursorCharge"){
          precursorCharge = value;
        }
        else if(key == "plotHeight"){
          if(value.empty()) throw std::runtime_error("No value for required metadata entry 'plotHeight'");
          plotHeight = std::stod(value);
        }
        else if(key == "plotWidth"){
          if(value.empty()) throw std::runtime_error("No value for required metadata entry 'plotWidth'");
          plotWidth = std::stod(value);
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
        if(elems[0] == "mz"){ //if header line, continue
          continue;
        }
        else {
          if(elems.size() != 15)
            throw std::runtime_error("elems.size() != 15. Size is " + std::to_string(elems.size()));

          mz.push_back(std::stod(elems[0]));
          intensity.push_back(std::stod(elems[1]));
          label.push_back(elems[2]);
		  color.push_back(elems[3]);
          includeLabel.push_back(std::stoi(elems[4]));
          ionType.push_back(elems[5]);
          ionNum.push_back(elems[6]);
          formatedLabel.push_back(elems[7]);
          labelX.push_back(std::stod(elems[8]));
          labelY.push_back(std::stod(elems[9]));
          includeArrow.push_back(std::stoi(elems[10]));
          arrowBegX.push_back(std::stod(elems[11]));
          arrowBegY.push_back(std::stod(elems[12]));
          arrowEndX.push_back(std::stod(elems[13]));
          arrowEndY.push_back(std::stod(elems[14]));
        }
      }
    }
  }

  return Rcpp::List::create(Rcpp::Named("metaData") = (Rcpp::List::create(
    Rcpp::Named("sequence") = sequence,
    Rcpp::Named("fullSequence") = fullSequence,
    Rcpp::Named("scanNum") = scanNum,
    Rcpp::Named("parentFile") = parentFile,
    Rcpp::Named("ofname") = ofname,
    Rcpp::Named("precursorCharge") = precursorCharge,
    Rcpp::Named("plotWidth") = plotWidth,
    Rcpp::Named("plotHeight") = plotHeight)),
    Rcpp::Named("spectrum") = Rcpp::DataFrame::create(Rcpp::_["stringsAsFactors"] = false,
                                                      Rcpp::Named("mz") = mz,
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


