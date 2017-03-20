/* Copyright 2014-2017 Rsyn
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Polygon.h
 * Author: jucemar
 *
 * Created on 2 de Setembro de 2016, 12:48
 */

#ifndef POLYGON_H
#define POLYGON_H
#include "dbu.h"

class Polygon {
protected:
	std::vector<DBUxy> clsPoints;
	
public:
	void resize(const int size) { clsPoints.resize(size); }
	void reserve(const int size) {clsPoints.reserve(size); }
	void addPoint(DBUxy point) { clsPoints.push_back(point); }
	void addPoint(DBUxy point, const int pos) { clsPoints[pos] = point; }
	void clear() { clsPoints.clear(); }
	
	DBUxy getPoint(const int pos) const { return clsPoints[pos]; }
	const std::vector<DBUxy> & allPoints () const { return clsPoints; }
	
	int getNumPoints() const { return clsPoints.size(); }
	bool hasPoints() const { return !isEmpty(); }
	bool isEmpty() const { return clsPoints.size() == 0; }
	
};

#endif /* POLYGON_H */

