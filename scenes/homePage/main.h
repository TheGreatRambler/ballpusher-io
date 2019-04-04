// needed URHO3D headers
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/InputEvents.h>

#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>

class homePageScene {
	private:
		SharedPtr<Scene> thisScene;
	public:
		homePageScene(SharedPtr<Scene> scene) {
			thisScene = scene;
		}
}
