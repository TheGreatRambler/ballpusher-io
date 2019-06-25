#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>
#include <memory>
#include <queue>
#include <string.h>

#include "menuBase.h"

#include "menus/pauseMenu.h"

class menuHandler {
private:
	std::queue<std::unique_ptr<MenuBase>> menus;

	Urho3D::Scene* currentScene;
	Urho3D::UI* ui;
	Urho3D::Context* context;

public:
	menuHandler(Urho3D::UI* ui_, Urho3D::Context* context_, Urho3D::Scene* cS) {
		currentScene = cS;
		ui = ui_;
		context = context_;
	}

	void openMenu(int menuType) {
		if (menus.empty()) {
			// this is the first menu to open
			// so pause scene
			currentScene->SetUpdateEnabled(false);
		} else {
			// remove last menu
			menus.front()->setOpen(false);
			// automatically deleted
			menus.pop();
		}

		std::unique_ptr<MenuBase> newMenu;
		if (menuType == 0) {
			// pause menu
			newMenu = new PauseMenu(ui, context);
			newMenu->setOpen(true);
		}
		menus.push(newMenu);
	}

	void closeCurrentMenu() {
	}
};