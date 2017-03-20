/*----------------------------------------------------------------------------------*/
/*  Desc:     Functions to check placement legality                                 */
/*                                                                                  */
/*            This script checks the following conditions:                          */
/*              ILLEGAL_TYPE 1: did a terminal or terminal_NI node move?            */
/*              ILLEGAL_TYPE 2: is a movable node flipping/rotated/mirrored?        */
/*              ILLEGAL_TYPE 3: is a movable node placed within the row sites?      */
/*              ILLEGAL_TYPE 4: is a movable node aligned to the circuit rows?      */
/*              ILLEGAL_TYPE 5: is there any overlap among the nodes within a row?  */
/*              ILLEGAL_TYPE 6: is a movable node placed on a multiple of siteX?    */
/*                                                                                  */
/*  Authors:  Myung-Chul Kim, IBM Corporation (mckima@us.ibm.com)                   */
/*                                                                                  */
/*  Created:  05/06/2014                                                            */
/*----------------------------------------------------------------------------------*/

#define _DEBUG
#include "evaluate.h"

inline bool operator<(const row &a, const row &b)
{
	return (a.origY < b.origY) || (a.origY == b.origY && a.origX < b.origX);
}
inline bool operator<(const cell &a, const cell &b)
{
	return (a.y_coord < b.y_coord) || (a.y_coord == b.y_coord && a.x_coord < b.x_coord);
}

/* ************************************************************* */
/* For legality check,                                           */
/* 1) slice multi-row objects into multiple single-row objects,  */
/* 2) and sort rows & objects based on locations                 */
/* NOTE:  assume all macros are rectangular                      */
/* ************************************************************* */
void circuit::sort_and_slice_objects()
{
	unsigned init_num_cell = cells.size();
	for(unsigned i=0 ; i<init_num_cell ; i++)
	//for(vector<cell>::iterator theCell=cells.begin() ; theCell != cells.end() ; ++theCell)
	{
		cell *theCell=&cells[i];
		if(theCell->height == rowHeight)
			continue;

		//NOTE: no movable macros in this contest
		assert(theCell->isFixed);
		//NOTE: the fixed macro should be aligned
		assert((int)((theCell->y_coord-by)/rowHeight)*rowHeight+by == theCell->y_coord);
		if(macros[ theCell->type ].obses.size() <= 1)
		{
			for(int y_coord = theCell->y_coord+rowHeight ; y_coord < theCell->y_coord + theCell->height ; y_coord+=rowHeight)
			{
				cell slicedCell;
				slicedCell.name = theCell->name;
				slicedCell.isFixed = theCell->isFixed;
				slicedCell.x_coord = theCell->x_coord;
				slicedCell.y_coord = y_coord;
				slicedCell.width   = theCell->width;
				slicedCell.height  = rowHeight;
				cells.push_back(slicedCell);
			}
			theCell->height = rowHeight;
		}
		else
		{
			double firstRow_xLL = 0.0;
			double firstRow_xUR = theCell->width / static_cast<double>(LEFdist2Microns);
			for(vector<rect>::iterator theRect=macros[ theCell->type ].obses.begin() ; theRect != macros[ theCell->type ].obses.end() ; ++theRect)
			{
				/*
				if((theRect->yUR-theRect->yLL) * static_cast<double>(LEFdist2Microns) != rowHeight)
				{
					cout << macros[ theCell->type ].name << endl;
					cout << theRect->xUR << " " << theRect->xLL << endl;
					cout << theRect->yUR << " " << theRect->yLL << endl;
					cout << (theRect->yUR-theRect->yLL) * static_cast<double>(LEFdist2Microns)<< " " << rowHeight << endl;
					exit(1);
				}
				assert((theRect->yUR-theRect->yLL) * static_cast<double>(LEFdist2Microns) == rowHeight);
				*/

				if(theRect->yLL == 0.)
				{
					firstRow_xLL = theRect->xLL;
					firstRow_xUR = theRect->xUR;
				}
				else
				{
					cell slicedCell;
					slicedCell.name = theCell->name;
					slicedCell.isFixed = theCell->isFixed;
					slicedCell.x_coord = theCell->x_coord + (unsigned)(theRect->xLL * static_cast<double>(LEFdist2Microns));
					slicedCell.y_coord = theCell->y_coord + (unsigned)(theRect->yLL * static_cast<double>(LEFdist2Microns));
					slicedCell.width   = (unsigned)((theRect->xUR - theRect->xLL) * static_cast<double>(LEFdist2Microns));
					slicedCell.height  = rowHeight;
					cells.push_back(slicedCell);
				}
			}
			theCell->x_coord += (unsigned)(firstRow_xLL * static_cast<double>(LEFdist2Microns));
			theCell->width  = (unsigned)((firstRow_xUR - firstRow_xLL) * static_cast<double>(LEFdist2Microns));
			theCell->height = rowHeight;
		}
	}
	sort(rows.begin(), rows.end());
	sort(cells.begin(), cells.end());
	return;
}

/* ************************************************************* */
/* Performs legality check                                       */
/* save the log in check_legality.log if placement is illegal    */
/* ************************************************************* */

bool circuit::check_legality()
{
	ofstream log("check_legality.log");
	log << fixed;
	// ILLEGAL_TYPE 1: did a terminal node move?                         
	unsigned error_type_1=0;
	cout << "  Testing ILLEGAL_TYPE 1 .. ";
	for(vector<cell>::iterator theCell=cells.begin() ; theCell != cells.end() ; ++theCell)
	{
		if(!theCell->isFixed)
			continue;

		if(theCell->init_x_coord != theCell->x_coord || theCell->init_y_coord != theCell->y_coord) 
		{
			log << "ILLEGAL TYPE 1 : Fixed node " << theCell->name <<" is moved. ";
			log << "(" << theCell->init_x_coord << "," << theCell->init_y_coord << ") -> ";
			log << "(" << theCell->x_coord << "," << theCell->y_coord << ")" << endl;
			error_type_1++;
		}
	}
	cout << error_type_1 <<endl;

  // ILLEGAL_TYPE 2: is a movable node flipping/rotated/mirrored?        
	unsigned error_type_2=0;
	cout << "  Testing ILLEGAL_TYPE 2 .. ";
	for(vector<cell>::iterator theCell=cells.begin() ; theCell != cells.end() ; ++theCell)
	{
		if(theCell->isFixed)
			continue;

		if(theCell->cellorient != "N")
		{
			log << "ILLEGAL TYPE 2 : Movable node " << theCell->name <<" is not in its original orientation. ";
			log << "(" << theCell->cellorient << ")" <<endl;
			error_type_2++;
		}
	}
	cout << error_type_2 <<endl;

  // ILLEGAL_TYPE 3: is a movable node placed aligned to the circuit rows?          
	sort_and_slice_objects();
	unsigned error_type_3=0;
	unsigned row_index=0;
	cout << "  Testing ILLEGAL_TYPE 3 .. ";
	for(vector<cell>::iterator theCell=cells.begin() ; theCell != cells.end() ; ++theCell)
	{
		if(theCell->isFixed)
			continue;

		for( ; row_index<rows.size() ; row_index++)
		{
			row *theRow=&rows[row_index]; 
			if(theCell->y_coord > theRow->origY)
				continue;

			if(theCell->y_coord != theRow->origY)
			{
				log << "ILLEGAL TYPE 3 : Movable node " << theCell->name <<" is not aligned to a circuit row. ";
				log << "(" << theCell->x_coord << ", "<< theCell->y_coord << ")" <<endl;
				error_type_3++;
			}
			break;
		}
		if(theCell->y_coord > rows[rows.size()-1].origY)
		{
			log << "ILLEGAL TYPE 3 : Movable node " << theCell->name <<" is not aligned to a circuit row. ";
			log << "(" << theCell->x_coord << ", "<< theCell->y_coord << ")" <<endl;
			error_type_3++;
		}
	}
	cout << error_type_3 <<endl;

  // ILLEGAL_TYPE 4: is a movable node placed within the row sites?          
	unsigned error_type_4=0;
	row_index=0;
	cout << "  Testing ILLEGAL_TYPE 4 .. ";
	for(vector<cell>::iterator theCell=cells.begin() ; theCell != cells.end() ; ++theCell)
	{
		if(theCell->isFixed)
			continue;

		for( ; row_index<rows.size() ; row_index++)
		{
			row *theRow=&rows[row_index];
			if(theCell->y_coord > theRow->origY)
				continue;

			bool within_row = false;
			while(theCell->y_coord == theRow->origY)
			{
				if(theCell->x_coord >= theRow->origX)
				{
					if(theCell->x_coord+theCell->width <= theRow->origX + theRow->numSites * theRow->stepX)
					{
						within_row = true;
						break;
					}
					break;
				}
				row_index++;
				if(row_index > rows.size()-1)
					break;
				theRow = &rows[row_index];
			}
			if(!within_row)
			{
				log << "ILLEGAL TYPE 4 : Movable node " << theCell->name <<" is not within a row site. ";
				log << "(" << theCell->x_coord << ", "<< theCell->y_coord << ")" << endl;
				error_type_4++;
			}
			break;
		}
	}
	cout << error_type_4 <<endl;
	
	// ILLEGAL_TYPE 5: is there any overlap among the nodes (movable and/or fixed) within a row?  
	cout << "  Testing ILLEGAL_TYPE 5 .. ";
	unsigned error_type_5=0;
	for(vector<cell>::iterator currCell=cells.begin() ; currCell != cells.end()-1 ; ++currCell)
	{
		vector<cell>::iterator nextCell=currCell+1;
		if(currCell->y_coord == nextCell->y_coord && currCell->x_coord + currCell->width > nextCell->x_coord )
		{
			log << "ILLEGAL TYPE 5 : node " << currCell->name <<" overlaps with node " << nextCell->name <<". ";
			log << "(" << currCell->x_coord << " to "<< currCell->x_coord+currCell->width << ", "<< currCell->y_coord << "), (" << nextCell->x_coord <<", "<< nextCell->y_coord << ")" << endl;
			error_type_5++;
		}
	}
	cout << error_type_5 <<endl;

  // ILLEGAL_TYPE 6: is a movable node placed within the row sites?          
	unsigned error_type_6=0;
	row_index=0;
	cout << "  Testing ILLEGAL_TYPE 6 .. ";
	for(vector<cell>::iterator theCell=cells.begin() ; theCell != cells.end() ; ++theCell)
	{
		if(theCell->isFixed)
			continue;

		for( ; row_index<rows.size() ; row_index++)
		{
			row *theRow=&rows[row_index];
			if(theCell->y_coord > theRow->origY)
				continue;

			bool site_aligned = false;
			while(theCell->y_coord == theRow->origY)
			{
				if(theCell->x_coord >= theRow->origX)
				{
					if(theCell->x_coord == theRow->origX + int((theCell->x_coord - theRow->origX)/theRow->stepX) * theRow->stepX)
					{
						site_aligned = true;
						break;
					}
					break;
				}
				row_index++;
				if(row_index > rows.size()-1)
					break;
				theRow = &rows[row_index];
			}
			if(!site_aligned)
			{
				log << "ILLEGAL TYPE 6 : Movable node " << theCell->name <<" is not aligned to a row site. ";
				log << "(" << theCell->x_coord << ", "<< theCell->y_coord << ")" << endl;
				error_type_6++;
			}
			break;
		}
	}
	cout << error_type_6 <<endl;

	log.close();
	if((error_type_1+error_type_2+error_type_3+error_type_4+error_type_5+error_type_6) == 0)
	{
		system("rm check_legality.log");
		return true;
	}
	return false;
}

