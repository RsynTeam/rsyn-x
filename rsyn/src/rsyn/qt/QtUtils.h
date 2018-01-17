#ifndef RSYN_QT_UTILS_H
#define RSYN_QT_UTILS_H

#include <cstdio>
#include <vector>
#include <tuple>
#include <string>

#include <QColor>
#include <QPoint>
#include <QRect>
#include <QIcon>

#include "rsyn/util/Bounds.h"
#include "rsyn/util/dbu.h"
#include "rsyn/util/Color.h"

class QTreeWidget;
class QTreeWidgetItem;

namespace Rsyn {
namespace QtUtils {

class Item;

enum CheckState {
	NonCheckable,
	Checked,
	PartiallyChecked,
	Unchecked
}; // end enum

// -----------------------------------------------------------------------------

class QTreeColumnDescriptor {
public:

	int getIndex() const {
		return _index;
	} // end method

	const std::string &getText() const {
		return _text;
	} // end method

	const QIcon &getIcon() const {
		return _icon;
	} // end method

	void setIndex(const int index) {
		_index = index;
	} // end method

	void setText(const std::string &value) {
		_text = value;
	} // end method

	void setIcon(const QIcon &icon) {
		_icon = icon;
	} // end method

private:
	int _index = 0;
	std::string _text;
	QIcon _icon;
}; // end class

// -----------------------------------------------------------------------------

class QTreeEntryDescriptor {
public:

	const std::string &getKey() const {
		return _key;
	} // end method

	int getColumn() const {
		return _column;
	} // end method

	const std::string &getText() const {
		return _text;
	} // end method

	CheckState getCheckState() const {
		return _checkState;
	} // end method

	const QIcon &getIcon() const {
		return _icon;
	} // end method

	void setKey(const std::string &key) {
		_key = key;
	} // end method

	void setColumn(const int index) {
		_column = index;
	} // end method

	void setText(const std::string &value) {
		_text = value;
	} // end method

	void setText(const int value) {
		_text = std::to_string(value);
	} // end method
	
	void setText(const double value, const std::string &format = "") {
		const std::size_t BUFFER_SIZE = 100;
		char buffer[BUFFER_SIZE];
		snprintf(buffer, BUFFER_SIZE,
				format.empty()? "%f" : format.c_str(), value);
		_text = buffer;
	} // end method

	void setIcon(const QIcon &icon) {
		_icon = icon;
	} // end method

	void setCheckState(const CheckState checkState) {
		_checkState = checkState;
	} // end method

private:
	std::string _key;
	int _column = 1;
	std::string _text;
	QIcon _icon;
	CheckState _checkState = NonCheckable;
}; // end class

// -----------------------------------------------------------------------------

class QTreeDescriptor {
public:

	void addHeader(const int index, const std::string &name, const QIcon &icon = QIcon()) {
		QTreeColumnDescriptor item;
		item.setIndex(index);
		item.setText(name);
		item.setIcon(icon);
		_columns.push_back(item);
	} // end method

	void add(const std::string &key, const int column = 0, const std::string &text = "", const CheckState checkState = NonCheckable, const QIcon &icon = QIcon()) {
		QTreeEntryDescriptor item;
		item.setKey(key);
		item.setColumn(column);
		item.setText(text);
		item.setCheckState(checkState);
		item.setIcon(icon);
		_entries.push_back(item);
	} // end method

	const std::vector<QTreeColumnDescriptor> &
	allColumns() const {
		return _columns;
	} // end method


	const std::vector<QTreeEntryDescriptor> &
	allEntries() const {
		return _entries;
	} // end method

private:
	std::vector<QTreeEntryDescriptor> _entries;
	std::vector<QTreeColumnDescriptor> _columns;
}; // end class

// -----------------------------------------------------------------------------

QColor convert(const Color &color);

QPoint convert(const DBUxy &point);

QRect convert(const Bounds &bounds);

void populateQTreeWidget(const QTreeDescriptor &treeDescriptor, QTreeWidget *treeWidget, const std::string &seperator = "/");

QIcon createIcon(const QPen &pen, const QBrush &brush, const int w, const int h);

void
tracePathOutline(
		const std::vector<DBUxy> &pathPoints,
		const float halfThickness,
		std::vector<QPointF> &outlinePoints);

} // end namespace (QtUtils)
} // end namespace (Rsyn)

#endif /* QTUTILS_H */

