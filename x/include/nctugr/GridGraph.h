// Jucemar Monteiro 2016-08-17
// Added to ignore some warning messages in this file.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#ifndef GRIDGRAPH_H
#define GRIDGRAPH_H

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <vector>
#include <cmath>
#include <limits.h>

#include "pheap.h"
#include "Point.h"

using namespace std ;

struct GridCell
{
	int rank ;
	int flag ;
	//int flag2 ;
	int end ;
	//bool traced ;
	bool inHeap ;
	//bool inque ;
	float pathCost ;
	float totalCost ;
	int distance ;
	int wlen ;
	int x_axis, y_axis, z_axis ;
	 
	GridCell *fromCell ;
	GridCell *parentCell ;

	//fibheap_el *myel ;
	pheap_el myel ;

} ;

class FuncMapChangeRecord {		// new by TK
public:
	float data;
	int   position;
	int   direction;
};

class GridGraph {
	private:
  
  
public:
	int **h_edge    , **v_edge;  // owokko change it form private to public


	int h_cap, v_cap ;


	int xNum, yNum, zNum ;
	Point2D_f* func_2Dmap;
	//float* func_2Dmap;
	Point_f* func_map;
	bool* in_change_map;
	//vector<Point_f> func_map;
	vector<int> func_map_change_record;
	vector<int> func_2Dmap_change_record;
	vector< vector<vector< Point_i> > > demand_map;
	vector< vector<vector< Point_i> > > capacity_map;
	vector< vector<vector< Point_i> > > history_map;
	vector<vector< Point_i> >  demand_2Dmap;
	vector<vector< Point_i> >  capacity_2Dmap;
	GridCell ***bottomCell ;

	int* tk_change_map;
	FuncMapChangeRecord* record;
	int recordNum;

	GridGraph( int x, int y, int layer) : xNum(x), yNum(y), zNum(layer) { 

		h_edge = new int*[xNum-1] ;
		for( int i=0; i<xNum-1; i++ ){
			h_edge[i] = new int[yNum] ;
			memset( h_edge[i], 0, sizeof(int)*yNum ) ;
		}

		v_edge = new int*[xNum] ;    
		for( int i=0; i<xNum; i++ ){
			v_edge[i] = new int[yNum-1] ;
			memset( v_edge[i], 0, sizeof(int)*(yNum-1) ) ;
		}    
	
		func_2Dmap=new Point2D_f[x*(y)];	
		//func_2Dmap=new float[x*(y)*2];	
		func_map=new Point_f[x*y*layer];
		in_change_map= new bool[x*y*layer];
		memset( in_change_map, 0, sizeof(bool)*x*y*layer ) ;

		//func_map.resize(x*y*layer);
		/*
		demand_map.resize(layer);
		capacity_map.resize(layer);
		history_map.resize(layer);
		for( int i=0; i<layer; i++ )
		{
			demand_map[i].resize(y);
			capacity_map[i].resize(y);
			history_map[i].resize(y);
			for( int j=0; j<y; j++ )
			{
				demand_map[i][j].resize(x);
				capacity_map[i][j].resize(x);
				history_map[i][j].resize(x);
			}
				
		}
		
		demand_2Dmap.resize(y);
		capacity_2Dmap.resize(y);
		for( int j=0; j<y; j++ )
		{
			  demand_2Dmap[j].resize(x);
                 capacity_2Dmap[j].resize(x);
		}
		*/
		
		bottomCell = new GridCell**[zNum] ;
		for(int l=0; l<zNum; l++)
		{
			bottomCell[l] = new GridCell*[x] ;
			for( int i=0; i<x; i++ ){
				bottomCell[l][i] = new GridCell[y] ;
				for( int j=0; j<y; j++ ){
					bottomCell[l][i][j].flag = -1 ;
					//bottomCell[l][i][j].flag2 = -1 ;
					bottomCell[l][i][j].end = -1 ;
					//bottomCell[l][i][j].traced = false ;
					bottomCell[l][i][j].x_axis = i ;
					bottomCell[l][i][j].y_axis = j ;
					bottomCell[l][i][j].z_axis = l ;
					bottomCell[l][i][j].inHeap = false ;
					bottomCell[l][i][j].pathCost = 0 ;
					bottomCell[l][i][j].totalCost = static_cast<float>(INT_MAX) ;
					bottomCell[l][i][j].fromCell = NULL ;
					bottomCell[l][i][j].parentCell = NULL ;
					//bottomCell[l][i][j].myel = new fibheap_el ;
					bottomCell[l][i][j].myel.data =&bottomCell[l][i][j] ;
					//bottomCell[l][i][j].parentCell = NULL ;
				}
			}
		}

		// TK:
		tk_change_map = new int[x*y*layer*3];
		memset( tk_change_map, 0, sizeof(int)*x*y*layer*3 );

	}

	

	~GridGraph(){
		//printf("start ~gridgraph\n"); fflush(stdout);
		delete [] tk_change_map;
          for( int i=0; i<xNum-1; i++ ){
              	delete [] h_edge[i] ;
          }
		delete [] h_edge;

          for( int i=0; i<xNum; i++ ){
              	delete [] v_edge[i] ;
          }
          delete [] v_edge ;

		delete [] func_2Dmap;
		delete [] func_map;
		delete [] in_change_map;

          for(int l=0; l<zNum; l++)
          {
               for( int i=0; i<xNum; i++ ){
				/*
                    for( int j=0; j<xNum; j++ ){
                         delete bottomCell[l][i][j].myel;
                    }
				*/
                    delete [] bottomCell[l][i] ;
               }
               delete [] bottomCell[l];
          }
		delete []  bottomCell;


	}

	int getHCap( void ){
		return h_cap ;
	}

	int getVCap( void ){
		return v_cap ;
	}

	int getXNum( void ){
		return xNum ;
	}

	int getYNum( void ){
		return yNum ;
	}

	
	void resetCellInformation(){
		for(int l=0;l<zNum;l++)
		for(int i=0;i<xNum;i++)
		for(int j=0;j<yNum;j++){
			bottomCell[l][i][j].flag = -1 ;
			bottomCell[l][i][j].end = -1 ;
      		}
  	}





  void resetCost()
  {
    for ( int i = 0 ; i < xNum - 1 ; ++i ) {
      for ( int j = 0 ; j < yNum ; ++j ) {
        h_edge[i][j]=0;       
      }
    }
    for ( int i = 0 ; i < xNum ; ++i ) {
      for ( int j = 0 ; j < yNum - 1 ; ++j ) {
        v_edge[i][j]=0;
      }
    }
  }

  bool addCost( int x, int y, int c, bool hori ) 
  {
	  if ( hori )
	  {
		  h_edge[x][y] += c ;
		  //return (h_edge[x][y] > h_cap) ;
	  }
	  else
	  {
		  v_edge[x][y] += c ;
		  //return  (v_edge[x][y] > v_cap) ;
	  }
  }


  bool add3DCost( int x, int y, int z, int c, int hori )
  {
   	  if ( hori==1 )
       {
            demand_map[z][y][x].x += c ;
            return (demand_map[z][y][x].x > capacity_map[z][y][x].x) ;
       }
       else if (hori==0)
       {
            demand_map[z][y][x].y += c ;
            return (demand_map[z][y][x].y > capacity_map[z][y][x].y) ;
       }
	  else if (hori==2)
	  {
		  demand_map[z][y][x].z += c ;
            return (demand_map[z][y][x].z > capacity_map[z][y][x].z) ;
	  }
	  else
		 printf("ERROR %s %d\n", __FILE__, __LINE__);

	 return false;
  }



  bool add2DCost( int x, int y, int c, int hori )
  {
       if ( hori==1 )
       {
            demand_2Dmap[y][x].x += c ;
            return (demand_2Dmap[y][x].x > capacity_2Dmap[y][x].x) ;
       }
       else if (hori==0)
       {
            demand_2Dmap[y][x].y += c ;
            return (demand_2Dmap[y][x].y > capacity_2Dmap[y][x].y) ;
       }
       else
           printf("ERROR %s %d\n", __FILE__, __LINE__);

      return false;
  }

  
  void setCost( int x, int y, int c,  bool hori ) {
  
    cout<<"If call this function,please tell Liu Wen-Hao,thank you";
    cout<<"If call this function,please tell Liu Wen-Hao,thank you";
    getchar();
    if ( hori ) {
      h_edge[x][y] = c ;
    }
    else
      v_edge[x][y] = c ;
  }
  int getCost( int x, int y, bool hori ) {

	  assert( x < xNum && x >= 0 ) ;
	  assert( y < yNum && y >= 0 ) ;

	  if ( hori ){
		  return h_edge[x][y] ;
	  }
	  else{
		  return v_edge[x][y] ;
	  }
  }
  

  int getOverflow( void ) {
    int overflow = 0 ;

    for ( int i = 0 ; i < xNum - 1 ; ++i ) 
      for ( int j = 0 ; j < yNum ; ++j ) 
	if ( h_edge[i][j] > h_cap ) 
	  overflow += (h_edge[i][j] - h_cap) ;
    
    for ( int i = 0 ; i < xNum ; ++i )
      for ( int j = 0 ; j < yNum - 1 ; ++j ) 
	if ( v_edge[i][j] > v_cap ) 
	  overflow += (v_edge[i][j] - v_cap) ;

    return overflow ;
  }



  int getWirelength( int &maxOf ) {
    int length = 0 ;
    maxOf = 0 ;
    for ( int i = 0 ; i < xNum - 1 ; ++i )
      for ( int j = 0 ; j < yNum ; ++j ) {
    	  length += h_edge[i][j] ;

        if( (h_edge[i][j]-h_cap) > maxOf )
          maxOf = h_edge[i][j] - h_cap ;
      }

    for ( int i = 0 ; i < xNum ; ++i )
      for ( int j = 0 ; j < yNum - 1 ; ++j ) {
      	length += v_edge[i][j] ;

        if( (v_edge[i][j]-v_cap) > maxOf )
          maxOf=v_edge[i][j]-v_cap;
      }

    return length ;
  }
  
  //0516
  void saveGridGraph( const string& filename ) {
      ofstream fout( filename.c_str(), ios::out ) ;

      fout << xNum << " " << yNum << endl ;
      for ( int i = 0 ; i < xNum ; i++ ) {
          for ( int j = 0 ; j < yNum ; j++ ) {
              fout << h_edge[i][j] << " " ;
          }
          fout << endl ;
      }
      fout << endl ;

      for ( int i = 0 ; i < xNum ; i++ ) {
          for ( int j = 0 ; j < yNum ; j++ ) {
              fout << v_edge[i][j] << " " ;
          }
          fout << endl ;
      }
      fout << endl ;

      fout.close() ;
  }

};
#endif


// Jucemar Monteiro 2016-08-17
// Added to ignore some warning messages in this file.
#pragma GCC diagnostic pop