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

	XMLFile* sceneFile = cache->GetResource<XMLFile>("scenes/levels/" + name + "/files/Scenes/Scene.xml");
	// set scene to the XML file
	scene_->LoadXML(sceneFile->GetRoot());
	// create player
	playerNode = scene_->CreateChild("PlayerMain");
	playerNode->SetPosition(Vector3(0, 3, 0));
	playerNode->SetScale(Vector3(1, 1, 1));
	StaticModel* playerStaticModel = playerNode->CreateComponent<StaticModel>();
	playerNode->SetModel(cache->GetResource<Model>("creatures/player/normal/Models/Cube.mdl"));
	playerNode->SetMaterial(cache->GetResource<Material>("creatures/player/normal/Materials/Material.xml"));
	playerNode->SetCastShadows(true);

	// camera
	cameraNode_ = scene_->CreateChild("Camera");
	playerNode->SetPosition(Vector3(0, 3, -10));
	Camera* camera = cameraNode_->CreateComponent<Camera>();
	camera->SetFarClip(2000);

	SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
}

platformerLevel::update() {
	// face camera towards player
	cameraNode_->LookAt(playerNode->GetPosition());
}