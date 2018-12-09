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
#include <geometry.hpp>

namespace labels{

	double const ARROW_THRESHOLD_DIV = 97.3528;
	
	class Labels;
	struct OverlapNumComparison;
	struct YComparison;
	
	class Labels{
	public:
		typedef geometry::DataLabel labType;
		typedef std::list<labType*> pointsListType;
		typedef std::map<labType*, pointsListType> graphType;
		typedef std::list<geometry::Rect> dataListType;
		
		Labels(double _xMin, double _xMax, double _yMin = 0, double _yMax = 100,
			   double _nudgeThreshold = 1, double _nudgeAmt = 5){
			xMin = _xMin; xMax = _xMax; yMin = _yMin; yMax = _yMax;
			center = geometry::Point(((_xMin + _xMax)/2), ((_yMin + _yMax)/2));
			maxIterations = 100;
			nudgeThreshold = _nudgeThreshold;
			nudgeAmt = _nudgeAmt;
			arrowThresholdH = (xMax - xMin) / ARROW_THRESHOLD_DIV;
			arrowThresholdV = (yMax - yMin) / ARROW_THRESHOLD_DIV;
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
		
		double xMin, xMax, yMin, yMax;
		geometry::Point center;
		size_t maxIterations;
		double nudgeThreshold, nudgeAmt;
		double arrowThresholdH, arrowThresholdV;
		
		void countAllOverlapNum();
		size_t getOverlapNum(const labType&) const;
		void sortByOverlap();
		void sortByY();
		void populateGraph(graphType&) const;
		void getOverlap(labType*, pointsListType&) const;
		geometry::Point getCenter(const pointsListType&) const;
		void addVectorToList(geometry::Vector2D, pointsListType&) const;
		//void spaceOut(labType*, pointsListType&);
		void addStaticLables();
		void addArows();
		
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
