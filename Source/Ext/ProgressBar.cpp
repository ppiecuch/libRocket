#include "../../Include/Rocket/Ext.h"
#include "../../Include/Rocket/Ext/ElementProgressBar.h"
#include "../../Include/Rocket/Core/ElementInstancerGeneric.h"
#include "../../Include/Rocket/Core/Factory.h"
#include "../../Include/Rocket/Core/StyleSheetSpecification.h"
#include "../../Include/Rocket/Core/XMLParser.h"
#include "../../Include/Rocket/Core/Plugin.h"
#include "../../Include/Rocket/Core/Core.h"

namespace Rocket {
namespace ProgressBar {

void RegisterElementInstancers()
{
    Core::ElementInstancer* instancer = new Core::ElementInstancerGeneric< ElementProgressBar >();
    Core::Factory::RegisterElementInstancer("progressbar", instancer);
    instancer->RemoveReference();
}

static bool initialised = false;

class ProgressBarPlugin : public Rocket::Core::Plugin
{
public:
	void OnShutdown()
	{
		initialised = false;
		delete this;
	}

	int GetEventClasses()
	{
		return Rocket::Core::Plugin::EVT_BASIC;
	}
};

void Initialise()
{
	// Prevent double initialisation
	if (!initialised)
	{
		RegisterElementInstancers();

		// Register the progress bar plugin, so we'll be notified on Shutdown
		Rocket::Core::RegisterPlugin(new ProgressBarPlugin());

		initialised = true;
	}
}

}
}
