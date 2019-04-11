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

#ifndef RSYN_GRAPHICS_H
#define RSYN_GRAPHICS_H

//! @brief This class provides utilities for the graphics user interface (GUI),
//! but it's not dependent on any specific GUI framework (e.g. wxWidgets, QT).

// TODO: This class needs a deep clean up... 2016-10-31

#include "core/Rsyn.h"
#include "phy/PhysicalDesign.h"
#include "session/Session.h"
#include "util/Color.h"
#include "util/Stipple.h"
#include "tool/timing/TimingTypes.h"

namespace Rsyn {

class Timer;

enum CanvasTheme {
        CANVAS_THEME_WHITE,
        CANVAS_THEME_BLACK,

        NUM_CANVAS_THEMES
};  // end enum

class RenderingStyle {
       public:
        RenderingStyle()
            : clsColor(255, 255, 255),
              clsFillPattern(STIPPLE_MASK_EMPTY),
              clsLineStyle(LINE_STIPPLE_NONE) {}  // end constructor

        RenderingStyle(const Color &color, const LineStippleMask &lineStyle,
                       const FillStippleMask &fillPattern)
            : clsColor(color),
              clsFillPattern(fillPattern),
              clsLineStyle(lineStyle) {}  // end constructor

        Color getColor() const { return clsColor; }
        FillStippleMask getFillPattern() const { return clsFillPattern; }
        LineStippleMask getLineStyle() const { return clsLineStyle; }

        void setColor(const Color &color) { clsColor = color; }
        void setFillPattern(const FillStippleMask &pattern) {
                clsFillPattern = pattern;
        }
        void setLineStyle(const LineStippleMask &style) {
                clsLineStyle = style;
        }

       private:
        Color clsColor;
        FillStippleMask clsFillPattern;
        LineStippleMask clsLineStyle;

};  // end class

enum GraphicsLayerZ {
        GRAPHICS_LAYER_BACKGROUND,
        GRAPHICS_LAYER_FLOORPLAN,
        GRAPHICS_LAYER_INSTANCE,
        GRAPHICS_LAYER_PIN,
        GRAPHICS_LAYER_ROUTING,
        GRAPHICS_LAYER_FOREGROUND,

        NUM_GRAPHICS_LAYERS
};  // end enum

class Graphics : public Service, public Rsyn::DesignObserver {
       public:
        virtual void start(const Rsyn::Json &params);
        virtual void stop();

        virtual void onPostInstanceCreate(Rsyn::Instance instance) override;

        bool isColoringEnabled() const { return clsColoringEnabled; }
        void setColoringEnabled(const bool coloringEnabled) {
                clsColoringEnabled = coloringEnabled;
        }

        //! @brief Get the color associated to an instance
        Color &getCellColor(Rsyn::Instance instance) {
                return clsInstanceColors[instance];
        }

        //! @brief Returns the z-order of a tech layer.
        float getPhysicalLayerZ(const Rsyn::PhysicalLayer layer) const {
                return getGraphicsLayerZ(GRAPHICS_LAYER_ROUTING,
                                         layer.getIndex());
        }  // end method

        //! @brief Returns a z order for a specific graphics layer.
        float getGraphicsLayerZ(const GraphicsLayerZ &layer) const {
                return (layer / NUM_GRAPHICS_LAYERS);
        }  // end method

        //! @brief Returns a z order for a specific graphics layer accounting
        //! for
        //! sub layers. The offset z order is given in the interval [layer z
        //! order,
        //! layer z order + 1).
        float getGraphicsLayerZ(const GraphicsLayerZ &layer,
                                const int offset) const {
                return (layer / NUM_GRAPHICS_LAYERS) + (offset / (offset + 1));
        }  // end method

        //! @brief Returns the current background color.
        Color getBackgroundColor() const {
                return clsBackgroundColor[clsCanvasTheme];
        }

        //! @brief Returns the rendering style of instances.
        const RenderingStyle &getInstanceRendering() const {
                return clsInstanceColor[clsCanvasTheme];
        }

        //! @brief Returns the rendering style of the highlighted objects.
        const RenderingStyle &getHighlightRendering() const {
                return clsHighlightColor[clsCanvasTheme];
        }

        //! @brief Returns the rendering style of the select objects.
        const RenderingStyle &getSelectionRendering() const {
                return clsSelectionColor[clsCanvasTheme];
        }

        //! @brief Returns the rendering style of a routing layer.
        const RenderingStyle &getRoutingLayerRendering(
            const int routingLayerId) const {
                return routingLayerId >= techLayerMasks.size()
                           ? techLayerMasks.back()
                           : techLayerMasks[routingLayerId];
        }  // end method

        //! @brief Sets the current canvas theme.
        void setCanvasTheme(const CanvasTheme &theme) {
                clsCanvasTheme = theme;
        }  // end method

        // Coloring schemes.
        void coloringRandomBlue();
        void coloringRandomGray();
        void coloringByCellType();
        void coloringColorful();
        void coloringGray();
        void coloringSlack(TimingMode mode);
        void coloringCriticality(const TimingMode mode, const double threshold);
        void coloringRelativity(const TimingMode mode);
        void coloringCentrality(const TimingMode mode, const double threshold);
        void coloringCriticalPath(const TimingMode mode,
                                  const bool showNeighbors);
        void coloringQDPMoved();
        void colorNeighbours(Rsyn::Instance);

       private:
        // Circuitry
        Rsyn::Design clsDesign;
        Rsyn::Module clsModule;

        // Physical Design
        Rsyn::PhysicalDesign clsPhysicalDesign;

        // Services
        Timer *clsTimer = nullptr;

        // Store instance colors.
        Rsyn::Attribute<Rsyn::Instance, Color> clsInstanceColors;

        CanvasTheme clsCanvasTheme = CANVAS_THEME_WHITE;

        bool clsColoringEnabled;

        static const std::array<Color, NUM_CANVAS_THEMES> clsBackgroundColor;
        static const std::array<RenderingStyle, NUM_CANVAS_THEMES>
            clsInstanceColor;
        static const std::array<RenderingStyle, NUM_CANVAS_THEMES>
            clsHighlightColor;
        static const std::array<RenderingStyle, NUM_CANVAS_THEMES>
            clsSelectionColor;
        static const std::array<RenderingStyle, 16> techLayerMasks;

};  // end class

}  // end namespace

#endif /* GRAPHICS_H */
