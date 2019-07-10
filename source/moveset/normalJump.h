#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/Node.h>

#include "../moveset.h"

class normalJump : public MoveBase {
private:
	Urho3D::Vector3 jumpVector;

	int stockpiledInputs[1][3];

public:
	void init() {
		// Normal jump height
		jumpVector = Urho3D::Vector3(0, 10, 0);

		requirementLength = 1;
		physicsWorldMatching = { 0 };
		inputMatching = { 1 };
		movementMatching = { 2 };
	}

	void performMove() {
		// Will handle animation later
	};

	void update(float delta) {
		// 4 seconds long
		helperHoldToJump(jumpVector, delta, 4.0);
	}
};