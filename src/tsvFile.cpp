//
//  tsvFile.cpp
//  ionFinder
//
//  Created by Aaron Maurais on 2/22/19.
//  Copyright © 2019 Aaron Maurais. All rights reserved.
//

#include <tsvFile.hpp>

bool utils::TsvFile::read()
{
	if(_fname.empty()){
		std::cerr << "File name must be specified!" << std::endl;
		return false;
	}
	
	std::ifstream inF(_fname.c_str());
	if(!inF)
		return false;
	
	std::string line;
	std::vector<std::string> elems;
	if(_hasHeaders)
	{
		utils::safeGetline(inF, line);
		utils::split(line, _delim, elems);
		_nCol = elems.size();
		colMapType::iterator foundIt;
		for(size_t i = 0; i < _nCol; i++){
			foundIt = _colNames.find(elems[i]);
			if(foundIt == _colNames.end()){
				std::string temp = elems[i];
				if(!_caseSensitive)
					temp = utils::toLower(temp);
				_colNames[temp] = i;
			}
			else{
				throw std::runtime_error("Duplicate colnames found!");
			}
		}
	}
	else{
		std::streampos old = inF.tellg();
		utils::safeGetline(inF, line, old);
		inF.seekg(old);
		utils::split(line, _delim, elems);
		_nCol = elems.size();
		for(size_t i = 0; i < _nCol; i++){
			_colNames[("c" + std::to_string(i + 1))] = i;
		}
	}
	_dat.resize(_nCol);
	
	while(utils::safeGetline(inF, line))
	{
		line = utils::trim(line);
		if(line == "")
			continue;
		utils::split(line, _delim, elems);
		if(elems.size() > _nCol){
			std::cerr << "Not enough colnames!" << std::endl;
			return false;
		}
		
		for(size_t i = 0; i < _nCol; i++){
			if(i < elems.size())
				_dat[i].push_back(elems[i]);
			else _dat[i].push_back(_blank);
		}
		_nRow++;
	}
	
	return true;
}

bool utils::TsvFile::write(std::ostream& out, char delim)
{
	if(!out)
		return false;
	
	for(datType::iterator it1 = _dat.begin(); it1 != _dat.end(); ++it1)
	{
		for(rowType::iterator it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			if(it2 == it1->begin())
				out << *it2;
			else out << delim << *it2;
		}
		out << std::endl;
	}
	return true;
}

bool utils::TsvFile::write(std::string ofname, char delim){
	std::ofstream outF(ofname.c_str());
	return write(outF, delim);
}

