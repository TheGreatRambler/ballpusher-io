#include <string>

#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

class platformerLevel {
private:
	SharedPtr<Scene> scene_;
	SharedPtr<Context> context_;
	ResourceCache* cache;
	Node* playerNode;
	Node* cameraNode_;

public:
	bool upHeld = false;
	bool downHeld = false;
	bool leftHeld = false;
	bool rightHeld = false;
	bool jumpHeld = false;

	platformerLevel(SharedPtr<Context> theContext);
	void setupCurrentScene(std::string name);
	void setCache(ResourceCache* theCache);
	void update();
}