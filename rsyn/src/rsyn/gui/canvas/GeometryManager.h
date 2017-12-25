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

#ifndef RSYN_GEOMETRY_MANAGER_H
#define RSYN_GEOMETRY_MANAGER_H

#ifdef __WXMAC__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <tuple>
#include <map>
#include <vector>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

//BOOST_GEOMETRY_REGISTER_POINT_2D(DBUxy, std::int64_t, cs::cartesian, x, y)

#include "rsyn/util/Color.h"
#include "rsyn/util/dbu.h"
#include "rsyn/util/float2.h"
#include "rsyn/util/FloatRectangle.h"

#include "Stipple.h"

// -----------------------------------------------------------------------------

class GeometryManager {
public:
	typedef int LayerId;
	typedef std::tuple<LayerId, int> ObjectId; // <layer id, object id>
	typedef int GroupId;

	typedef bg::model::point<float, 2, bg::cs::cartesian> Point;
	typedef bg::model::box<Point> Box;

	typedef bg::model::d2::point_xy<float> PolygonPoint;
	typedef bg::model::polygon<PolygonPoint> Polygon;

	static const GroupId INVALID_GROUP_ID;
	static const ObjectId INVALID_OBJECT_ID;

	enum ObjectType {
		OBJECT_TYPE_INVALID = -1,

		RECTANGLE,
		POLYGON,
		PATH
	}; // end enum

	enum BoxOrientation {
		BOX_ORIENTATION_INVALID,
		BOX_ORIENTATION_SW,
		BOX_ORIENTATION_SE,
		BOX_ORIENTATION_NE,
		BOX_ORIENTATION_NW
	};

	GeometryManager();

	void reset();
	void removeAllObjects();

	LayerId createLayer(
			const std::string &name,
			const int zIndex = 0,
			const Color lineColor = Color(0, 0, 0),
			const Color fillColor = Color(0, 0, 0),
			const LineStippleMask linePattern = LINE_STIPPLE_SOLID,
			const FillStippleMask fillPattern = STIPPLE_MASK_EMPTY);

	GroupId createGroup();
	void addObjectToGroup(const ObjectId objectId, const GroupId groupId);

	ObjectId addRectangle(const LayerId layerId, const Box &box, void * data = nullptr, const BoxOrientation orientation = BOX_ORIENTATION_INVALID, const GroupId groupId = INVALID_GROUP_ID);
	ObjectId addPolygon(const LayerId layerId, const std::vector<DBUxy> &points, const float2 displacement, void * data = nullptr, const GroupId groupId = INVALID_GROUP_ID);
	ObjectId addPath(const LayerId layerId, const std::vector<DBUxy> &points, const float thickness, void * data = nullptr, const GroupId groupId = INVALID_GROUP_ID);

	ObjectId searchObjectAt(const float x, const float y) const;

	void addObjectToHighlight(const ObjectId objectId);
	void addGroupOfObjectToHighlight(const ObjectId objectId);
	void addGroupToHighlight(const GroupId groupId);
	void clearHighlight() { highlightedObjects.clear(); }

	void setLayerVisibility(const LayerId layerId, const bool visible) {
		layers[layerId].visible = visible;
	}
	void setLayerVisibility(const std::string &layerName, const bool visible) {
		auto it = mapLayerNameToLayerId.find(layerName);
		if (it != mapLayerNameToLayerId.end()) {
			layers[it->second].visible = visible;
		} // end if
	} // end method

	void setObjectFillColor(const ObjectId &objectId, const Color &color) {
		Object &object = getObject(objectId);
		object.hasFillColor = true;
		object.fillColor = color;
	} // end method

	void clearObjectFillColor(const ObjectId &objectId) {
		Object &object = getObject(objectId);
		object.hasFillColor = false;
	} // end method

	void setObjectLineColor(const ObjectId &objectId, const Color &color) {
		Object &object = getObject(objectId);
		object.hasLineColor = true;
		object.lineColor = color;
	} // end method

	void clearObjectLineColor(const ObjectId &objectId) {
		Object &object = getObject(objectId);
		object.hasLineColor = false;
	} // end method

	void * getObjectData(const ObjectId &objectId) const {
		return isObjectIdValid(objectId)?
			layers[std::get<0>(objectId)].objects[std::get<1>(objectId)].data :
			nullptr;
	} // end method

	static bool isObjectIdValid(const ObjectId &objectId) {
		return objectId != INVALID_OBJECT_ID;
	} // end method

	static bool isGroupIdValid(const GroupId &groupId) {
		return groupId != INVALID_GROUP_ID;
	} // end method

	void render() const;
	void renderFocusedObject(const ObjectId &objectId) const;
	void renderHighlightedObjects() const;

private:

	typedef std::pair<Box, ObjectId> RTreeEntry;

	struct Object {
		ObjectType type = OBJECT_TYPE_INVALID;
		GroupId groupId = INVALID_GROUP_ID;
		Box box;
		Polygon polygon;
		float area = 0;

		bool hasFillColor : 1;
		bool hasLineColor : 1;
		BoxOrientation orientation : 3;

		Color fillColor;
		Color lineColor;

		// TODO: Maybe replace this by an OpenGL genlist (seeglGenLists).
		std::vector<std::tuple<GLenum, int>> tessellationType;
		std::vector<float2> tessellationPoints;

		// Client data.
		void * data = nullptr;

		Object() :
				hasFillColor(false),
				hasLineColor(false),
				orientation(BOX_ORIENTATION_INVALID) {}
	}; // end struct

	struct Layer {
		std::string name;
		FillStippleMask fillPattern = STIPPLE_MASK_EMPTY;
		LineStippleMask linePattern = LINE_STIPPLE_SOLID;
		Color fillColor;
		Color lineColor;
		int zIndex = 0;
		bool visible = true;

		float z = 0;

		std::deque<Object> objects;
		bgi::rtree<RTreeEntry, bgi::quadratic<16>> rtree;
	}; // end struct

	std::vector<Layer> layers;
	std::vector<std::set<ObjectId>> groups;
	std::map<std::string, LayerId> mapLayerNameToLayerId;
	std::set<ObjectId> highlightedObjects;

	GLUtriangulatorObj *tobj = nullptr;

	mutable Color defaultColor;

	Object &getObject(const ObjectId &objectId) { 
		return layers[std::get<0>(objectId)].objects[std::get<1>(objectId)];
	} // end method

	const Object &getObject(const ObjectId &objectId) const {
		return layers[std::get<0>(objectId)].objects[std::get<1>(objectId)];
	} // end method

	Layer &getLayer(const LayerId &layerId) {
		return layers[layerId];
	} // end method

	const Layer &getLayer(const LayerId &layerId) const {
		return layers[layerId];
	} // end method

public:
	LineStippleMask getLayerLinePattern (const LayerId &layerId) const {
		return layers[layerId].linePattern;
	} // end method
	
	Color getLayerLineColor (const LayerId &layerId) const {
		return layers[layerId].lineColor;
	} // end method
	
	FillStippleMask getLayerFillPattern (const LayerId &layerId) const {
		return layers[layerId].fillPattern;
	} // end method
	
	Color getLayerFillColor (const LayerId &layerId) const {
		return layers[layerId].fillColor;
	} // end method
	
	float getLayerZ(const LayerId &layerId) const {
		return layers[layerId].z;
	}
	
private:
	void resortLayers();

	void applyColor(const Color &color) const {
		glColor3ub(color.r, color.g, color.b);
	} // end method

	void applyDefaultColor(const Color &color) const {
		defaultColor = color;
		glColor3ub(color.r, color.g, color.b);
	} // end method

	void restoreDefaultColor() const {
		glColor3ub(defaultColor.r, defaultColor.g, defaultColor.b);
	} // end method

	void renderRectangleOutline(const Layer &layer, const Object &object) const;
	void renderPolygonOutline(const Layer &layer, const Object &object) const;
	void renderPathOutline(const Layer &layer, const Object &object) const;

	void renderRectangleFill(const Layer &layer, const Object &object) const;
	void renderPolygonFill(const Layer &layer, const Object &object) const;
	void renderPathFill(const Layer &layer, const Object &object) const;

	void tracePathOutline(
			const std::vector<DBUxy> &pathPoints,
			const float halfThickness,
			std::vector<float2> &outlinePoints) const;
	bool findMyPoint(float2 v0, float2 v1, float2 v2, const float thickness, std::vector<float2> &outlinePoints) const;
	float2 findIntersection(float2 p1, float2 p2, float2 d1, float2 d2) const;

	void initTessellator();
	void tessellate(Object &object);
	static void tessellate_beginCallback(GLenum type, void * data);
	static void tessellate_endCallback(void * data);
	static void tessellate_vertexCallback(GLdouble * vertex, void * data);
	static void tessellate_errorCallback(GLenum errorCode);

}; // end method

#endif /* GEOMETRYMANAGER_H */

