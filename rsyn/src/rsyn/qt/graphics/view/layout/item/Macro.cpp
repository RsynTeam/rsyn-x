/* Copyright 2014-2018 Rsyn
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
 
#include "Macro.h"

#include <QPainter>

#include <Rsyn/Session>
#include <Rsyn/PhysicalDesign>
#include "rsyn/qt/QtUtils.h"

namespace Rsyn {

MacroGraphicsItem::MacroGraphicsItem(Rsyn::Cell cell) : CellGraphicsItem(cell) {
	Rsyn::Session session;
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();

	Rsyn::PhysicalLibraryCell physicalLibraryCell =
			physicalDesign.getPhysicalLibraryCell(cell);

	if (physicalLibraryCell.hasLayerObstacles()) {
		for (const Bounds &obs : physicalLibraryCell.allLayerObstacles()) {
			QRect rect = QtUtils::convert(obs);
			QPainterPath shape;
			shape.addRect(rect);
			clsShape += shape;
		} // end for
	} else {
		Bounds bounds(DBUxy(), cell.getSize());
		QRect rect = QtUtils::convert(bounds);
		QPainterPath shape;
		shape.addRect(rect);
		clsShape += shape;
	} // end else
	clsShape = clsShape.simplified();
	clsShape.translate(cell.getX(), cell.getY());
} // end constructor

// -----------------------------------------------------------------------------

void
MacroGraphicsItem::render(GraphicsScene *scene, QPainter *painter, const float lod, const QRectF &exposedRect) {
	painter->drawPath(clsShape);

	// Draw pins.
	if (lod > 50) {
		renderPins(painter);
	} // end if

	// Draw name.
	if (lod > 100) {
		renderName(painter, exposedRect);
	} // end method
} // end method

// -----------------------------------------------------------------------------

} // end namespace