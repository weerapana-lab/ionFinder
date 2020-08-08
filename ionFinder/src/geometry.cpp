//
// geometry.cpp
// ionFinder
// -----------------------------------------------------------------------------
// MIT License
// Copyright 2020 Aaron Maurais
// -----------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -----------------------------------------------------------------------------
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
