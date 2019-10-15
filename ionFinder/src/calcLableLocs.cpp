//
//  calcLableLocs.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/13/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <calcLableLocs.hpp>

void labels::Labels::countAllOverlapNum()
{
	//get num of overlapint rectangles
	for(pointsListType::iterator it = labeledPoints.begin(); it != labeledPoints.end(); ++it)
		(*it)->overlapNum = getOverlapNum(*(*it));
}

void labels::Labels::sortByOverlap(){
	countAllOverlapNum();
	labeledPoints.sort(labels::OverlapNumComparison());
}

void labels::Labels::sortByY(){
	//labeledPoints.sort(labels::YComparison());
}

size_t labels::Labels::getOverlapNum(const labels::Labels::labType& lab) const
{
	size_t ret = 0;
	for(pointsListType::const_iterator it = labeledPoints.begin(); it != labeledPoints.end(); ++it)
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
	/*if(lab->getLabel() == "y2")
		std::cout << "found!";
	labType* temp;*/
	
	overlapList.clear();
	lab->overlapNum = 0;
	for(pointsListType::const_iterator it = labeledPoints.begin(); it != labeledPoints.end(); ++it)
	{
		if((*it)->labelLoc == lab->labelLoc)
			continue;
		
		if(!(*it)->getIncludeLabel())
			continue;
		
		/*if((*it)->getLabel() == "b3")
		{
			std::cout << "found!";
			//temp = *it;
		}*/
		
		if(lab->labelLoc.intersects((*it)->labelLoc))
		{
			//temp = *it;
			overlapList.push_back((*it));
			lab->overlapNum++;
		}
	}
}

void labels::Labels::populateGraph(labels::Labels::graphType& graph) const
{
	graph.clear();
	for(pointsListType::const_iterator it = labeledPoints.begin(); it != labeledPoints.end(); ++it)
	{
		if(!(*it)->getIncludeLabel())
			continue;
		pointsListType tempList;
		getOverlap((*it), tempList);
		if((*it)->overlapNum > 0)
			graph[(*it)] = tempList;
	}
}

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

bool labels::Labels::maxInList(const labType* const p, const pointsListType& l) const
{
	double tempMax = p->labelLoc.getY();
	for(pointsListType::const_iterator it = l.begin(); it != l.end(); ++it)
		if(tempMax < (*it)->labelLoc.getY() || (*it)->forceLabel)
			return false;
	return true;
}

bool labels::Labels::overlapsStaticDataPoints(const labels::Labels::labType* const lab) const
{
	for(dataListType::const_iterator it = dataPoints.begin(); it != dataPoints.end(); ++it)
	{
		if(lab->labelLoc.getX() == it->getX())
			continue;
		
		if(lab->labelLoc.intersects(*it))
			return true;
	}
	return false;
}

//void labels::Labels::spaceOut(labels::Labels::labType* lab, labels::Labels::pointsListType& overlapList){}

void labels::Labels::addStaticLables()
{
	graphType graph;
	populateGraph(graph); //populate graph of overlapping points
	sortByY(); //sort labeledPoints by intensity
	
	for(pointsListType::iterator it = labeledPoints.begin(); it != labeledPoints.end(); ++it)
	{
		if(!(*it)->forceLabel) //if it is not a labeled b or y ion
		{
			//if it is not max int in labels it overlaps
			if(!maxInList(*it, graph[(*it)]) || overlapsStaticDataPoints((*it)))
			{
				(*it)->setIncludeLabel(false);
			}
		}//end of if
	}//end of for
}

void labels::Labels::addArows()
{
	for(pointsListType::iterator it = labeledPoints.begin(); it != labeledPoints.end(); ++it)
	{
		if((*it)->getIncludeLabel() && (*it)->movement.getMagnitude() > 0)
		{
			if((*it)->movement.getH() > arrowThresholdH  || (*it)->movement.getV() > arrowThresholdV)
			{
				double begX = (*it)->labelLoc.getX();
				double begY = (*it)->labelLoc.getX();
				(*it)->arrow = geometry::Line(begX, begY,
											  begX + (*it)->movement.getH(),
											  begY + (*it)->movement.getV());
			}
		}
	}
}

void labels::Labels::spaceOutAlg2()
{
	labeledPoints.unique();
	dataPoints.unique();
	
	graphType graph;
	//size_t numIterations = 0;
	
	for(pointsListType::iterator it = labeledPoints.begin(); it != labeledPoints.end(); ++it)
	{
		if((*it)->labelLoc.getY() < nudgeThreshold)
			(*it)->movement = geometry::Vector2D((nudgeAmt - (*it)->labelLoc.getY()), 0);
	}
	
	addStaticLables();
	
	/*do{
		sortByOverlap();
		populateGraph(graph);
		
		if(labeledPoints.front()->overlapNum > 0)
		{
			spaceOut(labeledPoints.front(), graph[labeledPoints.front()]);
		}//end of if
		
		numIterations++;
	} while(labeledPoints.front()->overlapNum > 0 && numIterations < maxIterations);
	
	addStaticLables();*/
	addArows();
	
}//end of fxn



