#include "ExampleOverlay.h"

#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"

#include "rsyn/qt/GraphicsView.h"

#include <limits>
#include <QtWidgets>
#include <QColor>

// -----------------------------------------------------------------------------

namespace RsynExample {

// -----------------------------------------------------------------------------

ExampleOverlay::ExampleOverlay() :
		Rsyn::GraphicsOverlay("Example Overlay", nullptr) {
	setZValue(100);
	setVisible(false);
} // end method

// -----------------------------------------------------------------------------

ExampleOverlay::~ExampleOverlay() {
} // end destructor

// -----------------------------------------------------------------------------

bool
ExampleOverlay::init(Rsyn::GraphicsView *view, std::vector<Rsyn::GraphicsLayerDescriptor> &visibilityItems) {
	Rsyn::Session session;
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();

	if (!physicalDesign)
		return false;

	const Bounds &coreBounds = physicalDesign.getPhysicalDie().getBounds();
	bounds = QRectF(coreBounds.getX(), coreBounds.getY(),
			coreBounds.getWidth(), coreBounds.getHeight());

	visibilityItems.push_back(Rsyn::GraphicsLayerDescriptor("Overlays/Example", false));
	view->registerObserver(this);
	return true;
} // end method

// -----------------------------------------------------------------------------

void
ExampleOverlay::onChangeVisibility(const std::string &key, const bool visible) {
	if (key == "Overlays/Example") {
		setVisible(visible);
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
ExampleOverlay::onChangePhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) {
} // end method

// -----------------------------------------------------------------------------

void
ExampleOverlay::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) {
	QPen pen;
	pen.setWidth(0);
	pen.setColor(Qt::gray);

	painter->setPen(pen);
	painter->drawLine(bounds.left(), bounds.bottom(), bounds.right(), bounds.top());
	painter->drawLine(bounds.left(), bounds.top(), bounds.right(), bounds.bottom());

	const int fontSize = 16;
	QFont font("Times", fontSize);
	painter->setFont(font);

	const double scaleValue = scale()/painter->transform().m11();
	painter->save();
	painter->translate(bounds.x(), bounds.y() + bounds.height());
	painter->scale(scaleValue, -scaleValue);
	painter->setPen(QColor(0, 0, 0));
	painter->drawText(fontSize * 0.5, fontSize * 1.5, QString(QString::fromStdString(
			"This is an example of overlay...")));
	painter->restore();
} // end method

// -----------------------------------------------------------------------------

} // end namespace
