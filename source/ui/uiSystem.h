#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/UI.h>

#include <RmlUi/Core.h>

#include "Urho3DRenderInterface.h"
#include "Urho3DSystemInterface.h"

class uiSystem {
private:
	// For the UI
	Urho3DSystemInterface rocketSystemInterface;
	Urho3DRenderInterface rocketRenderer;
	Rml::Core::Context* uiContext;

	Urho3D::Context* uC;
	Urho3D::Graphics* gSub;

	Rml::Core::ElementDocument* mainDocument;

public:
	uiSystem(Urho3D::Context* context, Urho3D::Graphics* gS) : rocketSystemInterface(context), rocketRenderer(context) {
		uC = context;
		gSub = gS;

		Rml::Core::SetRenderInterface(&rocketRenderer);
		Rml::Core::SetSystemInterface(&rocketSystemInterface);

		Rml::Core::Initialise();

		// Get screen width and height
		int w = gSub->GetWidth();
		int h = gSub->GetHeight();

		uiContext = Rml::Core::CreateContext("main", Rml::Core::Vector2i(w, h));

		// Load fonts (Only regular for now)
		Rml::Core::FontDatabase::LoadFontFace("AssetsData/fonts/opensans/OpenSans-Regular.ttf");

		// Load main menu (May be all menus in future)
		mainDocument = uiContext->LoadDocument("AssetsData/ui/html/mainMenu.html");

		// Show it
		mainDocument->Show();
	}

	void update() {
		// Simply update the entire context
		uiContext->Update();
	}

	void render() {
		uiContext->Render();
	}

	void shutdown() {
		uiContext->RemoveReference();
		Rml::Core::Shutdown();
	}
};