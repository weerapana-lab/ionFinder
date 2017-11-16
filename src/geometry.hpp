//
//  geometry.hpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/14/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#ifndef geometry_hpp
#define geometry_hpp

#include <cmath>
#include <iostream>

using namespace std;

namespace geometry{
	
	class Point;
	class Line;
	class Rect;
	class Vector2D;
	
	inline bool valueInRange(double, double, double);
	inline double dist(const Point&, const Point&);
	
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
		
		size_t overlapNum;
		
		Rect() : Point() {
			width = 0; height = 0;
			overlapNum = 0;
		}
		Rect(double _x, double _y, double _width, double _height) : Point(_x, _y){
			width = _width;
			height = _height;
			overlapNum = 0;
		}
		~Rect() {}
		
		bool intersects(const Rect&) const;
		bool operator == (const Rect& comp) const{
			return (x == comp.x && y == comp.y &&
					width == comp.width && height == comp.height);
		}
		
		bool operator < (const Rect& comp) const{
			return overlapNum < comp.overlapNum;
		}
		
		double getWidth() const{
			return width;
		}
		double getHeight() const{
			return height;
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
	};
	
	class DataLabel{
	private:
		string label;
		Point dataPoint;
	
	public:
		Line arrow;
		Rect labelLoc;
		Vector2D movement;
		
		
		DataLabel(){
			label = "";
			arrow = Line();
			labelLoc = Rect();
			movement = Vector2D();
		}
		
		void setLabel(string str){
			label = str;
		}
		string getLabel() const{
			return label;
		}
		
		bool operator > (const DataLabel& comp) const{
			return labelLoc.overlapNum > comp.labelLoc.overlapNum;
		}
		bool operator < (const DataLabel& comp) const{
			return labelLoc.overlapNum < comp.labelLoc.overlapNum;
		}
		bool operator == (const DataLabel& comp) const{
			return labelLoc == comp.labelLoc;
		}
	};
}

#endif /* geometry_h */
