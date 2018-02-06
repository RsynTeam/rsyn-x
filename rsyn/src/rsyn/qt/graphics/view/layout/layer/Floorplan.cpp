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
 
#include "Floorplan.h"

#include <Rsyn/Session>
#include <Rsyn/PhysicalDesign>

#include "rsyn/qt/graphics/infra/GraphicsView.h"

#include <limits>
#include <QtWidgets>
#include <QColor>

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

FloorplanLayoutGraphicsLayer::FloorplanLayoutGraphicsLayer() {
} // end method

// -----------------------------------------------------------------------------

FloorplanLayoutGraphicsLayer::~FloorplanLayoutGraphicsLayer() {
} // end destructor

// -----------------------------------------------------------------------------

bool FloorplanLayoutGraphicsLayer::init(LayoutGraphicsScene *scene, std::vector<GraphicsLayerDescriptor> &visibilityItems) {
	Rsyn::Session session;
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();

	if (!physicalDesign)
		return false;

	// Store tracks.
	const Bounds &coreBounds = physicalDesign.getPhysicalDie().getBounds();

	clsCoreBounds = QRect(coreBounds.getX(), coreBounds.getY(),
			coreBounds.getWidth(), coreBounds.getHeight());

	return true;
} // end method

// -----------------------------------------------------------------------------

void
FloorplanLayoutGraphicsLayer::render(QPainter *painter, const float lod, const QRectF &exposedRect) {
	QPen pen;
	pen.setColor(Qt::darkGray);
	pen.setCosmetic(true);
	painter->setPen(pen);

	QBrush brush;
	painter->setBrush(brush);

	painter->drawRect(clsCoreBounds);

	LayoutGraphicsLayer::render(painter, lod, exposedRect);
} // end method

// -----------------------------------------------------------------------------

} // end namespace