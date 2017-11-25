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
#include "../include/geometry.hpp"

using namespace std;

namespace labels{
	
	double const CENTER_VEC_DIV = 1;
	double const LOCAL_VEC_DIV = 1;
	
	
	class Labels;
	struct OverlapNumComparison;
	struct YComparison;
	
	class Labels{
	public:
		typedef geometry::DataLabel labType;
		typedef std::list<labType*> pointsListType;
		typedef std::map<labType*, pointsListType> graphType;
		typedef std::list<geometry::Rect> dataListType;
		
		Labels(double _xMin, double _xMax, double _yMin = 0, double _yMax = 100){
			//Min = _xMin; xMax = _xMax; yMin = _yMin; yMax = _yMax;
			center = geometry::Point(((_xMin + _xMax)/2), ((_yMin + _yMax)/2));
			maxIterations = 100;
			centerVecDiv = CENTER_VEC_DIV;
			localVecDiv = LOCAL_VEC_DIV;
		}
		~Labels(){}
		
		void push_back_dataPoint(const geometry::Rect& _new){
			dataPoints.push_back(_new);
		}
		void push_back_labeledPoint(labType* _new){
			labeledPoints.push_back(_new);
		}
		
		
		void spaceOutAlg1();
		void spaceOutAlg2();
		
	private:
		pointsListType labeledPoints;
		dataListType dataPoints;
		
		//double xMin, xMax, yMin, yMax;
		geometry::Point center;
		size_t maxIterations;
		double centerVecDiv;
		double localVecDiv;
		
		void countAllOverlapNum();
		size_t getOverlapNum(const labType&) const;
		void sortByOverlap();
		void sortByY();
		void populateGraph(graphType&) const;
		void getOverlap(labType*, pointsListType&) const;
		geometry::Point getCenter(const pointsListType&) const;
		void addVectorToList(geometry::Vector2D, pointsListType&) const;
		
		bool maxInList(const labType* const, const pointsListType&) const;
		bool overlapsStaticDataPoints(const labType* const) const;
	};
	
	struct OverlapNumComparison {
		bool const operator()(labels::Labels::labType *lhs,
							  labels::Labels::labType *rhs) const{
			return (*lhs) > (*rhs);
		}
	};
	
	struct YComparison {
		bool const operator()(labels::Labels::labType *lhs,
							  labels::Labels::labType *rhs) const{
			return lhs->labelLoc.getY() > rhs->labelLoc.getY();
		}
	};
}

#endif /* calcLableLocs_hpp */
