#include <exception>

#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/io/Graphics.h"

#include "PropertyDialog.h"
#include "rsyn/qt/QtUtils.h"

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

PropertyDialog::PropertyDialog(QWidget *parent) :
	QDialog(parent)
{
	setupUi(this);
} // end constructor

// -----------------------------------------------------------------------------

void
PropertyDialog::setProperties(const QtUtils::QTreeDescriptor &properties) {
	QtUtils::populateQTreeWidget(properties, treeWidget);
} // end method

} // end namespae