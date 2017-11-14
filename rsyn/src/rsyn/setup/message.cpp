#include "rsyn/session/Session.h"

// Registration
namespace Rsyn {
void Session::registerDefaultMessages() {
	registerMessage("TIMER-001", WARNING,
			"Unusual timing arc sense.",
			"Timing arc <arc> has the unusual (for academic tools) sense <sense>.");
	registerMessage("TIMER-002", WARNING,
			"Unusual timing arc type.",
			"Timing arc <arc> has the unusual (for academic tools) type <type>.");

	registerMessage("GRAPHICS-001", INFO,
			"Unable to initialize GLEW.");
	registerMessage("GRAPHICS-002", INFO,
			"Rendering to texture is not supported.");
} // end method
} // end namespace
