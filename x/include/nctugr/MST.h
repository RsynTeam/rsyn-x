#ifndef DISJOINT_H
#define DISJOINT_H

#include <iostream>
#include <vector>
#include <map>
#include <stdlib.h>
#include <algorithm>
#include "Point.h"
using namespace std;


class DisjointSet {
private:
  std::vector<int> parent ;
  std::vector<int> rank ;  
public:
  void AddSet( void );
  void AddSet( int size);
  void Union( int x, int y );
  int Find( int x );
  void clear();
};

struct MSTedge{
  int fromIndex, toIndex;
  double length;
};


class nctuMST
{
  public:
    vector<MSTedge> edgeArray;
    vector<MSTedge> treeEdge;

    void buildGraph(vector<Point> &nodeArray);
    double buildTree();

    void clear();

   private:
	DisjointSet dis;
	int numPin;

    //list<int> nodeSet;
    void connectNodeInR1(int index, multimap<int, int> &nodeSet, vector<Point> &nodeArray);
    void connectNodeInR2(int index, multimap<int, int> &nodeSet, vector<Point> &nodeArray);
    void connectNodeInR3(int index, multimap<int, int> &nodeSet, vector<Point> &nodeArray);
    void connectNodeInR4(int index, multimap<int, int> &nodeSet, vector<Point> &nodeArray);
    
    void buildCompleteGraph( vector<Point> &nodeArray);
    void buildSpaningGraph( vector<Point> &nodeArray);

    multimap<int, int> nodeSetX;
    multimap<int, int> nodeSetY;
    vector< std::pair<int, int> > sortArray;
    //std::multimap<int, int>::iterator it

};


#endif
