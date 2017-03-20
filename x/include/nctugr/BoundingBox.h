#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <vector>
#include <cmath>
#include <cstdlib>

#include "GridGraph.h"
#include "Point.h"

struct BoundingBox
{
	Point TopRight ; 
	Point BottomLeft ;
	bool expandLeft;
	bool expandRight;
	bool expandTop;
	bool expandBottom;
	
	bool inside( const Point& n );
	bool inside( const RPoint& n );
	bool inside( const int x, const int y );
	bool inside( const BoundingBox& smallbox );
	
	int size();
	int numEdge();
	int numHorEdge();
	int numVerEdge();
	
	int distance( int x, int y, GridCell ***GCell, int netCount );
	int distance( const Point& p );
	
	void Bondary( vector<Point>& points  );
	void Bondary( int x, int y  );
	
	BoundingBox( vector<Point>& points ){
		Bondary( points );
	}
	
	BoundingBox(){
		TopRight.x=-1 ;
		TopRight.y=-1 ;
		BottomLeft.x=INT_MAX ;
		BottomLeft.y=INT_MAX ;

		expandLeft=true;
		expandRight=true;
		expandTop=true;
		expandBottom=true;
	}
};

#endif // #ifndef BOUNDINGBOX_H

