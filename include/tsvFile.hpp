//
//  tsvFile.hpp
//  ionFinder
//
//  Created by Aaron Maurais on 2/22/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#ifndef tsvFile_hpp
#define tsvFile_hpp

#include <string>
#include <map>
#include <vector>
#include <fstream>

#include <utils.hpp>

namespace utils{
	
	class TsvFile;
	
	class TsvFile{
	private:
		typedef std::vector<std::string> rowType;
		typedef std::vector<rowType> datType;
		datType _dat;
		
		typedef std::map<std::string, size_t> colMapType;
		colMapType _colNames;
		
		std::string _fname;
		bool _hasHeaders;
		bool _quote;
		char _delim;
		size_t _nCol;
		size_t _nRow;
		std::string _blank;
		
		//private cell access functions
		std::string _getVal(size_t row, std::string colName) const{
			return _getVal(row, getColIndex(colName));
		}
		std::string _getVal(size_t row, size_t col) const{
			return _dat[col][row];
		}
		
	public:
		TsvFile(){
			_fname = "";
			_hasHeaders = true;
			_quote = false;
			_delim = '\t';
			_nCol = 0; _nRow = 0;
			_blank = "";
		}
		TsvFile(std::string fname, bool hasHeaders = true,
				char delim = IN_DELIM, bool quote = false, std::string blank = ""){
			_fname = fname;
			_hasHeaders = hasHeaders;
			_delim = delim;
			_nCol = 0; _nRow = 0;
			_blank = blank;
		}
		
		//modifers
		bool read(std::string fname){
			_fname = fname;
			return read();
		}
		bool read();
		bool write(std::ostream& out, char delim = '\t');
		bool write(std::string ofname, char delim = '\t');
		
		//access to multiple cells
		void getRow(size_t index, std::vector<std::string>& elems) const;
		std::string getRow(size_t index) const;
		std::vector<std::string> getCol(std::string rowName) const;
		datType& getDat(){
			return _dat;
		}
		
		//individual cell access
		std::string getValStr(size_t row, size_t col) const{
			return _getVal(row, col);
		}
		std::string getValStr(size_t row, std::string colName) const{
			return _getVal(row, colName);
		}
		double getValDouble(size_t row, size_t col) const{
			return atof(_getVal(row, col).c_str());
		}
		double getValDouble(size_t row, std::string colName) const{
			return atof(_getVal(row, colName).c_str());
		}
		int getValInt(size_t row, size_t col) const{
			return atoi(_getVal(row, col).c_str());
		}
		int getValInt(size_t row, std::string colName) const{
			return atoi(_getVal(row, colName).c_str());
		}
		size_t getValSize(size_t row, size_t col) const{
			return std::stoi(_getVal(row, col));
		}
		size_t getValSize(size_t row, std::string colName) const{
			return std::stoi(_getVal(row, colName));
		}
		bool getValBool(size_t row, size_t col) const{
			return atoi(_getVal(row, col).c_str());
		}
		bool getValBool(size_t row, std::string colName) const{
			return atoi(_getVal(row, colName).c_str());
		}
		
		//metadata access
		long getColIndex(std::string colName) const{
			colMapType::const_iterator it = _colNames.find(colName);
			if(it == _colNames.end())
				return -1;
			return it->second;
		}
		bool colExists(std::string search) const{
			colMapType::const_iterator it = _colNames.find(search);
			return it != _colNames.end();
		}
		size_t getNrow() const{
			return _nRow;
		}
		size_t getNcol() const{
			return _nCol;
		}
	};
}

#endif /* tsvFile_hpp */
