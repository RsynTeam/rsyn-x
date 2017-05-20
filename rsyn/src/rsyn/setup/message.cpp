#include "rsyn/engine/Engine.h"

// Registration
namespace Rsyn {
void Engine::registerDefaultMessages() {
	registerMessage("TIMER-001", WARNING,
			"Unusual timing arc sense.",
			"Timing arc <arc> has the unusual (for academic tools) sense <sense>.");
	registerMessage("TIMER-002", WARNING,
			"Unusual timing arc type.",
			"Timing arc <arc> has the unusual (for academic tools) type <type>.");
} // end method
} // end namespace
