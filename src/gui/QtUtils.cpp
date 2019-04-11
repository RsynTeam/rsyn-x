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

#include "QtUtils.h"

#include "util/DBU.h"
#include "util/Float2.h"

#include "util/Point.h"
#include "util/Rect.h"
#include "util/Polygon.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <QTreeWidget>
#include <QHeaderView>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QSize>

namespace Rsyn {
namespace QtUtils {

// -----------------------------------------------------------------------------
// Public Function
// -----------------------------------------------------------------------------

QColor convert(const Color &color) {
        return QColor(color.r, color.g, color.b);
}  // end function

// -----------------------------------------------------------------------------

QPoint convert(const DBUxy &point) {
        return QPoint(point.x, point.y);
}  // end function

// -----------------------------------------------------------------------------

QRect convert(const Bounds &bounds) {
        return QRect(bounds.getLower().x,
                     bounds.getLower().y /*upper? no all scene is y-inverted*/,
                     bounds.getWidth(), bounds.getHeight());
}  // end method

// -----------------------------------------------------------------------------

QPoint convert(const Rsyn::Point &point) {
        return QPoint(point.getX(), point.getY());
}  // end method

// -----------------------------------------------------------------------------

QPolygon convert(const Rsyn::Polygon &polygon) {
        QPolygon qpoly;
        for (const Rsyn::Point &point : polygon.allPoints()) {
                qpoly.append(QtUtils::convert(point));
        }  // end method
        return qpoly;
}  // end method

// -----------------------------------------------------------------------------

std::string toString(const QRectF &rect) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "(" << rect.x() << ", " << rect.y() << "; " << rect.width()
            << ", " << rect.height() << ")";
        return oss.str();
}  // end method

// -----------------------------------------------------------------------------

std::string toString(const QRect &rect) {
        std::ostringstream oss;
        oss << "(" << rect.x() << ", " << rect.y() << "; " << rect.width()
            << ", " << rect.height() << ")";
        return oss.str();
}  // end method

// -----------------------------------------------------------------------------

std::string toString(const QPoint &p) {
        std::ostringstream oss;
        oss << "(" << p.x() << ", " << p.y() << ")";
        return oss.str();
}  // end method

// -----------------------------------------------------------------------------

std::string toString(const QPointF &p) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "(" << p.x() << ", " << p.y() << ")";
        return oss.str();
}  // end method

// -----------------------------------------------------------------------------

std::string toString(const QSizeF &size) {
        return toString(QPointF(size.width(), size.height()));
}  // end method

// -----------------------------------------------------------------------------

std::string toString(const QSize &size) {
        return toString(QPoint(size.width(), size.height()));
}  // end method

// -----------------------------------------------------------------------------

void populateQTreeWidget(const QTreeDescriptor &treeDescriptor,
                         QTreeWidget *treeWidget,
                         const std::string &separator) {
        treeWidget->clear();
        treeWidget->setEnabled(false);
        treeWidget->header()->setStretchLastSection(false);

        // Populate header.
        QTreeWidgetItem *header = treeWidget->headerItem();
        for (const QTreeColumnDescriptor &column :
             treeDescriptor.allColumns()) {
                header->setText(column.getIndex(),
                                QString::fromStdString(column.getText()));
                if (!column.getIcon().isNull()) {
                        header->setIcon(column.getIndex(), column.getIcon());
                }  // end if
        }          // end for

        // Create rows.
        std::map<std::string, QTreeWidgetItem *> hierarchy;

        for (const QTreeEntryDescriptor &entry : treeDescriptor.allEntries()) {
                typedef boost::char_separator<char> ChSep;
                typedef boost::tokenizer<ChSep> TknChSep;
                ChSep sep(separator.c_str());
                TknChSep tok(entry.getKey(), sep);

                std::string hierarchicalName;
                QTreeWidgetItem *parent = nullptr;

                for (TknChSep::iterator beg = tok.begin(); beg != tok.end();
                     ++beg) {
                        const std::string &name = *beg;
                        if (!hierarchicalName.empty()) {
                                hierarchicalName += separator;
                        }  // end if
                        hierarchicalName += name;

                        // Check if this item was already created, if not create
                        // an item
                        // and add it to the tree.
                        QTreeWidgetItem *current = nullptr;

                        auto it = hierarchy.find(hierarchicalName);
                        if (it == hierarchy.end()) {
                                current = parent
                                              ? new QTreeWidgetItem(parent)
                                              : new QTreeWidgetItem(treeWidget);
                                current->setText(0,
                                                 QString::fromStdString(name));
                                current->setExpanded(true);
                                hierarchy[hierarchicalName] = current;
                        } else {
                                current = it->second;
                        }  // end else

                        parent = current;
                }  // end for
        }          // end for

        // Populate row/columns.
        for (const QTreeEntryDescriptor &item : treeDescriptor.allEntries()) {
                QTreeWidgetItem *current = hierarchy[item.getKey()];

                if (item.getColumn() > 0) {
                        current->setText(
                            item.getColumn(),
                            QString::fromStdString(item.getText()));
                }  // end if

                switch (item.getCheckState()) {
                        case CheckState::Checked:
                                current->setCheckState(item.getColumn(),
                                                       Qt::Checked);
                                break;
                        case CheckState::Unchecked:
                                current->setCheckState(item.getColumn(),
                                                       Qt::Unchecked);
                                break;
                        case CheckState::PartiallyChecked:
                                current->setCheckState(item.getColumn(),
                                                       Qt::PartiallyChecked);
                                break;
                        case CheckState::NonCheckable:
                                break;
                }  // end switch

                if (!item.getIcon().isNull()) {
                        current->setIcon(item.getColumn(), item.getIcon());
                }  // end
        }          // end for

        // Adjust column sizes.
        if (treeWidget->columnCount()) {
                treeWidget->header()->setSectionResizeMode(
                    0, QHeaderView::Stretch);
        }  // end if

        for (int i = 1; i < treeWidget->columnCount(); i++) {
                treeWidget->header()->setSectionResizeMode(
                    i, QHeaderView::ResizeToContents);
        }  // end for

        treeWidget->setEnabled(true);
}  // end method

// -----------------------------------------------------------------------------

QIcon createIcon(const QPen &pen, const QBrush &brush, const int w,
                 const int h) {
        QPixmap pixmap(w, h);
        QPainter painter(&pixmap);

        painter.fillRect(0, 0, w, h, Qt::white);

        painter.setBrush(brush);
        painter.setPen(pen);

        painter.drawRect(0, 0, w - 1, h - 1);

        return QIcon(pixmap);
}  // end method

}  // end namespace (QtUtils)
}  // end namespace (Rsyn)
