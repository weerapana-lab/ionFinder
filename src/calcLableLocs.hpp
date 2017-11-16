//
//  calcLableLocs.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/13/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef calcLableLocs_hpp
#define calcLableLocs_hpp

#include <list>
#include <queue>
#include <map>
#include <iostream>
#include "geometry.hpp"

using namespace std;

namespace labels{
	
	double const CENTER_VEC_DIV = 1;
	double const LOCAL_VEC_DIV = 1;
	
	class Labels;
	struct OverlapNumComparison;
	
	class Labels{
	public:
		typedef geometry::DataLabel labType;
		typedef std::list<labType*> pointsListType;
		typedef std::map<labType*, pointsListType> graphType;
		
		Labels(double _xMin, double _xMax, double _yMin = 0, double _yMax = 100){
			//Min = _xMin; xMax = _xMax; yMin = _yMin; yMax = _yMax;
			center = geometry::Point(((_xMin + _xMax)/2), ((_yMin + _yMax)/2));
			maxIterations = 100;
			centerVecDiv = CENTER_VEC_DIV;
			localVecDiv = LOCAL_VEC_DIV;
		}
		~Labels(){}
		
		void push_back(labType* _new){
			points.push_back(_new);
		}
		
		void spaceOutAlg1();
		void spaceOutAlg2();
		
	private:
		pointsListType points;
		
		//double xMin, xMax, yMin, yMax;
		geometry::Point center;
		size_t maxIterations;
		double centerVecDiv;
		double localVecDiv;
		
		void countAllOverlapNum();
		size_t getOverlapNum(const labType&) const;
		void sortByOverlap();
		void populateGraph(graphType&) const;
		void getOverlap(labType*, pointsListType&) const;
		geometry::Point getCenter(const pointsListType&) const;
		void addVectorToList(geometry::Vector2D, pointsListType&) const;
	};
	
	struct OverlapNumComparison {
		bool const operator()(labels::Labels::labType *lhs,
							  labels::Labels::labType *rhs) const{
			return (*lhs) > (*rhs);
		}
	};
}

#endif /* calcLableLocs_hpp */
