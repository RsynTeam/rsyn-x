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

/*
 * File:   DensityGridOverlay.h
 * Author: jucemar
 *
 * Created on June 1, 2018, 11:02 AM
 */

#ifndef DENSITYGRIDOVERLAY_H
#define DENSITYGRIDOVERLAY_H

#include "gui/graphics/LayoutGraphicsLayer.h"
#include "tool/congestion/DensityGrid.h"

#include <QColor>
#include <QGraphicsItem>
#include <QLine>

#include <array>
#include <vector>
#include <map>

namespace Rsyn {

class DensityGridOverlay : public Rsyn::LayoutGraphicsLayer {
       private:
        Rsyn::Design clsDesign;
        Rsyn::Module clsModule;
        Rsyn::PhysicalDesign clsPhDesign;
        //	Rsyn::Graphics *clsRsynGraphics = nullptr;
        QRectF bounds;
        Rsyn::DensityGrid *clsDensityGrid = nullptr;

       public:
        DensityGridOverlay() = default;
        DensityGridOverlay(const DensityGridOverlay &orig) = default;
        ~DensityGridOverlay() = default;

        virtual bool init(Rsyn::LayoutGraphicsScene *scene,
                          std::vector<Rsyn::GraphicsLayerDescriptor>
                              &visibilityItems) override;

        virtual void render(QPainter *painter, const float lod,
                            const QRectF &exposedRect) override;

       protected:
        void renderBins(QPainter *painter);
        void renderOverflow(QPainter *painter);
        void renderFreeArea(QPainter *painter);
};  // end class

}  // end namespace

#endif /* DENSITYGRIDOVERLAY_H */
