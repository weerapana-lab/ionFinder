//
//  calcLableLocs.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/13/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "../include/calcLableLocs.hpp"

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
	//if(lab->getLabel() == "946.405")
	//	cout << "found!";
	//labType temp;
	
	overlapList.clear();
	lab->overlapNum = 0;
	for(pointsListType::const_iterator it = labeledPoints.begin(); it != labeledPoints.end(); ++it)
	{
		if((*it)->labelLoc == lab->labelLoc)
			continue;
		
		if(lab->labelLoc.intersects((*it)->labelLoc))
		{
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
		pointsListType tempList;
		getOverlap((*it), tempList);
		if((*it)->overlapNum > 0)
			graph[(*it)] = tempList;
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

void labels::Labels::spaceOutAlg2()
{
	labeledPoints.unique();
	dataPoints.unique();
	
	graphType graph;
	//size_t i = 0;
	
	populateGraph(graph); //populate graph of overlaping points
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
	
	/*do {
		sortByOverlap();
		populateGraph(graph);
		
		for(graphType::iterator it1 = graph.begin(); it1 != graph.end(); ++it1)
		{
			//if there are points overlaping current point
			if(it1->first->overlapNum > 0)
			{
				if(!it1->first->forceLabel) //if it is not a labeled b or y ion
				{
					for(pointsListType::iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it1)
					{
						if((!(*it2)->forceLabel) && overlapsStaticDataPoints(*it2))
						{
							(*it2)->setIncludeLabel(false);
							//it1->second.remove(*it2);
						}
					}
					if(!maxInList(it1->first, it1->second)) //if it is not max int in labels it overlaps
					{
						it1->first->setIncludeLabel(false);
					}
					//check if it overlaps any static data points
					else if(overlapsStaticDataPoints(it1->first))
					{
						it1->first->setIncludeLabel(false);
					}//end of else
				}//end of if
				else {
					continue;
				}//end of else
			}//end of if
		}//end of for
		i++;
	} while(labeledPoints.front()->overlapNum > 0 && i < maxIterations);*/
}//end of fxn

void labels::Labels::spaceOutAlg1()
{
	labeledPoints.unique();
	dataPoints.unique();
	
	sortByOverlap();
	graphType graph;
	size_t i = 0;
	while(labeledPoints.front()->overlapNum > 0)
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




