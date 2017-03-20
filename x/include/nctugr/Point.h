#ifndef POINT_H
#define POINT_H

//#include <cmath>
#include <math.h>
#include <iostream>
#include <string>

//int abs( int a ); // { return ( (a<0) ? a*(-1) : a ) ; }


class Point {
public:
	int x, y, z; 
//	bool isOriginal;		//--
//	int degree ;			// degree of Pin

	Point( void ) ;
	Point( int x, int y ) ;
	Point( int x, int y, int z ) ;
	Point( const Point& p ) ;
	int distance( const Point& p ) ;
	  ~Point() { }
};


struct Point_f {
	float x,y,z;
	Point_f(){ x=0; y=0; z=0;};
};

struct Point2D_f {
	float x,y;
	Point2D_f(){ x=0; y=0;};
};

struct Point_i {
     int x,y,z;
     Point_i(int i, int j, int k){ x=i; y=j; z=k;};
     Point_i(){ x=0; y=0; z=0;};
	 bool operator ==(const Point_i &t){
          return t.x == x && t.y == y && t.z == z;
     }

     Point_i operator =(const Point_i &t){
          x = t.x;
          y = t.y;
          z = t.z;

          return t;
     }

};

struct hisRecord {
     int x, y, hori, value;
     int normalizeValue; // 1~100
};

bool operator< ( const hisRecord& a, const hisRecord& b );
/*
struct hisRecordt {
     int x, y, hori, value;
     int normalizeValue; // 1~100
	hisRecordt(){}
	bool operator >(const hisRecordt &t);
};
*/


/*

class Point {
public:
  int x, y ; 

  //std::string name ;

  bool isOriginal;		//--
  int degree ;			// degree of Pin

  Point( void ) {
    x = y = 0 ;
    isOriginal = false ;
    degree = 0 ;
  }
  Point( int x, int y ) : x(x), y(y) {
    isOriginal = false; 
    degree = 0 ;
    //createName(x, y) ;
  }
  Point( const Point& p ) {
    x = p.x ;
    y = p.y ;
    isOriginal = p.isOriginal;
    degree = p.degree ;
    //name = p.name ;
  }
	int distance( const Point& p ) {
		//return std::abs(p.x-x) + std::abs(p.y-y) ;			// GCC4.1.2
		return abs(p.x-x) + abs(p.y-y) ;			// GCC4.4.2, GCC4.4.3 v1	
		//return std::abs((float)(p.x-x)) + std::abs((float)(p.y-y)) ;	// GCC4.4.2, GCC4.3.3 v2
  }
  ~Point() { }
 
};
*/

std::ostream& operator<< ( std::ostream& os, const Point &p ) ;
bool operator==( const Point& a, const Point& b ) ;
bool operator!=( const Point& a, const Point& b ) ;
bool operator<( const Point& a, const Point& b ) ;

struct AdjustCapEdge{
	int x, y, z;
	bool hori;
	int value;
};


class RPoint {
public:
	//bool hori ;
	int hori ;
	int x, y, z;

	RPoint() ;
	RPoint( int x, int y, int h ) ;
	RPoint( int x, int y, int z, int h ) ;
	RPoint(const RPoint& p) ;
	int distance( RPoint& p ); 
};



// Route Point
/*
class RPoint {
public:
  bool hori ;
  int x, y ;
  RPoint() {

  }
  RPoint( int x, int y, bool h ) : x(x), y(y), hori(h) {

  }
  RPoint(const RPoint& p) {
    x = p.x ;
    y = p.y ;
    hori = p.hori ;
  }
	int distance( RPoint& p ) {
		//return std::abs(p.x-x) + std::abs(p.y-y) ;			//GCC4.1.2
		return abs(p.x-x) + abs(p.y-y) ;			//GCC4.4.2, GCC4.4.3 v1	
		//return std::abs((float)(p.x-x)) + std::abs((float)(p.y-y)) ;	//GCC4.3.2, GCC4.4.3 v2
  }
};
*/

std::ostream& operator<< ( std::ostream& os, const RPoint &p ) ;
bool operator< ( const RPoint& a, const RPoint& b ) ;

struct eqRPoint {
  bool operator()( const RPoint& a, const RPoint& b ) const {
    return a.x == b.x && a.y == b.y && a.z == b.z  && a.hori == b.hori;
  }
};


struct RPointHash {
  size_t operator()( const RPoint& p ) const {
//    int b = (p.hori == true ) ;
//    return p.y+(p.x-p.y)^(p.x+p.y)-p.x+b ;
    		return p.x*6000+p.y*3 + p.hori;
    	//return 0;
  }
};
/*
// Maze Point 
class MPoint {
public:
  int cost ;
  int cost2 ;
  int x, y ;
  int back ;
  MPoint() {

  }
  MPoint( int x, int y, int cost, int back ) : x(x), y(y), cost(cost), back(back) {

  }
  MPoint( const MPoint& p ) {
    x = p.x ;
    y = p.y ;
    cost = p.cost ;
    cost2 = p.cost2 ;
    back = p.back ;
  }
};

std::ostream& operator<< ( std::ostream& os, const MPoint &p ) ;
bool operator< ( const MPoint& a, const MPoint& b ) ;
*/
/*
inline bool operator==( const Point& a, const Point& b ) {
  return a.x == b.x && a.y == b.y ;
}

inline bool operator!=( const Point& a, const Point& b ) {
  return !(a == b) ;
}

inline std::ostream& operator<< ( std::ostream& os, const Point &p ) {
  return os << "(" << p.x << "," << p.y << ")" ;
}

inline std::ostream& operator<< ( std::ostream& os, const RPoint &p ) {
  return os << (p.hori ? 'h' : 'v') << "(" << p.x << "," << p.y << ")" ;
}

inline std::ostream& operator<< ( std::ostream& os, const MPoint &p ) {
  return os << "(" << p.x << "," << p.y << ")" << p.cost ;
}

inline bool operator< ( const MPoint& a, const MPoint& b ) {
  return a.cost+a.cost2 > b.cost+b.cost2 ;
}
*/


#endif
