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

#include "GeometryManager.h"
#include "rsyn/util/Stepwatch.h"

// -----------------------------------------------------------------------------

const GeometryManager::GroupId GeometryManager::INVALID_GROUP_ID = -1;
const GeometryManager::ObjectId GeometryManager::INVALID_OBJECT_ID = std::make_tuple(-1, -1);

// -----------------------------------------------------------------------------

GeometryManager::GeometryManager() {
	initTessellator();
} // end constructor

// -----------------------------------------------------------------------------

void GeometryManager::reset() {
	layers.clear();
	mapLayerNameToLayerId.clear();
	highlightedObjects.clear();
	groups.clear();
} // end method

// -----------------------------------------------------------------------------

void GeometryManager::removeAllObjects() {
	highlightedObjects.clear();
	groups.clear();
	for (Layer &layer : layers) {
		layer.objects.clear();
		layer.rtree.clear();
	} // end for
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::tessellate_beginCallback(GLenum type, void * data) {
	GeometryManager::Object &object = *((GeometryManager::Object *) data);
	object.tessellationType.push_back(std::make_tuple(type, 0));

	//std::cout << "POLYGON: " << data << " " << ((int) type) << "\n";
} // end function

// -----------------------------------------------------------------------------

void
GeometryManager::tessellate_endCallback(void * data) {
	GeometryManager::Object &object = *((GeometryManager::Object *) data);
	std::get<1>(object.tessellationType.back()) = object.tessellationPoints.size();
} // end function

// -----------------------------------------------------------------------------

void
GeometryManager::tessellate_vertexCallback(GLdouble * vertex, void * data) {
	GeometryManager::Object &object = *((GeometryManager::Object *) data);
	object.tessellationPoints.push_back(
			float2((float) vertex[0], (float) vertex[1]));

	//std::cout << "  point: " << vertex[0] << ", " << vertex[1] << "\n";
} // end cuntion

// -----------------------------------------------------------------------------

void
GeometryManager::tessellate_errorCallback(GLenum errorCode) {
	const GLubyte *errString;
	errString = gluErrorString(errorCode);
	std::cout << "ERROR: " << errString << "\n";
	std::exit(0);
} // end function

// -----------------------------------------------------------------------------

void
GeometryManager::initTessellator() {
	tobj = gluNewTess();
	gluTessCallback(tobj, GLU_TESS_VERTEX_DATA, (_GLUfuncptr) tessellate_vertexCallback);
	gluTessCallback(tobj, GLU_TESS_BEGIN_DATA, (_GLUfuncptr) tessellate_beginCallback);
	gluTessCallback(tobj, GLU_TESS_END_DATA, (_GLUfuncptr) tessellate_endCallback);
	gluTessCallback(tobj, GLU_TESS_ERROR, (_GLUfuncptr) tessellate_errorCallback);
	gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::tessellate(Object &object) {
	const Polygon &polygon = object.polygon;
	const int numPoints = bg::num_points(polygon);

	GLdouble * vertices = new GLdouble[3 * numPoints];
	int index = 0;
	for (auto it1 = boost::begin(boost::geometry::exterior_ring(object.polygon));
			it1 != boost::end(boost::geometry::exterior_ring(object.polygon)); ++it1) {
		const PolygonPoint &p = *it1;
		vertices[index++] = p.x();
		vertices[index++] = p.y();
		vertices[index++] = 0;
	} // end for

	gluTessBeginPolygon(tobj, &object);
	gluTessBeginContour(tobj);
	for (int i = 0, offset = 0; i < numPoints; i++, offset += 3) {
		gluTessVertex(tobj, vertices + offset, vertices + offset);
	} // end for
	gluTessEndContour(tobj);
	gluTessEndPolygon(tobj);

	delete[] vertices;
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::resortLayers() {
	const int numLayers = layers.size();

	// Sort layers based on their z-index.
	std::vector<std::tuple<int, int>> sortedLayers(numLayers); // (z-index, layer id)
	for (int i = 0; i < numLayers; i++) {
		const Layer &layer = layers[i];
		sortedLayers[i] = std::make_tuple(layer.zIndex, i);
	} // end for
	std::sort(sortedLayers.begin(), sortedLayers.end());

	// Assign z to layers.
	const float zmin = 0;
	const float zmax = 1;
	const float zinc = (zmax - zmin) / numLayers;

	float z = zmin;
	for (int i = 0; i < numLayers; i++) {
		Layer &layer = layers[std::get<1>(sortedLayers[i])];
		layer.z = z;
		z += zinc;
	} // end for
} // end method

// -----------------------------------------------------------------------------

GeometryManager::LayerId
GeometryManager::createLayer(
		const std::string &name,
		const int zIndex,
		const Color lineColor,
		const Color fillColor,
		const LineStippleMask linePattern,
		const FillStippleMask fillPattern
) {

	if (mapLayerNameToLayerId.count(name)) {
		std::cout << "ERROR: Layer '" << name << "' already exists.\n";
		return -1;
	} // end if

	const LayerId layerId = layers.size();
	layers.resize(layers.size() + 1);
	mapLayerNameToLayerId[name] = layerId;

	Layer &layer = layers.back();
	layer.name = name;
	layer.zIndex = zIndex;
	layer.fillPattern = fillPattern;
	layer.fillColor = fillColor;
	layer.lineColor = lineColor;
	layer.linePattern = linePattern;
	layer.visible = true;

	resortLayers();

	return layerId;
} // end method

// -----------------------------------------------------------------------------

GeometryManager::GroupId
GeometryManager::createGroup() {
	const GroupId groupId = groups.size();
	groups.resize(groups.size() + 1);
	return groupId;
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::addObjectToGroup(const ObjectId objectId, const GroupId groupId) {
	if (groupId == INVALID_GROUP_ID)
		return;

	Object &object = getObject(objectId);
	if (object.groupId != INVALID_GROUP_ID) {
		std::cout << "ERROR: Object already belong to a group.\n";
	} else {
		object.groupId = groupId;
		groups[groupId].insert(objectId);
	} // end if
} // end method

// -----------------------------------------------------------------------------

GeometryManager::ObjectId
GeometryManager::addRectangle(
		const LayerId layerId,
		const Box &box,
		void * data,
		const GroupId groupId
) {
	Object object;
	object.type = RECTANGLE;
	object.box = box;
	object.area = (float) bg::area(box);
	object.data = data;

	Layer &layer = layers[layerId];
	layer.objects.push_back(object);

	const ObjectId objectId =  std::make_tuple(layerId, (int) (layer.objects.size() - 1));
	layer.rtree.insert(std::make_pair(box, objectId));
	addObjectToGroup(objectId, groupId);

	return objectId;
} // end method

// -----------------------------------------------------------------------------

GeometryManager::ObjectId
GeometryManager::addPolygon(
		const LayerId layerId,
		const std::vector<DBUxy> &points,
		const float2 displacement,
		void * data,
		const GroupId groupId
) {
	if (points.size() < 2)
		return INVALID_OBJECT_ID;

	Object object;
	object.type = POLYGON;
	object.data = data;
	for (const DBUxy &p : points) {
		bg::append(object.polygon, PolygonPoint(
				(float) p.x + displacement.x,
				(float) p.y + displacement.y));
	} // end for

	if (points.front() != points.back()) {
		bg::append(object.polygon, PolygonPoint(
				(float) points.front().x + displacement.x,
				(float) points.front().y + displacement.y));
	} // end if

	object.area = (float) std::abs(bg::area(object.polygon));
	tessellate(object);

	Layer &layer = layers[layerId];
	layer.objects.push_back(object);

	const ObjectId objectId =  std::make_tuple(layerId, (int) (layer.objects.size() - 1));
    Box box;
    bg::envelope(object.polygon, box);
	layer.rtree.insert(std::make_pair(box, objectId));
	addObjectToGroup(objectId, groupId);

	return objectId;
} // end method

// -----------------------------------------------------------------------------

GeometryManager::ObjectId
GeometryManager::addPath(
		const LayerId layerId,
		const std::vector<DBUxy> &points,
		const float thickness,
		void * data,
		const GroupId groupId
) {

	if (points.size() < 2)
		return INVALID_OBJECT_ID;

	std::vector<float2> outlinePoints;
	tracePathOutline(points, thickness/2, outlinePoints);

	Object object;
	object.type = POLYGON;
	object.data = data;
	for (const float2 &p : outlinePoints) {
		bg::append(object.polygon, PolygonPoint(p.x, p.y));
	} // end for
	object.area = (float) std::abs(bg::area(object.polygon));
	tessellate(object);

	Layer &layer = layers[layerId];
	layer.objects.push_back(object);

	const ObjectId objectId =  std::make_tuple(layerId, (int) (layer.objects.size() - 1));
    Box box;
    bg::envelope(object.polygon, box);
	layer.rtree.insert(std::make_pair(box, objectId));
	addObjectToGroup(objectId, groupId);

	return objectId;
} // end method

// -----------------------------------------------------------------------------

GeometryManager::ObjectId
GeometryManager::searchObjectAt(
		const float x,
		const float y
) const {

	std::vector<std::tuple<float, ObjectId>> results;

	const int numLayers = layers.size();
	for (int i = 0; i < numLayers; i++) {
		const Layer &layer = layers[i];
		if (!layer.visible)
			continue;

		std::vector<RTreeEntry> entries;
		layer.rtree.query(bgi::intersects(Point(x, y)), std::back_inserter(entries));

		for (const RTreeEntry &entry : entries) {
			const Object &object = layer.objects[std::get<1>(entry.second)];
			switch (object.type) {
				case RECTANGLE:
					results.push_back(std::make_tuple(object.area, entry.second));
					break;

				case POLYGON:
				case PATH:
					if (bg::within(Point(x, y), object.polygon) )
						results.push_back(std::make_tuple(object.area, entry.second));

					break;
				default:
					assert(false);
			} // end switch
		} // end for
	} // end for

	std::sort(results.begin(), results.end());
	return results.empty()? INVALID_OBJECT_ID : std::get<1>(results.front());
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::addObjectToHighlight(const ObjectId objectId) {
	if (objectId != INVALID_OBJECT_ID)
		highlightedObjects.insert(objectId);
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::addGroupOfObjectToHighlight(const ObjectId objectId) {
	if (objectId != INVALID_OBJECT_ID)
		addGroupToHighlight(getObject(objectId).groupId);
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::addGroupToHighlight(const GroupId groupId) {
	if (groupId != INVALID_GROUP_ID) {
		for (const ObjectId &objectId : groups[groupId])
			highlightedObjects.insert(objectId);
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::render() const {
	//Stepwatch watch("render");

	const int numLayers = layers.size();
	for (int i = 0; i < numLayers; i++) {
		const Layer &layer = layers[i];
		if (!layer.visible)
			continue;

		const int numObjects = layer.objects.size();

		// Outline
		if (layer.linePattern != LINE_STIPPLE_NONE) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			applyDefaultColor(layer.lineColor);
			for (int k = 0; k < numObjects; k++) {
				const Object &object = layer.objects[k];
				switch (object.type) {
					case RECTANGLE: renderRectangleOutline(layer, object); break;
					case POLYGON: renderPolygonOutline(layer, object); break;
					case PATH: renderPathOutline(layer, object); break;

					default:
						assert(false);
				} // end switch
			} // end for
		} // end if

		// Fill
		if (layer.fillPattern != STIPPLE_MASK_EMPTY) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			if (layer.fillPattern != STIPPLE_MASK_FILL) {
				glEnable(GL_POLYGON_STIPPLE);
				glPolygonStipple(STIPPLE_MASKS[layer.fillPattern]);
			} else {
				glDisable(GL_POLYGON_STIPPLE);
			} // end else

			applyDefaultColor(layer.fillColor);
			for (int k = 0; k < numObjects; k++) {
				const Object &object = layer.objects[k];
				switch (object.type) {
					case RECTANGLE: renderRectangleFill(layer, object); break;
					case POLYGON: renderPolygonFill(layer, object); break;
					case PATH: renderPathFill(layer, object); break;

					default:
						assert(false);
				} // end switch
			} // end for

			if (layer.fillPattern != STIPPLE_MASK_FILL) {
				glDisable(GL_POLYGON_STIPPLE);
			} // end if
		} // end if

	} // end for
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::renderFocusedObject(
		const ObjectId &objectId
) const {
	if (!isObjectIdValid(objectId))
		return;

	const Layer &layer = layers[std::get<0>(objectId)];
	const Object &object = layer.objects[std::get<1>(objectId)];

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glLogicOp(GL_INVERT);
	//glEnable(GL_COLOR_LOGIC_OP);
	glLineWidth(2);
	glColor3ub(0, 0, 0);
	glLineStipple(3, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	glDisable(GL_DEPTH_TEST);

	switch (object.type) {
		case RECTANGLE: renderRectangleOutline(layer, object); break;
		case POLYGON: renderPolygonOutline(layer, object); break;
		case PATH: renderPathOutline(layer, object); break;
		default:
			assert(false);
	} // end switch

	glLineWidth(1);
	glDisable(GL_LINE_STIPPLE);
	//glDisable(GL_COLOR_LOGIC_OP);
	glEnable(GL_DEPTH_TEST);
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::renderHighlightedObjects() const {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2);
	glColor3ub(0, 0, 0);
	glDisable(GL_DEPTH_TEST);

	for (const ObjectId &objectId : highlightedObjects) {
		const Object &object = getObject(objectId);
		const Layer &layer = getLayer(std::get<0>(objectId));

		switch (object.type) {
			case RECTANGLE: renderRectangleOutline(layer, object); break;
			case POLYGON: renderPolygonOutline(layer, object); break;
			case PATH: renderPathOutline(layer, object); break;
			default:
				assert(false);
		} // end switch
	} // end for

	glLineWidth(1);
	glDisable(GL_LINE_STIPPLE);
	glEnable(GL_DEPTH_TEST);
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::renderRectangleOutline(const Layer &layer, const Object &object) const {
	if (object.hasLineColor) {
		applyColor(object.fillColor);
	} // end if

	glBegin(GL_QUADS);
	glVertex3f(object.box.min_corner().get<0>(), object.box.min_corner().get<1>(), layer.z);
	glVertex3f(object.box.max_corner().get<0>(), object.box.min_corner().get<1>(), layer.z);
	glVertex3f(object.box.max_corner().get<0>(), object.box.max_corner().get<1>(), layer.z);
	glVertex3f(object.box.min_corner().get<0>(), object.box.max_corner().get<1>(), layer.z);
	glEnd();

	if (object.hasLineColor) {
		restoreDefaultColor();
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::renderPolygonOutline(const Layer &layer, const Object &object) const {
	if (object.hasLineColor) {
		applyColor(object.fillColor);
	} // end if

	glBegin(GL_LINE_STRIP);
	for (auto it1 = boost::begin(boost::geometry::exterior_ring(object.polygon));
			it1 != boost::end(boost::geometry::exterior_ring(object.polygon)); ++it1) {
		const PolygonPoint &p = *it1;
		glVertex3f(p.x(), p.y(), layer.z);
	} // end for
	glEnd();

	if (object.hasLineColor) {
		restoreDefaultColor();
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::renderPathOutline(const Layer &layer, const Object &object) const {
	renderPolygonOutline(layer, object);
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::renderRectangleFill(const Layer &layer, const Object &object) const {
	if (object.hasFillColor) {
		applyColor(object.fillColor);
	} // end if

	glBegin(GL_QUADS);
	glVertex3f(object.box.min_corner().get<0>(), object.box.min_corner().get<1>(), layer.z);
	glVertex3f(object.box.max_corner().get<0>(), object.box.min_corner().get<1>(), layer.z);
	glVertex3f(object.box.max_corner().get<0>(), object.box.max_corner().get<1>(), layer.z);
	glVertex3f(object.box.min_corner().get<0>(), object.box.max_corner().get<1>(), layer.z);
	glEnd();

	if (object.hasFillColor) {
		restoreDefaultColor();
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::renderPolygonFill(const Layer &layer, const Object &object) const {
	if (object.hasFillColor) {
		applyColor(object.fillColor);
	} // end if

	int index0 = 0;
	const int numComponents = object.tessellationType.size();
	for (int i = 0; i < numComponents; i++) {
		const int index1 = std::get<1>(object.tessellationType[i]);

		glBegin(std::get<0>(object.tessellationType[i]));
		for (int k = index0; k < index1; k++) {
			const float2 &p = object.tessellationPoints[k];
			glVertex3f(p.x, p.y, layer.z);
		} // end for
		index0 = index1;
		glEnd();
	} // end if

	if (object.hasFillColor) {
		restoreDefaultColor();
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::renderPathFill(const Layer &layer, const Object &object) const {
	renderPolygonFill(layer, object);
} // end method

// -----------------------------------------------------------------------------

void
GeometryManager::tracePathOutline(
		const std::vector<DBUxy> &pathPoints,
		const float halfThickness,
		std::vector<float2> &outlinePoints) const
{

	// Historical Note: This function was adapted from wxSightGL developed by
	// Lenna and Guilherme around 2007.

	float2 v1;

	// First point
	v1 = float2( pathPoints[0] ) +
			( float2( pathPoints[1] ) - float2( pathPoints[0] ) ).perpendicular().normalized() * halfThickness;
	outlinePoints.push_back(v1);

	// Forward traversal.
	bool control = true;
	for( size_t i = 1; i < pathPoints.size() - 1; i++){
		control = findMyPoint(
				float2(pathPoints[i-1]),
				float2(pathPoints[i  ]),
				float2(pathPoints[i+1]),
				halfThickness, outlinePoints);
	} // end for

	// Control point.
	if (control) {
		v1 = float2(pathPoints.back()) + float2( float2( pathPoints.back() ) -
				float2( pathPoints[pathPoints.size()-2] ) ).perpendicular().normalized() * halfThickness ;
		outlinePoints.push_back(v1);
	} // end if

	// Last point.
	v1 = float2( pathPoints.back() ) + float2( float2( pathPoints[pathPoints.size()-2] ) -
			float2( pathPoints.back() ) ).perpendicular().normalized() * halfThickness;
	outlinePoints.push_back(v1);

	// Backward traversal.
	for( int i = pathPoints.size() - 2; i >= 1; i--){
		findMyPoint(
				float2(pathPoints[i+1]),
				float2(pathPoints[i  ]),
				float2(pathPoints[i-1]),
				halfThickness, outlinePoints );
	} // end if

	v1 = float2( pathPoints[0] ) + float2( float2( pathPoints[0] ) -
			float2( pathPoints[1] ) ).perpendicular().normalized() * halfThickness ;
	outlinePoints.push_back(v1);
	v1 = float2( pathPoints[0] ) + float2( float2( pathPoints[1] ) -
			float2( pathPoints[0] ) ).perpendicular().normalized() * halfThickness ;
	outlinePoints.push_back(v1);
} // end method

// -----------------------------------------------------------------------------

bool
GeometryManager::findMyPoint(
		float2 v0,
		float2 v1,
		float2 v2,
		const float thickness,
		std::vector<float2> &outlinePoints
) const {

	// Historical Note: This function was adapted from wxSightGL developed by
	// Lenna and Guilherme around 2007.

	// Direction vectors.
	float2 d1 = v1 - v0;
	float2 d2 = v2 - v1;

	// Points over the lines.
	float2 p1 = v1 + (d1.perpendicular().normalized()) * thickness;
	float2 p2 = v1 + (d2.perpendicular().normalized()) * thickness;

	float m1 = (v0.x - v1.x) == 0 ? (v0.y - v1.y) : (v0.y - v1.y) / (v0.x - v1.x);
	float m2 = (v1.x - v2.x) == 0 ? (v1.y - v2.y) : (v1.y - v2.y) / (v1.x - v2.x);

	float tg = (m1 - m2) / (1 + m1 * m2);
	float distancia = std::sqrt(std::pow((p1.x - p2.x), 2.0f) + std::pow((p1.y - p2.y), 2.0f));
	float limite = std::sqrt(2.0f * std::pow(thickness, 2.0f));

	if (distancia > limite && tg > 0.0f) { // dois
		outlinePoints.push_back(p1);
		outlinePoints.push_back(p2);
		return true;
	} else {
		float2 q = findIntersection(p1, p2, d1, d2); // um
		outlinePoints.push_back(q);
		return true;
	}//end else
} // end method

// -----------------------------------------------------------------------------

float2
GeometryManager::findIntersection(
		float2 p1,
		float2 p2,
		float2 d1,
		float2 d2
) const {

	// Historical Note: This function was adapted from wxSightGL developed by
	// Lenna and Guilherme around 2007.

	const float a = p1.x;
	const float e = p1.y;
	const float b = d1.x;
	const float f = d1.y;
	const float c = p2.x;
	const float g = p2.y;
	const float d = d2.x;
	const float h = d2.y;

	float t;
	if ((b * h - d * f) == 0) t = 0;
	else t = ((c - a) * h + (e - g) * d) / (b * h - d * f);
	float2 q = p1 + (d1 * t);

	bool bug;
	if ((t < 0 || t > 1)) {
		bug = true;
	} else {
		bug = false;
	} // end else

	return q;
}//end function