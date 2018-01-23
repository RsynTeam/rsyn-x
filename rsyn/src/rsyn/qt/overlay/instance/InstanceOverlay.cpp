
#include "rsyn/session/Session.h"
#include "rsyn/phy/PhysicalDesign.h"

#include "rsyn/qt/GraphicsView.h"
#include "InstanceOverlay.h"
#include "CellItem.h"
#include "MacroItem.h"

#include <limits>
#include <QtWidgets>
#include <QColor>

// -----------------------------------------------------------------------------

namespace Rsyn {

// -----------------------------------------------------------------------------

InstanceOverlay::InstanceOverlay(QGraphicsItem *parent) :
		GraphicsOverlay("Instances", parent) {
} // end method

// -----------------------------------------------------------------------------

InstanceOverlay::~InstanceOverlay() {
} // end destructor

// -----------------------------------------------------------------------------

bool InstanceOverlay::init(GraphicsView *view, std::vector<GraphicsLayerDescriptor> &visibilityItems) {
	Rsyn::Session session;
	Rsyn::Design design = session.getDesign();
	Rsyn::Module module = session.getTopModule();
	Rsyn::PhysicalDesign physicalDesign = session.getPhysicalDesign();

	if (!physicalDesign)
		return false;

	clsView = view;
	clsInstanceMap = design.createAttribute(nullptr);

	for (Rsyn::Instance instance : module.allInstances()) {
		if (instance.getType() != Rsyn::CELL)
			continue;

		Rsyn::PhysicalCell physicalCell =
				physicalDesign.getPhysicalInstance(instance).asPhysicalCell();

		QGraphicsItem *item = nullptr;
		if (instance.isMacroBlock()) {
			item = new MacroGraphicsItem(physicalCell);
		} else {
			item = new CellGraphicsItem(physicalCell);
		} // end else
		
		item->setParentItem(this);
		clsInstanceMap[instance] = item;
	} // end for

	{ // Command: select (@todo move elsewhere)
		ScriptParsing::CommandDescriptor dscp;
		dscp.setName("select");
		dscp.setDescription("Selects an object in the current view.");

		dscp.addPositionalParam("name",
			ScriptParsing::PARAM_TYPE_STRING,
			ScriptParsing::PARAM_SPEC_MANDATORY,
			"Name of the object to be selected.");

		dscp.addNamedParam("clear",
			ScriptParsing::PARAM_TYPE_BOOLEAN,
			ScriptParsing::PARAM_SPEC_OPTIONAL,
			"Clear selection.",
			"false");

		session.registerCommand(dscp, [&](const ScriptParsing::Command &command) {
			const std::string name = command.getParam("name");
			const bool clear = command.getParam("clear");

			if (clear) {
				clsView->scene()->clearSelection();
			} // end if

			Rsyn::Session session;
			Rsyn::Design design = session.getDesign();
			Rsyn::Instance instance = design.findInstanceByName(name);

			if (!instance) {
				std::cout << "INFO: Instance " << name << " not found.\n";
			} else {
				QGraphicsItem *item = clsInstanceMap[instance];
				if (item) {
					item->setSelected(true);
					clsView->ensureVisible(item);
				} // end if
			} // end if
		});
	} // end scope

	view->registerObserver(this);
	return true;
} // end method

// -----------------------------------------------------------------------------

void
InstanceOverlay::onChangeVisibility(const std::string &key, const bool visible) {
	if (key == "Instances") {
		setVisible(visible);
	} else if (key == "Instances/Cells") {
		std::cout << "TODO: Show/Hide Instances.Cells\n";
	} else if (key == "Instances/Macros") {
		std::cout << "TODO: Show/Hide Instances.Macros\n";
	} // end else
} // end method

// -----------------------------------------------------------------------------

} // end namespace