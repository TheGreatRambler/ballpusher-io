#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>

#include "controls.h";

enum CURRENT_PLAYER_PHYSICS_STATE {
	STANDING,
	WALKING,
	RUNNING,
	FALLING,
	SLIDING,
};

class playerPhysics {
private:
	Urho3D::PhysicsWorld* world;
	Urho3D::RigidBody* player;

	inputHandler* inputSystem;

	CURRENT_PLAYER_PHYSICS_STATE currentState = STANDING;

	/*
				 FORWARDS

					 0
			   315       45

		LEFT  270   DIR   90  RIGHT

			   225       135
					180

				 BACKWARDS
	*/


	// Relating to walking
	double timeStartedWalking = 0;
	int currentWalkingSpeed = 0;
	int MAX_RUN_SPEED = 20;

	// Related to falling
	int startingFallingSpeed = 50;
	int currentFallingSpeed = 0;
	int fallingAcceleration = 2;
	int MAX_FALLING_SPEED = 70;

public:
	playerPhysics(Urho3D::PhysicsWorld* pW) {
		world = pW;
		// Gravity handled by this
		player->SetUseGravity(false);
		// Prevent body from rotating unless we specifically allow it
		player->SetAngularFactor(Urho3D::Vector3(0, 0, 0));
	}

	void update(double delta) {
		// Delta is recieved in seconds
		if (currentState != FALLING) {
			if (currentState != STANDING) {
			}
		} else {
			// Accelerate falling
			if (currentFallingSpeed == 0) {
				// PLayer just started falling
				currentFallingSpeed = startingFallingSpeed;
			} else {
				if (currentFallingSpeed + fallingAcceleration >= MAX_FALLING_SPEED) {
					// At the cusp
					currentFallingSpeed = MAX_FALLING_SPEED;
				} else {
					// Simple acceleration
					currentFallingSpeed += fallingAcceleration;
				}
			}

			// Do fancy stuff
			Urho3D::PhysicsRaycastResult raycast;
			// Raycast straight down
			Urho3D::Ray rayStraightDown = Urho3D::Ray(player->GetPosition(), Urho3D::Vector3(0, -1, 0));
			world->RaycastSingle(raycast&, rayStraightDown&, MAX_FALLING_SPEED * 2);
		}
	}

	void setInputSystem(inputHandler* iS) {
		inputSystem = iS;
	}
};