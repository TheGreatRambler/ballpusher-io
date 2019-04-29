#include <string>

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/InputEvents.h>

// The scenes (homepage now)
#include "source/platformerLevels.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif

using namespace Urho3D;

class mainApp : public Application {
	URHO3D_OBJECT(TutorialApp, Application)
private:
	ResourceCache* cache;
	platformerLevel* currentLevel;
	/*
	 These are the base variables avaliable to the application
	   SharedPtr<Engine> engine_;
	   VariantMap engineParameters_;
	*/

	void setCurrentScene(std::string name) {
		// at some point, select scene based on name
		currentLevel = &platformerLevel(context_);
		currentLevel->setupCurrentScene(name);
		currentLevel->setCache(cache);
	}

public:
	TutorialApp(Context* context) : Application(context) {
	}
	virtual void Setup() {
		// Setup the engine parameters and other things
		engineParameters_[EP_WINDOW_TITLE] = "Ballpusher IO";
		engineParameters_[EP_FULL_SCREEN] = false;
		engineParameters_[EP_HEADLESS] = false;
		engineParameters_[EP_SOUND] = false;
		// resource path will now be "bin/assets" (will pack so
		//   it is "bin/assets.pak" instead)
		engineParameters_[EP_RESOURCE_PATHS] = "assets;";
	}
	virtual void Start() {
		cache = GetSubsystem<ResourceCache>();
		SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(mainApp, HandleKeyDown));

		// for now
		setCurrentScene("TEST");
	}
	virtual void Stop() {
	}

	void HandleKeyDown(StringHash eventType, VariantMap& eventData) {
		using namespace KeyDown;
		// Check for pressing ESC. Note the engine_ member variable for convenience
		// access to the Engine object
		int key = eventData[P_KEY].GetInt();
#if !defined(__EMSCRIPTEN__)
		// only exits with ESC on desktop
		if (key == KEY_ESCAPE)
			engine_->Exit();
#endif
	}
};

URHO3D_DEFINE_APPLICATION_MAIN(mainApp)
