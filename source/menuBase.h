#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>
#include <queue>
#include <string.h>

class MenuBase {
private:
	Urho3D::Sprite* rootSprite;
	Urho3D::Window* window;
	Urho3D::Text* titleText;

	Urho3D::UI* ui;
	Urho3D::Context* context;

	// Doesnt exist yet
	MenuBase* previousMenu = nullptr;
	MenuBase* child = nullptr;

public:
	MenuBase(Urho3D::UI* ui_, Urho3D::Context* context_) {
		ui = ui_;
		context = context_;

		rootSprite = ui_->GetRoot()->CreateChild<Urho3D::Sprite>();
		rootSprite->SetEnabled(true);
		rootSprite->SetPosition(0, 0);
		// Bu default, invisible
		rootSprite->SetVisible(false);

		window = rootSprite->CreateChild<Urho3D::Window>();
		window->SetMinWidth(500);
		window->SetLayout(Urho3D::LayoutMode::LM_VERTICAL, 6, Urho3D::IntRect(6, 6, 6, 6));
		window->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
		window->BringToFront();

		titleText = window->CreateChild<Urho3D::Text>();
		titleText->SetText("Menu");
		// aligned at top of menu
		titleText->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_TOP);
	}

	~MenuBase() {
		// Just delee all of it until I start using smart pointers
		delete rootSprite;
		delete window;
		delete titleText;
		// Dont delete all of them because some are subsystems
	}

	// Like abstract void
	// To be defined by children
	virtual void setup() = 0;

	void setName(char menuName[]) {
		rootSprite->SetName(menuName);
		titleText->SetText(menuName);
	}

	void setVisible(bool o) {
		rootSprite->SetVisible(o);
	}

	void removeChild() {
		delete child;
		child = nullptr;
	}

	void close() {
		previousMenu->setup();
		previousMenu->setVisible(true);
		// Removes this menu
		previousMenu->removeChild();
	}

	void setPreviousMenu(MenuBase* pM) {
		previousMenu = pM;
	}

	void openNewMenu(MenuBase* newMenu) {
		// Menus keep track of their history
		newMenu->setPreviousMenu(this);
		child = newMenu;
		newMenu->setup();
		newMenu->setVisible(true);
		setVisible(false);
	}
};