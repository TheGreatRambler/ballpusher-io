#include <Urho3D/Engine/Engine.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>

#include "../SettingsGlobal.h"
#include "../menuBase.h"

class PauseMenu : public MenuBase {
	void setup() {
		setName("Pause");
	}
};