#ifndef RSYN_QT_GRAPHICS_STIPPLE_H
#define RSYN_QT_GRAPHICS_STIPPLE_H

#include "rsyn/util/Stipple.h"

#include <QColor>
#include <QIcon>
#include <QBrush>
#include <QPen>

namespace Rsyn {
class GraphicsStippleMgr {
public:

	static GraphicsStippleMgr * get() {return instance;}
	static void create();

	const QBrush &getBrush(const FillStippleMask &mask) const {
		return stippleBrushes[mask];
	} // end method

	QIcon createIcon(const FillStippleMask &mask, const int w, const int h, const QColor &color) const;

private:
	
	GraphicsStippleMgr();

	std::vector<QBrush> stippleBrushes;

	static GraphicsStippleMgr *instance;

}; // end class
} // end namespace

#endif /* GRAPHICSSTIPPLE_H */

