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

// @todo Reimplement using the new graphics infrastructure.

//#include "ExampleOverlay.h"
//
//#include "session/Session.h"
//#include "phy/PhysicalDesign.h"
//
//#include "gui/graphics/GraphicsView.h"
//
//#include <limits>
//#include <QtWidgets>
//#include <QColor>
//
////
///-----------------------------------------------------------------------------
//
// namespace RsynExample {
//
////
///-----------------------------------------------------------------------------
//
// ExampleOverlay::ExampleOverlay() :
//		Rsyn::GraphicsOverlay("Example Overlay", nullptr) {
//	setZValue(100);
//	setVisible(false);
//} // end method
//
////
///-----------------------------------------------------------------------------
//
// ExampleOverlay::~ExampleOverlay() {
//} // end destructor
//
////
///-----------------------------------------------------------------------------
//
// bool
// ExampleOverlay::init(Rsyn::GraphicsView *view,
// std::vector<Rsyn::GraphicsLayerDescriptor> &visibilityItems) {
//	Rsyn::Session session;
//	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();
//
//	if (!physicalDesign)
//		return false;
//
//	const Bounds &coreBounds = physicalDesign.getPhysicalDie().getBounds();
//	bounds = QRectF(coreBounds.getX(), coreBounds.getY(),
//			coreBounds.getWidth(), coreBounds.getHeight());
//
//	visibilityItems.push_back(Rsyn::GraphicsLayerDescriptor("Overlays/Example",
// false));
//	view->registerObserver(this);
//	return true;
//} // end method
//
////
///-----------------------------------------------------------------------------
//
// void
// ExampleOverlay::onChangeVisibility(const std::string &key, const bool
// visible) {
//	if (key == "Overlays/Example") {
//		setVisible(visible);
//	} // end if
//} // end method
//
////
///-----------------------------------------------------------------------------
//
// void
// ExampleOverlay::onChangePhysicalLayerVisibility(const Rsyn::PhysicalLayer
// &layer, const bool visible) {
//} // end method
//
////
///-----------------------------------------------------------------------------
//
// void
// ExampleOverlay::paint(QPainter *painter, const QStyleOptionGraphicsItem
// *item, QWidget *widget) {
//	QPen pen;
//	pen.setWidth(0);
//	pen.setColor(Qt::gray);
//
//	painter->setPen(pen);
//	painter->drawLine(bounds.left(), bounds.bottom(), bounds.right(),
// bounds.top());
//	painter->drawLine(bounds.left(), bounds.top(), bounds.right(),
// bounds.bottom());
//
//	const int fontSize = 16;
//	QFont font("Times", fontSize);
//	painter->setFont(font);
//
//	const double scaleValue = scale()/painter->transform().m11();
//	painter->save();
//	painter->translate(bounds.x(), bounds.y() + bounds.height());
//	painter->scale(scaleValue, -scaleValue);
//	painter->setPen(QColor(0, 0, 0));
//	painter->drawText(fontSize * 0.5, fontSize * 1.5,
// QString(QString::fromStdString(
//			"This is an example of overlay...")));
//	painter->restore();
//} // end method
//
////
///-----------------------------------------------------------------------------
//
//} // end namespace
