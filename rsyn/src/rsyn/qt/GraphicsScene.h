#ifndef RSYN_GRAPHICS_SCENE_H
#define RSYN_GRAPHICS_SCENE_H

#include <QGraphicsScene>
#include <vector>

namespace Rsyn {

class GraphicsLayer;

class GraphicsScene : public QGraphicsScene {
public:

	GraphicsScene(QObject *parent = nullptr);
	~GraphicsScene();

	virtual void drawBackground(QPainter *painter, const QRectF &rect) override;
	virtual void drawForeground(QPainter *painter, const QRectF &rect) override;

	//! @brief Adds a graphics layer to this graphics scene. The graphics scene
	//! takes ownership of the graphics layer (i.e. it is responsible to delete
	//! it).
	void addLayer(GraphicsLayer *layer);

	// temp for experimentation
	bool clsHover = false;

private:

	
	std::vector<GraphicsLayer *> clsLayers;

}; // end class

} // end namespace

#endif