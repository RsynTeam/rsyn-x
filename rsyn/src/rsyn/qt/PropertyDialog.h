#ifndef RSYN_QT_PROPERTY_DIALOG_H
#define RSYN_QT_PROPERTY_DIALOG_H

#include <QDialog>
#include "ui_PropertyDialog.h"

#include <iostream>

namespace Rsyn {

namespace QtUtils {
class QTreeDescriptor;
} // end namespace

class PropertyDialog : public QDialog, public Ui::PropertyDialog {
    Q_OBJECT
public:
    PropertyDialog(QWidget *parent = 0);
	void setProperties(const QtUtils::QTreeDescriptor &properties);
private:

}; // end class

} // end namespace

#endif
