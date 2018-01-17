#include "QtUtils.h"

#include "rsyn/util/dbu.h"
#include "rsyn/util/float2.h"

#include <iostream>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <QTreeWidget>
#include <QHeaderView>
#include <QPainter>

namespace Rsyn {
namespace QtUtils {

// -----------------------------------------------------------------------------
// Helper Functions (Non-Public)
// -----------------------------------------------------------------------------

float2
findIntersection(
		float2 p1,
		float2 p2,
		float2 d1,
		float2 d2);

bool
findMyPoint(
		float2 v0,
		float2 v1,
		float2 v2,
		const float thickness,
		std::vector<QPointF> &outlinePoints);

// -----------------------------------------------------------------------------
// Public Function
// -----------------------------------------------------------------------------

QColor
convert(const Color &color) { 
	return QColor(color.r, color.g, color.b);
} // end function

// -----------------------------------------------------------------------------

QPoint
convert(const DBUxy &point) { 
	return QPoint(point.x, point.y);
} // end function

// -----------------------------------------------------------------------------

QRect convert(const Bounds &bounds) {
	return QRect(bounds.getLower().x, bounds.getLower().y /*upper? no all scene is y-inverted*/,
			bounds.getWidth(), bounds.getHeight());
} // end method

// -----------------------------------------------------------------------------

void
populateQTreeWidget(const QTreeDescriptor &treeDescriptor, QTreeWidget *treeWidget, const std::string &separator) {
	treeWidget->clear();
	treeWidget->setEnabled(false);
	treeWidget->header()->setStretchLastSection(false);

	// Populate header.
	QTreeWidgetItem* header = treeWidget->headerItem();
	for (const QTreeColumnDescriptor &column : treeDescriptor.allColumns()) {
		header->setText(column.getIndex(), QString::fromStdString(column.getText()));
		if (!column.getIcon().isNull()) {
			header->setIcon(column.getIndex(), column.getIcon());
		} // end if
	} // end for

	// Create rows.
	std::map<std::string, QTreeWidgetItem *> hierarchy;

	for (const QTreeEntryDescriptor &entry : treeDescriptor.allEntries()) {
		typedef boost::char_separator<char> ChSep;
		typedef boost::tokenizer<ChSep> TknChSep;
		ChSep sep(separator.c_str());
		TknChSep tok(entry.getKey(), sep);

		std::string hierarchicalName;
		QTreeWidgetItem *parent = nullptr;

		for (TknChSep::iterator beg = tok.begin(); beg != tok.end(); ++beg) {
			const std::string &name = *beg;
			if (!hierarchicalName.empty()) {
				hierarchicalName += separator;
			} // end if
			hierarchicalName += name;

			// Check if this item was already created, if not create an item
			// and add it to the tree.
			QTreeWidgetItem *current = nullptr;

			auto it = hierarchy.find(hierarchicalName);
			if (it == hierarchy.end()) {
				current = parent? new QTreeWidgetItem(parent) :
					new QTreeWidgetItem(treeWidget);
				current->setText(0, QString::fromStdString(name));
				current->setExpanded(true);
				hierarchy[hierarchicalName] = current;
			} else {
				current = it->second;
			} // end else

			parent = current;
		} // end for
	} // end for

	// Populate row/columns.
	for (const QTreeEntryDescriptor &item : treeDescriptor.allEntries()) {
		QTreeWidgetItem *current = hierarchy[item.getKey()];

		if (item.getColumn() > 0) {
			current->setText(item.getColumn(), 
					QString::fromStdString(item.getText()));
		} // end if

		switch (item.getCheckState()) {
			case CheckState::Checked:
				current->setCheckState(item.getColumn(), Qt::Checked);
				break;
			case CheckState::Unchecked:
				current->setCheckState(item.getColumn(), Qt::Unchecked);
				break;
			case CheckState::PartiallyChecked:
				current->setCheckState(item.getColumn(), Qt::PartiallyChecked);
				break;
			case CheckState::NonCheckable:
				break;
		} // end switch
		
		if (!item.getIcon().isNull()) {
			current->setIcon(item.getColumn(), item.getIcon());
		} // end
	} // end for

	// Adjust column sizes.
	if (treeWidget->columnCount()) {
		treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	} // end if
	
	for (int i = 1; i < treeWidget->columnCount(); i++) {
		treeWidget->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
	} // end for

	treeWidget->setEnabled(true);
} // end method

// -----------------------------------------------------------------------------

QIcon
createIcon(const QPen &pen, const QBrush &brush, const int w, const int h) {
	QPixmap pixmap(w, h);
	QPainter painter(&pixmap);

	painter.fillRect(0, 0, w, h, Qt::white);

	painter.setBrush(brush);
	painter.setPen(pen);

	painter.drawRect(0, 0, w-1, h-1);

	return QIcon(pixmap);
} // end method

// -----------------------------------------------------------------------------

void
tracePathOutline(
		const std::vector<DBUxy> &pathPoints,
		const float halfThickness,
		std::vector<QPointF> &outlinePoints
) {

	// Historical Note: This function was adapted from wxSightGL developed by
	// Lenna and Guilherme around 2007.

	float2 v1;

	// First point
	v1 = float2( pathPoints[0] ) +
			( float2( pathPoints[1] ) - float2( pathPoints[0] ) ).perpendicular().normalized() * halfThickness;
	outlinePoints.push_back(QPointF(v1.x, v1.y));

	// Forward traversal.
	bool control = true;
	for( size_t i = 1; i < pathPoints.size() - 1; i++){
		control = findMyPoint(
				float2(pathPoints[i-1]),
				float2(pathPoints[i  ]),
				float2(pathPoints[i+1]),
				halfThickness, outlinePoints);
	} // end for

	// Control point.
	if (control) {
		v1 = float2(pathPoints.back()) + float2( float2( pathPoints.back() ) -
				float2( pathPoints[pathPoints.size()-2] ) ).perpendicular().normalized() * halfThickness ;
		outlinePoints.push_back(QPointF(v1.x, v1.y));
	} // end if

	// Last point.
	v1 = float2( pathPoints.back() ) + float2( float2( pathPoints[pathPoints.size()-2] ) -
			float2( pathPoints.back() ) ).perpendicular().normalized() * halfThickness;
	outlinePoints.push_back(QPointF(v1.x, v1.y));

	// Backward traversal.
	for( int i = pathPoints.size() - 2; i >= 1; i--){
		findMyPoint(
				float2(pathPoints[i+1]),
				float2(pathPoints[i  ]),
				float2(pathPoints[i-1]),
				halfThickness, outlinePoints );
	} // end if

	v1 = float2( pathPoints[0] ) + float2( float2( pathPoints[0] ) -
			float2( pathPoints[1] ) ).perpendicular().normalized() * halfThickness ;
	outlinePoints.push_back(QPointF(v1.x, v1.y));
	v1 = float2( pathPoints[0] ) + float2( float2( pathPoints[1] ) -
			float2( pathPoints[0] ) ).perpendicular().normalized() * halfThickness ;
	outlinePoints.push_back(QPointF(v1.x, v1.y));
} // end method

// -----------------------------------------------------------------------------

bool
findMyPoint(
		float2 v0,
		float2 v1,
		float2 v2,
		const float thickness,
		std::vector<QPointF> &outlinePoints
) {

	// Historical Note: This function was adapted from wxSightGL developed by
	// Lenna and Guilherme around 2007.

	// Direction vectors.
	float2 d1 = v1 - v0;
	float2 d2 = v2 - v1;

	// Points over the lines.
	float2 p1 = v1 + (d1.perpendicular().normalized()) * thickness;
	float2 p2 = v1 + (d2.perpendicular().normalized()) * thickness;

	float m1 = (v0.x - v1.x) == 0 ? (v0.y - v1.y) : (v0.y - v1.y) / (v0.x - v1.x);
	float m2 = (v1.x - v2.x) == 0 ? (v1.y - v2.y) : (v1.y - v2.y) / (v1.x - v2.x);

	float tg = (m1 - m2) / (1 + m1 * m2);
	float distancia = std::sqrt(std::pow((p1.x - p2.x), 2.0f) + std::pow((p1.y - p2.y), 2.0f));
	float limite = std::sqrt(2.0f * std::pow(thickness, 2.0f));

	if (distancia > limite && tg > 0.0f) { // dois
		outlinePoints.push_back(QPointF(p1.x, p1.y));
		outlinePoints.push_back(QPointF(p2.x, p2.y));
		return true;
	} else {
		float2 q = findIntersection(p1, p2, d1, d2); // um
		outlinePoints.push_back(QPointF(q.x, q.y));
		return true;
	}//end else
} // end method

// -----------------------------------------------------------------------------

float2
findIntersection(
		float2 p1,
		float2 p2,
		float2 d1,
		float2 d2
) {

	// Historical Note: This function was adapted from wxSightGL developed by
	// Lenna and Guilherme around 2007.

	const float a = p1.x;
	const float e = p1.y;
	const float b = d1.x;
	const float f = d1.y;
	const float c = p2.x;
	const float g = p2.y;
	const float d = d2.x;
	const float h = d2.y;

	float t;
	if ((b * h - d * f) == 0) t = 0;
	else t = ((c - a) * h + (e - g) * d) / (b * h - d * f);
	float2 q = p1 + (d1 * t);

	bool bug;
	if ((t < 0 || t > 1)) {
		bug = true;
	} else {
		bug = false;
	} // end else

	return q;
} // end function

} // end namespace (QtUtils)
} // end namespace (Rsyn)

