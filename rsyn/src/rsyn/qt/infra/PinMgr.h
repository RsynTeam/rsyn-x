#ifndef RSYN_QT_PIN_MGR_H
#define RSYN_QT_PIN_MGR_H

#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"

#include <QPainterPath>

namespace Rsyn {

class QtPinMgr {
public:

	static void create();
	static QtPinMgr *get() {return instance;}

	const QPainterPath &
	getShape(Rsyn::Pin pin) const {
		Rsyn::LibraryPin lpin = pin.getLibraryPin();
		if (lpin) {
			Rsyn::PhysicalCell physicalCell = physicalDesign.getPhysicalCell(pin);
			return getShape(lpin, physicalCell.getOrientation());
		} else {
			return EMPTY_SHAPE;
		} // end else
	} // end method

	const QPainterPath &
	getShape(Rsyn::LibraryPin lpin, const Rsyn::PhysicalOrientation &orientation) const {
		return pins[lpin].shapes[orientation];
	} // end method

private:

	struct QtLibraryPin {
		QPainterPath shapes[Rsyn::NUM_PHY_ORIENTATION];
	};

	QtPinMgr();
	QtPinMgr(const QtPinMgr &mgr);

	static QPainterPath
	createShape(Rsyn::LibraryPin lpin, const Rsyn::PhysicalOrientation &orientation);

	Rsyn::PhysicalDesign physicalDesign;
	Rsyn::Attribute<Rsyn::LibraryPin, QtLibraryPin> pins;

	static QtPinMgr *instance;
	const QPainterPath EMPTY_SHAPE;
}; // end class

} // end namespace

#endif