//
//  geometry.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/14/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <geometry.hpp>

bool geometry::valueInRange(double value, double min, double max){
	return (value >= min) && (value <= max);
}

double geometry::dist(const geometry::Point& p1, const geometry::Point& p2){
	return sqrt(pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2));
}

void geometry::Point::move(const geometry::Vector2D& mv){
	x += mv.getH(); y += mv.getV();
}

bool geometry::Rect::intersects(const geometry::Rect& comp) const
{
	/*bool xOverlap = valueInRange(x, comp.x, comp.x + comp.width) ||
		valueInRange(comp.x, x, x + width);
	
	bool yOverlap = valueInRange(y, comp.y, comp.y + comp.height) ||
		valueInRange(comp.y, y, y + height);
	
	return xOverlap && yOverlap;*/
	
	Point l1 = getTLC();
	Point r1 = getBRC();
	Point l2 = comp.getTLC();
	Point r2 = comp.getBRC();
	
	// If one rectangle is on left side of other
	if (l1.x > r2.x || l2.x > r1.x)
		return false;
	
	// If one rectangle is above other
	if (l1.y < r2.y || l2.y < r1.y)
		return false;
	
	return true;
}
