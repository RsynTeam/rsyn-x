#ifndef NET_H
#define NET_H

#include <iostream>
#include <vector>
#include <set>
//#include <ext/hash_set>
#include <tr1/unordered_set>
#include <tr1/unordered_map>
#include "Point.h"
#include "BoundingBox.h"
#include <limits.h>
#include <string>
#include "Cell.h"
using namespace std ;
//using namespace __gnu_cxx ;

const int WATCHED_NET3 = -1 ;
typedef struct Net net;

// Two-Pin Net
struct TwoPin {

	Point from, to ;
	int fromIndex, toIndex;
	int wlen ;
	int ripup ;	// in 2d
	int maxLength;
	int overflow ;
	bool inOverflowBox ;
	//int nindex ;			// index of Net i 
	Net *parNet;
	float pathScore ;
	bool reroute ;
	int active_gpu_blk ;
	int crossing_blk;
	vector<RPoint> path ;
	BoundingBox br;
	int routeType;
	//bool fromHori;
	//bool toHori;

	int wakeupCountDown;
	int sleepCount;
	int lastReRouteRound;

	TwoPin() {
		overflow = 0 ;  
		ripup = 0 ;
		inOverflowBox = true ;
		reroute = true ;
		active_gpu_blk = 0;
		crossing_blk = 0;
		routeType=0;   // 1 one-direction,   2 two-direction,  3 maze

		//fromHori=false;
		//toHori=true;

	}

	TwoPin( const TwoPin& t) {
		overflow = 0 ;  
		overflow = t.overflow ;
		from = t.from ;
		to = t.to ;
		ripup = 0 ;
		inOverflowBox = true ;
		reroute = true ;
		active_gpu_blk = t.active_gpu_blk;
		crossing_blk = t.crossing_blk;
		routeType=t.routeType;
		parNet= t.parNet;
		fromIndex=t.fromIndex;
		toIndex=t.toIndex;	
	}

	/*
	~TwoPin() {
		path.clear();
		//printf("it is call\n");
	}
	*/	

	int HPWL() {
		return abs(from.x-to.x)+abs(from.y-to.y);
	} 
	
} ;

struct hashCount{//a hash function: function.
  size_t operator() (const int a) const {
    return a;
  }
};

struct eqCount{
  bool operator() (const int a, const int b) const {
     return a == b;
  }
};



class Net {

public:
	int id ;
	string name;
	vector< Point > pins ;
	vector< Point > pin3D ;
	vector<pinLocate> pinArray;
	vector< TwoPin > twopin ;
	bool reroute ;
	int wlen ;
	//int maxWlen ;
	int topologyWL ;
	//int overflow ;

	// TK
	//int minlayer;
	//int maxlayer;	

     tr1::unordered_map<int, int> currentNet[2] ;
     //tr1::unordered_map<int, int> historyPath[2] ;
     //tr1::unordered_map<int, int, hashCount, eqCount> currentNet[2] ;
     //tr1::unordered_multiset<RPoint, RPointHash, eqRPoint> currentNet ;
	//hash_multiset<Seg, RPointHash, eqSeg> currentNet ;
     tr1::unordered_multiset<RPoint, RPointHash, eqRPoint> flute ;

	Net() {
		wlen = 0 ;
		topologyWL = 0 ;
	}

/*
	Net( string& n ) : name(n){
		wlen=0;
		fluteWL=0;
	}
*/


	//Net( string& n, int i ) : name(n), id(i){
	Net( int i ) : id(i){
		wlen=0;
		topologyWL=0;
	}  

	/*
	 ~Net(){
		pins.clear();	
		pin3D.clear();
		twopin.clear();
		currentNet.clear();
		flute.clear();	
	}
	*/  

	bool findThePin( const Point& p ){ //--
		int pins_size=pins.size();
		for( int i = 0 ; i < pins_size ; i++ ){
		//   if( pins[i] == p )
	 		//if( p.x==pins[i].x && p.y==pins[i].y && p.z==pins[i].z )
	 		if( p.x==pins[i].x && p.y==pins[i].y )
				return true ;
		}    
		return false ;
	}


	bool findThe3DPin( const Point& p ){ //--
		int pins_size=pin3D.size();
          for( int i = 0 ; i < pins_size ; i++ ){
               if( p.x==pin3D[i].x && p.y==pin3D[i].y && p.z==pin3D[i].z )
                    return true ;
          }
          return false ;
     }

/*
	void addPin( const Point& p ){
		pins.push_back(p) ;
	}
*/

} ;

struct Order
{
     TwoPin* n ;
     double cost ;

} ;

static bool operator< ( const Order& a, const Order& b ) {
     if ( a.cost == b.cost )
          return a.n->HPWL() > b.n->HPWL() ;
     return a.cost > b.cost ;
}


static bool operator< (const Net& a,const Net& b ) {
  //return a.wlen < b.wlen;

  if(a.pins.size() == b.pins.size() && a.pins.size() > 1)
  {
    int a_short;
    int b_short;
    if( abs(a.pins[0].x - a.pins[1].x ) <  abs(a.pins[0].y - a.pins[1].y ))
      a_short=abs(a.pins[0].x - a.pins[1].x );
    else
      a_short=abs(a.pins[0].y - a.pins[1].y );

   if( abs(b.pins[0].x - b.pins[1].x ) <  abs(b.pins[0].y - b.pins[1].y ))
      b_short=abs(b.pins[0].x - b.pins[1].x );
    else
      b_short=abs(b.pins[0].y - b.pins[1].y );

     return  a_short<b_short;

  }
  return a.pins.size() > b.pins.size();
}
#endif
