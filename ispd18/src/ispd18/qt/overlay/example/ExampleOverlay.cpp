#include "ExampleOverlay.h"

#include <Rsyn/Session>
#include <Rsyn/PhysicalDesign>

#include "rsyn/qt/graphics/view/layout/LayoutGraphicsScene.h"

#include <QPen>
#include <QBrush>
#include <QColor>
#include <QPainter>

// -----------------------------------------------------------------------------

namespace RsynExample {

// -----------------------------------------------------------------------------

ExampleOverlay::ExampleOverlay() {
} // end method

// -----------------------------------------------------------------------------

ExampleOverlay::~ExampleOverlay() {
} // end destructor

// -----------------------------------------------------------------------------

bool
ExampleOverlay::init(Rsyn::LayoutGraphicsScene *scene, std::vector<Rsyn::GraphicsLayerDescriptor> &visibilityItems) {
	Rsyn::Session session;
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();

	if (!physicalDesign)
		return false;

	const Bounds &coreBounds = physicalDesign.getPhysicalDie().getBounds();
	clsCoreBounds = QRectF(coreBounds.getX(), coreBounds.getY(),
			coreBounds.getWidth(), coreBounds.getHeight());

	visibilityItems.push_back(Rsyn::GraphicsLayerDescriptor("Overlays", true)); // @todo should not be necessary, but it is for now
	visibilityItems.push_back(Rsyn::GraphicsLayerDescriptor("Overlays/Example", false));
	return true;
} // end method

// -----------------------------------------------------------------------------

void
ExampleOverlay::render(QPainter *painter, const float lod, const QRectF &exposedRect) {
	if (!getScene()->getVisibility("Overlays/Example"))
		return;

	QPen pen;
	pen.setWidth(0);
	pen.setColor(Qt::gray);

	painter->setPen(pen);
	painter->drawLine(clsCoreBounds.left(), clsCoreBounds.bottom(), clsCoreBounds.right(), clsCoreBounds.top());
	painter->drawLine(clsCoreBounds.left(), clsCoreBounds.top(), clsCoreBounds.right(), clsCoreBounds.bottom());

	{ // Render a text in viewport coordinates.
		// Save painter state.
		painter->save();

		// Map core bounds (in scene coordinates) to viewport coordinates.
		const QRectF mappedCoreBounds = painter->worldTransform().mapRect(clsCoreBounds);

		// Set the world transform to the identity matrix so that everything is
		// rendered in viewport (i.e. pixels) coordinates.
		painter->setWorldTransform(QTransform());

		// Compute the width of the text (in pixels).
		const int fontSize = 14;

		const QFont font("Times", fontSize);
		painter->setFont(font);

		const int offsetX = 5; // in pixels
		const int offsetY = fontSize + 5; // in pixels

		painter->setPen(QColor(0, 0, 0));
		painter->drawText(
				mappedCoreBounds.x() + offsetX,
				mappedCoreBounds.y() + offsetY,
				"This is an overlay...");

		// Restore painter state.
		painter->restore();
	} // end block
} // end method

// -----------------------------------------------------------------------------

} // end namespace
