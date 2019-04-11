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

#include "PinMgr.h"
#include "gui/QtUtils.h"

namespace Rsyn {

// -----------------------------------------------------------------------------

QtPinMgr *QtPinMgr::instance = nullptr;
// const QPainterPath QtPinMgr::EMPTY_SHAPE;

// -----------------------------------------------------------------------------

void QtPinMgr::create() {
        if (!instance) instance = new QtPinMgr();
}  // end method

// -----------------------------------------------------------------------------

QtPinMgr::QtPinMgr() {
        Rsyn::Session session;
        Rsyn::Design design = session.getDesign();
        Rsyn::Library library = session.getLibrary();

        pins = design.createAttribute();
        physicalDesign = session.getPhysicalDesign();

        for (Rsyn::LibraryCell lcell : library.allLibraryCells()) {
                for (Rsyn::LibraryPin lpin : lcell.allLibraryPins()) {
                        QtLibraryPin &qlpin = pins[lpin];
                        qlpin.shapes[ORIENTATION_N] =
                            createShape(lpin, ORIENTATION_N);
                        qlpin.shapes[ORIENTATION_S] =
                            createShape(lpin, ORIENTATION_S);
                        qlpin.shapes[ORIENTATION_W] =
                            createShape(lpin, ORIENTATION_W);
                        qlpin.shapes[ORIENTATION_E] =
                            createShape(lpin, ORIENTATION_E);
                        qlpin.shapes[ORIENTATION_FN] =
                            createShape(lpin, ORIENTATION_FN);
                        qlpin.shapes[ORIENTATION_FS] =
                            createShape(lpin, ORIENTATION_FS);
                        qlpin.shapes[ORIENTATION_FW] =
                            createShape(lpin, ORIENTATION_FW);
                        qlpin.shapes[ORIENTATION_FE] =
                            createShape(lpin, ORIENTATION_FE);
                }  // end for
        }          // end for
}  // end constructor

// -----------------------------------------------------------------------------

QPainterPath QtPinMgr::createShape(
    Rsyn::LibraryPin lpin, const Rsyn::PhysicalOrientation &orientation) {
        QPainterPath shape;

        Rsyn::Session session;
        Rsyn::PhysicalDesign phDesign = session.getPhysicalDesign();
        Rsyn::PhysicalLibraryPin phLibPin =
            phDesign.getPhysicalLibraryPin(lpin);
        if (phLibPin) {
                Rsyn::PhysicalLibraryCell phLibCell =
                    phLibPin.getPhysicalLibraryCell();
                const Rsyn::PhysicalTransform &transform =
                    phLibCell.getTransform(orientation);

                if (phLibPin.hasPinGeometries()) {
                        for (Rsyn::PhysicalPinGeometry phPinPort :
                             phLibPin.allPinGeometries()) {
                                for (Rsyn::PhysicalPinLayer phPinLayer :
                                     phPinPort.allPinLayers()) {
                                        if (phPinLayer.hasPolygonBounds()) {
                                                for (const Rsyn::Polygon
                                                         &polygon :
                                                     phPinLayer.allPolygons()) {
                                                        QPainterPath path;
                                                        path.addPolygon(
                                                            QtUtils::convert(
                                                                transform.apply(
                                                                    polygon)));
                                                        shape += path;
                                                }  // end for
                                        }          // end if
                                        if (phPinLayer.hasRectangleBounds()) {
                                                for (const Bounds &bounds :
                                                     phPinLayer.allBounds()) {
                                                        QPainterPath path;
                                                        path.addRect(
                                                            QtUtils::convert(
                                                                transform.apply(
                                                                    bounds)));
                                                        shape += path;
                                                }  // end for
                                        }          // end if
                                }                  // end for
                        }                          // end for
                }                                  // end if
        }                                          // end if

        return shape.simplified();
}  // end method

}  // end method
