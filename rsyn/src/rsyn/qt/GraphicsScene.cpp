#include <QPainter>

#include "GraphicsScene.h"
#include "GraphicsLayer.h"

extern const bool UseAlternativeSceneMgr;

namespace Rsyn {

GraphicsScene::GraphicsScene(QObject *parent) : QGraphicsScene(parent) {

} // end method

// -----------------------------------------------------------------------------

GraphicsScene::~GraphicsScene() {
	for (GraphicsLayer *layer : clsLayers) {
		delete layer;
	} // end for
} // end method

// -----------------------------------------------------------------------------

void
GraphicsScene::drawBackground(QPainter *painter, const QRectF &rect) {
	if (!UseAlternativeSceneMgr)
		return;

	std::cout << "draw background\n";
	painter->fillRect(rect, Qt::white);
	for (GraphicsLayer *layer : clsLayers) {
		layer->render(painter, rect);
	} // end for
} // end method

// -----------------------------------------------------------------------------

void
GraphicsScene::drawForeground(QPainter *painter, const QRectF &rect) {
	if (!UseAlternativeSceneMgr)
		return;

	std::cout << "draw foreground\n";
	QPen pen;
	pen.setWidth(2);
	if (clsHover) {
		pen.setColor(Qt::black);
	} else {
		pen.setColor(Qt::lightGray);
	}
	pen.setStyle(Qt::DashLine);
	pen.setCosmetic(true);

	static int offset = 0;
	pen.setDashOffset(offset++);

	QBrush brush;

	painter->setPen(pen);
	painter->setBrush(brush);

	QPainterPath path;
	path.addRect(sceneRect());

	painter->drawPath(path);

} // end method

// -----------------------------------------------------------------------------

void
GraphicsScene::addLayer(GraphicsLayer *layer) {
	layer->clsScene = this;
	clsLayers.push_back(layer);
	addItem(layer);
} // end method

} // end namespace