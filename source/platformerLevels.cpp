#include <string>

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLElement.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>

#include "platformerLevels.h"


platformerLevel::platformerLevel(SharedPtr<Context> theContext) {
	context_ = theContext;
}

platformerLevel::setCache(ResourceCache* theCache){ cache = cache }

platformerLevel::setupCurrentScene(std::string name) {
	// Let's setup a scene to render.
	scene_ = new Scene(context_);

	XMLFile* sceneFile = cache->GetResource<XMLFile>("scenes/levels/" + name + "/files/Scene.xml");
	// set scene to the XML file
	scene_->LoadXML(sceneFile->GetRoot());
	// Might need these
	// Let the scene have an Octree component!
	// scene_->CreateComponent<Octree>();
	// Let's add an additional scene component for fun.
	// scene_->CreateComponent<DebugRenderer>();
	SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
}