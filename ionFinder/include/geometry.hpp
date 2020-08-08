//
// geometry.hpp
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

#ifndef geometry_hpp
#define geometry_hpp

#include <cmath>
#include <iostream>

namespace geometry{
	
	class Point;
	class Line;
	class Rect;
	class Vector2D;
	
	std::string const NA_STR = "NA";
	
	bool valueInRange(double, double, double);
	double dist(const Point&, const Point&);
	
	class Point{
		friend double dist(const Point&, const Point&);
		friend class Vector2D;
		friend class Rect;
	protected:
		double x, y;
		
	public:
		
		Point(double _x, double _y){
			x = _x; y = _y;
		}
		Point(){
			x = 0; y = 0;
		}
		
		void operator = (const Point& cp){
			x = cp.x; y = cp.y;
		}
		bool operator == (const Point& comp){
			return (x == comp.x && y == comp.y);
		}
		
		void setPos(double _x, double _y){
			x = _x; y = _y;
		}
		void move(const Vector2D&);
		
		double getX() const{
			return x;
		}
		double getY() const{
			return y;
		}
	};
	
	class Rect : public Point{
	protected:
		double width, height;
		
	public:
		
		Rect() : Point() {
			width = 0; height = 0;
		}
		Rect(double _x, double _y, double _width, double _height) : Point(_x, _y){
			width = _width;
			height = _height;
		}
		~Rect() {}
		
		bool intersects(const Rect&) const;
		bool operator == (const Rect& comp) const{
			return (x == comp.x && y == comp.y &&
					width == comp.width && height == comp.height);
		}
		
		double getWidth() const{
			return width;
		}
		double getHeight() const{
			return height;
		}
		Point getTLC() const{
			Point ret;
			ret.x = (x - width/2);
			ret.y = (y + height/2);
			return ret;
		}
		Point getBRC() const{
			Point ret;
			ret.x = (x + width/2);
			ret.y = (y - height/2);
			return ret;
		}
	};
	
	class Vector2D{
	private:
		double v, h;
	public:
		
		Vector2D(){
			v = 0; h = 0;
		}
		Vector2D(double _v, double _h){
			v = _v; h = _h;
		}
		Vector2D(const Point& p1, const Point& p2){
			h = p2.x - p1.x;
			v = p2.y - p1.y;
		}
		~Vector2D() {}
		
		void operator += (const Vector2D& add){
			v += add.v;
			h += add.h;
		}
		void operator /= (double div){
			v /= div; h /= div;
		}
		
		double getMagnitude() const{
			return dist(Point(0, 0), Point(h, v));
		}
		double getDirrection() const{
			return atan(v/h);
		}
		double getH() const{
			return h;
		}
		double getV() const{
			return v;
		}
	};
	
	class Line{
	public:
		Point beg;
		Point end;
		
		Line() {
			beg = Point(0, 0);
			end = Point(0, 0);
		}
		Line(double begX, double begY, double endX, double endY){
			beg = Point(begX, begY);
			end = Point(endX, endY);
		}
		Line(const Point& _beg, const Vector2D& mov){
			beg = _beg;
			end = Point(beg.getX() + mov.getH(), end.getY() + mov.getV());
		}
		~Line() {}
		double length() const{
			return dist(beg, end);
		}
	};
	
	class DataLabel{
	private:
		std::string label;
		bool includeLabel;
		bool includeArrow;
		Point dataPoint;
	
	public:
		Line arrow;
		Rect labelLoc;
		Vector2D movement;
		bool forceLabel;
		size_t overlapNum;
		
		DataLabel(){
			label = NA_STR;
			arrow = Line();
			labelLoc = Rect();
			movement = Vector2D();
			forceLabel = false;
			includeLabel = false;
			includeArrow = false;
		}
		
		void setIncludeLabel(bool boo){
			includeLabel = boo;
		}
		void setLabel(std::string str){
			label = str;
		}
		std::string getLabel() const{
			return label;
		}
		bool getIncludeLabel() const{
			return includeLabel;
		}
		bool getIncludeArrow() const{
			return includeArrow;
		}
		
		bool operator > (const DataLabel& comp) const{
			return overlapNum > comp.overlapNum;
		}
		bool operator < (const DataLabel& comp) const{
			return overlapNum < comp.overlapNum;
		}
		bool operator == (const DataLabel& comp) const{
			return labelLoc == comp.labelLoc;
		}
	};
}

#endif /* geometry_h */
