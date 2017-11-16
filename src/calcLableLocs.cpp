//
//  calcLableLocs.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/13/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "calcLableLocs.hpp"

void labels::Labels::countAllOverlapNum()
{
	//get num of overlapint rectangles
	for(pointsListType::iterator it = points.begin(); it != points.end(); ++it)
		(*it)->labelLoc.overlapNum = getOverlapNum(*(*it));
}

void labels::Labels::sortByOverlap(){
	countAllOverlapNum();
	points.sort(labels::OverlapNumComparison());
}

size_t labels::Labels::getOverlapNum(const labels::Labels::labType& lab) const
{
	size_t ret = 0;
	for(pointsListType::const_iterator it = points.begin(); it != points.end(); ++it)
	{
		if((*it)->labelLoc == lab.labelLoc)
			continue;
		if(lab.labelLoc.intersects((*it)->labelLoc))
			ret++;
	}
	return ret;
}

void labels::Labels::getOverlap(labels::Labels::labType* lab, pointsListType& overlapList) const
{
	overlapList.clear();
	lab->labelLoc.overlapNum = 0;
	for(pointsListType::const_iterator it = points.begin(); it != points.end(); ++it)
	{
		if((*it)->labelLoc == lab->labelLoc)
			continue;
		if(lab->labelLoc.intersects((*it)->labelLoc))
		{
			overlapList.push_back((*it));
			lab->labelLoc.overlapNum++;
		}
	}
}

void labels::Labels::populateGraph(labels::Labels::graphType& graph) const
{
	graph.clear();
	for(pointsListType::const_iterator it = points.begin(); it != points.end(); ++it)
	{
		pointsListType tempList;
		getOverlap((*it), tempList);
		if((*it)->labelLoc.overlapNum > 0)
			graph[(*it)] = tempList;
		//cout << graph[(*it)].size() << endl;
	}
}

/*void labels::Labels::spaceOut()
{
	sortByOverlap();
	stack<labType*> sta;
	list<labType*> tempPoints = points;
	
	while(tempPoints.front()->labelLoc.overlapNum >= 1)
	{
		sta.push(tempPoints.front());
		tempPoints.pop_front();
		sortByOverlap();
	}

	while(sta.size() >= 1)
	{
		
	}
	
}*/

geometry::Point labels::Labels::getCenter(const pointsListType& _list) const
{
	double x = 0;
	double y = 0;
	size_t pointCount = 0;
	
	for(pointsListType::const_iterator it = _list.begin(); it != _list.end(); ++it)
	{
		x += (*it)->labelLoc.getX();
		y += (*it)->labelLoc.getY();
		pointCount++;
	}
	return (geometry::Point(x/pointCount, y/pointCount));
}

void labels::Labels::spaceOutAlg2()
{
	
}

void labels::Labels::spaceOutAlg1()
{
	sortByOverlap();
	graphType graph;
	size_t i = 0;
	while(points.front()->labelLoc.overlapNum > 0)
	{
		sortByOverlap();
		populateGraph(graph);
		
		//geometry::Vector2D tempCent();
		for(graphType::iterator it = graph.begin(); it != graph.end(); ++it)
		{
			geometry::Point localCenter = getCenter(it->second);
			
			geometry::Vector2D tempV(geometry::Point(it->first->labelLoc.getX(),
													 it->first->labelLoc.getY()),
									 localCenter);
			geometry::Vector2D tempC(center,
									 geometry::Point(it->first->labelLoc.getX(),
													 it->first->labelLoc.getY()));
			
			tempV /= localVecDiv;
			tempC /= centerVecDiv;
			
			//tempC += tempV;
			tempV += tempC;
									 
			
			it->first->labelLoc.move(tempV);
			
			//addVectorToList(tempV, it->second);
		}
		cout << i << endl;
		i++;
	}
	
}




