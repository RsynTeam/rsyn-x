#include "PinMgr.h"
#include "rsyn/qt/QtUtils.h"

namespace Rsyn {

// -----------------------------------------------------------------------------

QtPinMgr *QtPinMgr::instance = nullptr;
//const QPainterPath QtPinMgr::EMPTY_SHAPE;

// -----------------------------------------------------------------------------

void
QtPinMgr::create() {
	if (!instance)
		instance = new QtPinMgr();
} // end method

// -----------------------------------------------------------------------------

QtPinMgr::QtPinMgr() {
	Rsyn::Session session;
	Rsyn::Design design = session.getDesign();

	pins = design.createAttribute();
	physicalDesign = session.getPhysicalDesign();

	for (Rsyn::LibraryCell lcell : design.allLibraryCells()) {
		for (Rsyn::LibraryPin lpin : lcell.allLibraryPins()) {
			QtLibraryPin &qlpin = pins[lpin];
			qlpin.shapes[ORIENTATION_N ] = createShape(lpin, ORIENTATION_N);
			qlpin.shapes[ORIENTATION_S ] = createShape(lpin, ORIENTATION_S);
			qlpin.shapes[ORIENTATION_W ] = createShape(lpin, ORIENTATION_W);
			qlpin.shapes[ORIENTATION_E ] = createShape(lpin, ORIENTATION_E);
			qlpin.shapes[ORIENTATION_FN] = createShape(lpin, ORIENTATION_FN);
			qlpin.shapes[ORIENTATION_FS] = createShape(lpin, ORIENTATION_FS);
			qlpin.shapes[ORIENTATION_FW] = createShape(lpin, ORIENTATION_FW);
			qlpin.shapes[ORIENTATION_FE] = createShape(lpin, ORIENTATION_FE);
		} // end for
	} // end for
} // end constructor

// -----------------------------------------------------------------------------

QPainterPath
QtPinMgr::createShape(Rsyn::LibraryPin lpin, const Rsyn::PhysicalOrientation &orientation) {
	QPainterPath shape;

	Rsyn::Session session;
	Rsyn::PhysicalDesign phDesign = session.getPhysicalDesign();
	Rsyn::PhysicalLibraryPin phLibPin = phDesign.getPhysicalLibraryPin(lpin);
	if (phLibPin) {
		Rsyn::PhysicalLibraryCell phLibCell = phLibPin.getPhysicalLibraryCell();
		const Rsyn::PhysicalTransform &transform = phLibCell.getTransform(orientation);

		if (phLibPin.hasPinGeometries()) {
			for (Rsyn::PhysicalPinGeometry phPinPort : phLibPin.allPinGeometries()) {
				
				for (Rsyn::PhysicalPinLayer phPinLayer : phPinPort.allPinLayers()) {

					if (phPinLayer.hasPolygonBounds()) {
						for (const Rsyn::PhysicalPolygon &polygon : phPinLayer.allPolygons()) {
							QPolygon qpoly;
							for (auto it1 = boost::begin(boost::geometry::exterior_ring(polygon));
								it1 != boost::end(boost::geometry::exterior_ring(polygon)); ++it1) {
								const Rsyn::PhysicalPolygonPoint &p = *it1;
								qpoly.append(QtUtils::convert(transform.apply(
									p.get<0>(), p.get<1>())));
							} // end for
							QPainterPath path;
							path.addPolygon(qpoly);
							shape += path;
						} // end for
					} // end if
					if (phPinLayer.hasRectangleBounds()) {
						for (const Bounds &bounds : phPinLayer.allBounds()) {
							QPainterPath path;
							path.addRect(QtUtils::convert(transform.apply(bounds)));
							shape += path;
						} // end for
					} // end if
				} // end for 
			} // end for
		} // end if
	} // end if

	return shape.simplified();
} // end method

} // end method