#include <Urho3D/Engine/Engine.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>

#include "../SettingsGlobal.h"

class pauseMenu {
private:
	UI* ui;
	Context* context;
	Window* window;

public:
	pauseMenu(UI* ui_, Context* context_) {
		ui = ui_;
		context = context_;
		window = new Window(context_);
		ui_->GetRoot()->AddChild(window);
		window->SetVisible(false);
		window->SetMinWidth(384);
		window->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
		window->SetAlignment(HA_CENTER, VA_CENTER);
		window->SetName("PauseMenu");
	}

	void openPauseMenu() {
		window->SetVisible(true);
	}

	void closePauseMenu() {
		window->SetVisible(false);
	}
};