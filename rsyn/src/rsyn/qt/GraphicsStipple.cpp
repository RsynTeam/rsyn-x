#include <QPixmap>
#include <QBitmap>
#include <QIcon>
#include <QPainter>

#include "GraphicsStipple.h"

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

GraphicsStippleMgr *GraphicsStippleMgr::instance = nullptr;

// -----------------------------------------------------------------------------

void GraphicsStippleMgr::create() {
	if (!instance)
		instance = new GraphicsStippleMgr();
} // end method

// -----------------------------------------------------------------------------

GraphicsStippleMgr::GraphicsStippleMgr() {
	stippleBrushes.clear();
	stippleBrushes.resize(STIPPLE_MASK_NUM_MASKS);

	for (int i = 0; i < STIPPLE_MASK_NUM_MASKS; i++) {
		QBitmap pixmap;
		pixmap = pixmap.fromData(QSize(32, 32), STIPPLE_MASKS[i], QImage::Format_Mono);
		stippleBrushes[i].setTexture(pixmap);
	} // end for
} // end constructor

// -----------------------------------------------------------------------------

QIcon GraphicsStippleMgr::createIcon(const FillStippleMask &mask, const int w, const int h, const QColor &color) const {
	QPixmap pixmap(w, h);
	QPainter painter(&pixmap);

	painter.fillRect(0, 0, w, h, Qt::white);

	QBrush brush = getBrush(mask);
	brush.setColor(color);

	QPen pen;
	pen.setColor(color);

	painter.setBrush(brush);
	painter.setPen(pen);

	painter.drawRect(0, 0, w-1, h-1);

	return QIcon(pixmap);
} // end method

// -----------------------------------------------------------------------------

} // end namespace