#include "GraphicsScene.h"
#include "GraphicsView.h"

#include "rsyn/qt/overlay/highlight/HighlightOverlay.h"
#include "rsyn/qt/overlay/instance/CellItem.h"
#include "rsyn/qt/overlay/routing/RoutingItem.h"

#include "rsyn/qt/QtUtils.h"
#include "rsyn/qt/infra/PinMgr.h"

#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/util/Stepwatch.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QEvent>
#include <QToolTip>
#include <QStatusBar>

#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif

#include <iostream>

// -----------------------------------------------------------------------------

namespace Rsyn {

static const bool DebugRendering = false;

// -----------------------------------------------------------------------------

GraphicsView::GraphicsView(QWidget *parent) :
		QGraphicsView(parent),
		zoomScaling(1),
		sceneToViewportScalingFactor(0),
		rowHeight(0),
		initialized(false)
{

    setRenderHint(QPainter::Antialiasing, false);
    setDragMode(QGraphicsView::RubberBandDrag);
    setOptimizationFlags(
			QGraphicsView::DontSavePainterState |
			QGraphicsView::DontAdjustForAntialiasing);
	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setCacheMode(QGraphicsView::CacheBackground);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	if (DebugRendering) {
		setOptimizationFlag(QGraphicsView::IndirectPainting, true);
	} // end if

    updateViewMatrix();
} // end method

// -----------------------------------------------------------------------------

GraphicsView::~GraphicsView() {
	// Nil the observers's reference to this graphics view to avoid problems
	// when the observer is destroyed and tries to unregister itself.
	for (GraphicsObserver *observer : clsObservers) {
		observer->graphicsView = nullptr;
	} // end for
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::init() {
	updateAdjustedSceneRect();
	resetView();

	Rsyn::Session session;
	rsynGraphics = session.getService("rsyn.graphics");

	physicalDesign = session.getPhysicalDesign();
	rowHeight = physicalDesign.getRowHeight();

	initialized = true;
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::updateAdjustedSceneRect() {
	const QRectF &sceneRect = scene()->sceneRect();
	adjustedSceneRect = sceneRect;

	const qreal gap = std::min(sceneRect.width(), sceneRect.height()) * 0.01;
	adjustedSceneRect.adjust(-gap, -gap, +gap, +gap);

	// Adjust scene view to match viewport dimensions.
	const qreal w = width();
	const qreal W = adjustedSceneRect.width();
	const qreal h = height();
	const qreal H = adjustedSceneRect.height();

	const qreal scalingW = w / W;
	const qreal scalingH = h / H;
	sceneToViewportScalingFactor = std::min(scalingW, scalingH);

	if (scalingW < scalingH) {
		const qreal adjustedHeight = h / scalingW;
		const qreal d = (adjustedHeight - H) / 2;
		const qreal y0 = adjustedSceneRect.bottom();
		const qreal y1 = adjustedSceneRect.top();
		adjustedSceneRect.setBottom(y0 - d);
		adjustedSceneRect.setTop(y1 + d);
	} else {
		const qreal adjustedWidth = w / scalingH;
		const qreal d = (adjustedWidth - W) / 2;
		const qreal x0 = adjustedSceneRect.left();
		const qreal x1 = adjustedSceneRect.right();
		adjustedSceneRect.setLeft(x0 - d);
		adjustedSceneRect.setRight(x1 + d);
	} // end else

	//std::cout << "w: " << (sceneToViewportScalingFactor * adjustedSceneRect.width()) << " = " << width() << "\n";
	//std::cout << "h: " << (sceneToViewportScalingFactor * adjustedSceneRect.height()) << " = " << height() << "\n";
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::updateViewMatrix() {
    const qreal scaling = sceneToViewportScalingFactor*getZoom();

	QMatrix mtx;
	mtx.translate(0, rect().bottom());
	mtx.scale(scaling, -scaling);
	setMatrix(mtx);
	invertedTransform = transform().inverted();
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::updateRenderingCache() {
	cachedRendering = grab();
	//cachedRendering.save("test.png");
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::resetView() {
	zoomScaling = 1;
    updateViewMatrix();

    ensureVisible(QRectF(0, 0, 0, 0));
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::togglePointerMode(const bool enable) {
    setDragMode(enable
			? QGraphicsView::RubberBandDrag
			: QGraphicsView::ScrollHandDrag);
    setInteractive(enable);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::toggleOpenGL(const bool enable) {
#ifndef QT_NO_OPENGL
    setViewport(enable ?
		new QGLWidget(QGLFormat(QGL::SampleBuffers)) : new QWidget);
#endif
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::toggleAntialiasing(const bool enable) {
    setRenderHint(QPainter::Antialiasing, enable);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::print() {
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        render(&painter);
    }
#endif
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::zoomIn(int level) {
    zoomScaling *= std::pow(2, level);
	updateViewMatrix();
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::zoomOut(int level) {
    zoomScaling /= std::pow(2, level);
	updateViewMatrix();
} // end method

// -----------------------------------------------------------------------------

qreal
GraphicsView::getZoom() const {
	return zoomScaling;
} // end method

// -----------------------------------------------------------------------------

qreal
GraphicsView::getSceneWidth() const {
	return adjustedSceneRect.width() / getZoom();
} // end method

// -----------------------------------------------------------------------------

qreal
GraphicsView::getSceneHeight() const {
	return adjustedSceneRect.height() / getZoom();
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::resizeEvent(QResizeEvent * event) {
	if (!isInitialized())
		return;

	setUpdatesEnabled(false);
	updateAdjustedSceneRect();
	updateViewMatrix();
	setUpdatesEnabled(true);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::drawBackground(QPainter *painter, const QRectF &rect) {
	QGraphicsView::drawBackground(painter, rect);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::drawForeground(QPainter *painter, const QRectF &rect) {
	QGraphicsView::drawForeground(painter, rect);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::drawItems(QPainter *painter, int numItems,
					   QGraphicsItem *items[],
					   const QStyleOptionGraphicsItem options[]) {
	Stepwatch watch("Debug rendering #items=" + std::to_string(numItems));
	QGraphicsView::drawItems(painter, numItems, items, options);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::paintEvent(QPaintEvent *event) {
	{
		//Stepwatch watch("Rendering");
		QGraphicsView::paintEvent(event);
	}

	{
		//Stepwatch watch("Caching");
		//updateRenderingCache();
	}
} // end method

// -----------------------------------------------------------------------------

#ifndef QT_NO_WHEELEVENT
void
GraphicsView::wheelEvent(QWheelEvent *e) {
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->delta() > 0)
            zoomIn(1);
        else
            zoomOut(1);
		e->accept();
	} else {
		QGraphicsView::wheelEvent(e);
    } // end else
} // end method
#endif

// -----------------------------------------------------------------------------

void
GraphicsView::mousePressEvent(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::mouseMoveEvent(QMouseEvent *event) {
	// **temp**
	if (scene()) {
		((GraphicsScene *)scene())->clsHover = sceneRect().contains(mapToScene(event->pos()));
		scene()->invalidate(QRect(), QGraphicsScene::ForegroundLayer);
	} // end if

	if (clsHighlightOverlay) {
		QGraphicsItem *item = itemAt(event->pos());

		QString tooltip;
		QPainterPath hoverShape;
		bool needsUpdate = false;

		if (item) {
			if (true /*&& item != clsPreviousHoverItem*/) {
				const QPointF &mappedPos = mapToScene(event->pos());

				if (CellGraphicsItem *cellItem = dynamic_cast<CellGraphicsItem *>(item)) {
					Rsyn::PhysicalCell physicalCell = cellItem->getPhysicalCell();
					Rsyn::Cell cell = physicalCell.getCell();

					Rsyn::Pin hoverPin = nullptr;
					for (Rsyn::Pin pin : cell.allPins()) {
						const QPainterPath &pinShape = QtPinMgr::get()->getShape(pin.getLibraryPin(),
								physicalCell.getOrientation());
						if (pinShape.contains(mappedPos - item->pos())) {
							hoverShape = pinShape.translated(cellItem->pos());
							hoverPin = pin;
							break;
						} // end if
					} // end for

					if (!hoverPin || item != clsPreviousHoverItem) {
						needsUpdate = true;
						hoverShape = hoverShape = item->shape().translated(item->pos());
						tooltip = QString::fromStdString(cell.getName());
					} else if (hoverPin != clsHighlightOverlay->getHoverPin()) {
						needsUpdate = true;
						tooltip = QString::fromStdString(hoverPin.getFullName());
					} else 
					clsHighlightOverlay->setHoverPin(hoverPin);

				} else if (RoutingGraphicsItem *routingItem = dynamic_cast<RoutingGraphicsItem *>(item)) {
					needsUpdate = true;

					if (event->modifiers() & Qt::ShiftModifier) {
						hoverShape = item->shape().translated(item->pos());
					} else {
						Rsyn::Net net = routingItem->getNet();
						if (net) {
							hoverShape = traceNet(net, true);
							tooltip = QString::fromStdString(net.getName());
						} // end if
					} // end if
				} // end else
			} else {
				// no item
			} // end else
		} else {
			// No item...
		} // end else

		if (clsPreviousHoverItem != item || needsUpdate) {
			clsHighlightOverlay->clearHover();
			clsHighlightOverlay->setHoverOutline(hoverShape);
			if (statusBar) {
				statusBar->showMessage(tooltip);
			} // end if
		} // end if

		clsPreviousHoverItem = item;
	} // end if
	
    QGraphicsView::mouseMoveEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::mouseDoubleClickEvent(QMouseEvent * event) {
	if (event->button() == Qt::LeftButton) {
	} // end if
	QGraphicsView::mouseDoubleClickEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::enterEvent(QEvent *event) {
	QGraphicsView::enterEvent(event);
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::leaveEvent(QEvent *event) {
	if (clsHighlightOverlay) {
		clsHighlightOverlay->clearHover();
		clsHighlightOverlay->update();
	} // end if
	QGraphicsView::leaveEvent(event);
} // end method

// -----------------------------------------------------------------------------

float
GraphicsView::getNumExposedRows() const {
	return std::max(getSceneWidth(), getSceneHeight()) / rowHeight;
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::setVisibility(const std::string &key, const bool visible) {
	auto it = clsVisibility.find(key);
	if (visible != it->second) {
		clsVisibility[key] = visible;
		for (GraphicsObserver *observer : clsObservers) {
			observer->onChangeVisibility(key, visible);
		} // end for
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::setPhysicalLayerVisibility(const Rsyn::PhysicalLayer &layer, const bool visible) {
	auto it = clsPhysicalLayerVisibility.find(layer);
	if (visible != it->second) {
		clsPhysicalLayerVisibility[layer] = visible;
		for (GraphicsObserver *observer : clsObservers) {
			observer->onChangePhysicalLayerVisibility(layer, visible);
		} // end for
	} // end if
} // end method

// -----------------------------------------------------------------------------

void
GraphicsView::setPhysicalLayerVisibility(const std::string &layerName, const bool visible) {
	Rsyn::Session session;
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();
	Rsyn::PhysicalLayer layer = physicalDesign.getPhysicalLayerByName(layerName);
	if (layer) {
		setPhysicalLayerVisibility(layer, visible);
	} // end if
} // end method

// -----------------------------------------------------------------------------

QBrush
GraphicsView::getStippleBrush(const FillStippleMask &mask, const qreal dx, const qreal dy) const {
	GraphicsStippleMgr *mgr = GraphicsStippleMgr::get();
	QBrush brush = mgr->getBrush(mask);
	QTransform transform = invertedTransform;
	transform.translate(-dx, -dy);
	brush.setTransform(transform);
	return brush;
} // end method

// -----------------------------------------------------------------------------

QPainterPath
GraphicsView::traceNet(Rsyn::Net net, const bool includePins) {
	QPainterPath netShape;

	Rsyn::PhysicalNet phNet = physicalDesign.getPhysicalNet(net);
	for (Rsyn::PhysicalWire phWire : phNet.allWires()) {
		for (Rsyn::PhysicalWireSegment phWireSegment : phWire.allWireSegments()) {
			if (phWireSegment.getNumRoutingPoints() < 2)
				continue;

			const std::vector<Rsyn::PhysicalRoutingPoint> & routingPts = phWireSegment.allRoutingPoints();
			const int numRoutingPoint = routingPts.size();
			if (numRoutingPoint >= 2) {
				std::vector<DBUxy> points;
				points.reserve(numRoutingPoint);

				points.push_back(phWireSegment.getSourcePosition());
				for (int i = 1; i < numRoutingPoint - 1; i++) {
					points.push_back(routingPts[i].getPosition());
				} // end for
				points.push_back(phWireSegment.getTargetPosition());

				Rsyn::PhysicalLayer phLayer = phWireSegment.getLayer();
				const DBU width = phLayer.getWidth();

				std::vector<QPointF> outline;
				QtUtils::tracePathOutline(points, width/2.0f, outline);

				// @todo maybe tracePathOutline should return a polygon directly.
				QPolygonF poly;
				for (const QPointF &p : outline) {
					poly.append(p);
				} // end for

				QPainterPath path;
				path.addPolygon(poly);
				netShape += path;
			} // end if
		} // end for
	} // end for

	if (includePins) {
		for (Rsyn::Pin pin : net.allPins()) {
			Rsyn::Instance instance = pin.getInstance();
			if (instance.getType() == Rsyn::CELL) {
				Rsyn::PhysicalCell physicalCell = physicalDesign.getPhysicalCell(pin);
				Rsyn::LibraryPin lpin = pin.getLibraryPin();
				if (lpin) {
					netShape += QtPinMgr::get()->getShape(lpin,
							physicalCell.getOrientation())
							.translated(physicalCell.getX(), physicalCell.getY());
				} // end for
			} // end if
		} // end for
	} // end if

	return netShape.simplified();
} // end method

} // end namespace