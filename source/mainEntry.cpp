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

#include "SettingsGlobal.h"
#include "controls.h"

#include "ui/uiSystem.h"

class MainEntry : public Urho3D::Application {
	// Enable events
	URHO3D_OBJECT(MainEntry, Application);

private:
	// I am not using initialization lists because I am bad

	// A list of subsystems to help people out
	Urho3D::ResourceCache* cache;
	Urho3D::Graphics* graphics;
	Urho3D::UI* ui;
	Urho3D::Input* input;
	Urho3D::Renderer* renderer;

	// Input handler
	inputHandler* controls;

	// UI handler (I name things badly)
	uiSystem* uiHandler;

public:
	// Starting constructor
	MainEntry(Urho3D::Context* context) : Application(context) {
		// Had to init them
	}

	void Setup() {
		// Modify engine startup parameters
		engineParameters_["WindowTitle"] = GLOBAL_SETTINGS::windowName;
		engineParameters_["FullScreen"] = false;
		engineParameters_["Headless"] = false;
		engineParameters_["Sound"] = false;
		engineParameters_["LogQuiet"] = true;

		engineParameters_["WindowWidth"] = GLOBAL_SETTINGS::screenWidth;
		engineParameters_["WindowHeight"] = GLOBAL_SETTINGS::screenHeight;

		// Add resources
		engineParameters_["ResourcePaths"] = "Data;CoreData;AssetsData";

		// Construct a search path to find the resource prefix with two entries:
		// The first entry is an empty path which will be substituted with program/bin directory -- this entry is for binary when it is still in build tree
		// The second and third entries are possible relative paths from the installed program/bin directory to the asset directory -- these entries are for binary when it is in the Urho3D SDK installation location
		// if (!engineParameters_.Contains(EP_RESOURCE_PREFIX_PATHS))
		//	engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ";../share/Resources;../share/Urho3D/Resources";
	}

	void Start() {
		initSubsystems();
		subscribeToEvents();
		createControls();
		setupUI();
	}

	void initSubsystems() {
		// Set these variables to the subsystems so they are easily accessable
		cache = GetSubsystem<Urho3D::ResourceCache>();
		graphics = GetSubsystem<Urho3D::Graphics>();
		ui = GetSubsystem<Urho3D::UI>();
		input = GetSubsystem<Urho3D::Input>();
		renderer = GetSubsystem<Urho3D::Renderer>();
	}

	void subscribeToEvents() {
		// Simple update loop
		SubscribeToEvent(Urho3D::E_UPDATE, URHO3D_HANDLER(MainEntry, onSceneUpdate));
		// After rendering (Used mainly for the UI)
		SubscribeToEvent(Urho3D::E_POSTRENDERUPDATE, URHO3D_HANDLER(MainEntry, onSceneEndRendering));
	}

	void createControls() {
		// Create player 0 (or player 1)
		controls = new inputHandler(0, input);
	}

	void setupUI() {
		// Start UI
		uiHandler = new uiSystem(context_, graphics);
	}

	void onSceneUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
		// Handle update
		// Timestep
		float timeStep = eventData[Urho3D::Update::P_TIMESTEP].GetFloat();

		// Update UI
		uiHandler->update();

		if (input->GetKeyDown(Urho3D::KEY_ESCAPE)) {
			Stop();
		}
	}

	void onSceneEndRendering(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
		uiHandler->render();
	}

	void Stop() {
		// engine_->DumpResources(true);
		uiHandler->shutdown();
		engine_->Exit();
	}

	void setWindowNameAndIcon() {
		// Set window title and icon
		Urho3D::Image* icon = cache->GetResource<Urho3D::Image>("textures/ui/logo/logo.png");
		graphics->SetWindowIcon(icon);
	}
};

URHO3D_DEFINE_APPLICATION_MAIN(MainEntry)