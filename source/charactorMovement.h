#include <Urho3D/Container/Vector.h>
#include <Urho3D/Math/Ray.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>

#include <algorithm>
#include <vector>

#include "controls.h"
#include "moveset.h"

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

	/*
		None actually interact with the world, they simply exist to check collisions
		There are two types:
			Touch: Check to see if touching
			Active: Check to see if can perform actions off them
				Wall jumps, ceiling grabs, etc
	*/

	Urho3D::RigidBody* touchTop;
	Urho3D::RigidBody* activeTop;
	Urho3D::RigidBody* touchBottom;
	Urho3D::RigidBody* activeBottom;
	Urho3D::RigidBody* touchFront;
	Urho3D::RigidBody* activeFront;
	Urho3D::RigidBody* touchBack;
	Urho3D::RigidBody* activeBack;
	Urho3D::RigidBody* touchLeft;
	Urho3D::RigidBody* activeLeft;
	Urho3D::RigidBody* touchRight;
	Urho3D::RigidBody* activeRight;

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
	float timeStartedWalking = 0;
	int currentWalkingSpeed = 0;
	int MAX_RUN_SPEED = 20;

	// Related to sliding
	float frictionCoefficentWalking = 50.0;
	float frictionCoefficentSliding = 0.3;

	// Related to falling
	/*
	double startingFallingSpeed = 50;
	double currentFallingSpeed = 0;
	double fallingAcceleration = 0.3;
	double MAX_FALLING_SPEED = 70;
	*/

	// Time between jump inputs being processed
	float timeBetweenJump = 0.66;
	bool canJump = true;
	// Meaning, time since last jump
	float currentJumpTime = 0;
	// Time between squat inputs being processed
	float timeBetweenSquat = 0.66;
	bool canSquat = true;
	// Meaning, time since last squat
	float currentSquatTime = 0;

	// Pertaining to inputs
	bool lastJumpInput = false;
	bool lastSquatInput = false;
	bool jumpInputted = false;
	bool squatInputted = false;

	/*
		Vector with all movesets laid out
		Each one recieves the inputs as they are inputted (accounting for delay so
			the move can't occur 20 times a second)
		All should be added with `new`
	*/
	std::vector<MoveBase*> moveset;
	// Current running move
	MoveBase* currentMove = nullptr;

public:
	playerPhysics(Urho3D::PhysicsWorld* pW) {
		world = pW;
		// Gravity handled only during falling
		player->SetUseGravity(false);
		// Prevent body from rotating unless we specifically allow it
		player->SetAngularFactor(Urho3D::Vector3(0, 0, 0));
		// Enable CCd for non passing through the ground
		player->SetCcdMotionThreshold(3);
		// Make friction very high
		player->SetFriction(frictionCoefficentWalking);

		// Set all touch and active bodies to trigger mode so they don't interact at all with the world
		touchTop->SetTrigger(true);
		activeTop->SetTrigger(true);
		touchBottom->SetTrigger(true);
		activeBottom->SetTrigger(true);
		touchFront->SetTrigger(true);
		activeFront->SetTrigger(true);
		touchBack->SetTrigger(true);
		activeBack->SetTrigger(true);
		touchLeft->SetTrigger(true);
		activeLeft->SetTrigger(true);
		touchRight->SetTrigger(true);
		activeRight->SetTrigger(true);
	}

	~playerPhysics() {
		// Clears memory of each move
		for (MoveBase* move : moveset) {
			// Delete each instance
			delete move;
		}
		moveset.clear();
	}

	void update(float delta) {
		// Delta is recieved in seconds
		if (currentState != FALLING) {

			if (currentState == SLIDING) {
				if (player->GetUseGravity() != true) {
					player->SetUseGravity(true);
				}
				if (player->GetFriction() != frictionCoefficentSliding) {
					player->SetFriction(frictionCoefficentSliding);
				}
			} else {
				// Gravity is not handled during these times
				if (player->GetUseGravity() != false) {
					player->SetUseGravity(false);
				}
				if (player->GetFriction() != frictionCoefficentWalking) {
					// Disable sliding
					player->SetFriction(frictionCoefficentWalking);
				}
			}
		} else {
			// Turn on gravity for a time
			if (!player->GetUseGravity()) {
				player->SetUseGravity(true);
			}
		}

		// Update current move
		if (currentMove) {
			// Current move exists
			currentMove->update();
		}

		// Add delta at beginning
		if (canJump == false) {
			currentJumpTime += delta;
		}
		if (canSquat == false) {
			currentSquatTime += delta;
		}

		// Pass inputs to each move
		// Account for delay
		if (currentJumpTime >= timeBetweenJump) {
			// Set now
			canJump = true;
		}
		if (currentSquatTime >= timeBetweenSquat) {
			// Set now
			canSquat = true;
		}

		// This bools handle if the inlut was first pressed on this frame
		bool jump = false;
		bool squat = false;

		if (inputSystem->isJumpPressed()) {
			// Wants to jump
			if (canJump == true) {
				jump = true;
				canJump = false;
			}
		}

		if (inputSystem->isSquatPressed()) {
			// Wants to jump
			if (canSquat == true) {
				squat = true;
				canSquat = false;
			}
		}

		if ((jump || squat) && !currentMove) {
			// Two moves shouldn't be able to run at the same time
			// Sure, makes glitches less fun, but it is smart
			int inputNum;
			// If both inputs were handled this frame, jump overrules
			// Pretty rare occurrence
			if (squat) {
				inputNum = SQUAT;
			}
			if (jump) {
				inputNum = JUMP;
			}

			// If angle is -1. no angle is being inputted
			int moving = inputSystem->getAngle() == -1 ? NO_MOVEMENT : YES_MOVEMENT;

			// Handle the input

			Urho3D::PODVector<Urho3D::RigidBody*> collidingBodies;

			// Determine wall collisions
			// Really messy, but concise
			activeFront->GetCollidingBodies(collidingBodies);
			bool frontWallTouching = collidingBodies.Size() != 0;
			collidingBodies.Clear();
			activeBack->GetCollidingBodies(collidingBodies);
			bool backWallTouching = collidingBodies.Size() != 0;
			collidingBodies.Clear();
			activeLeft->GetCollidingBodies(collidingBodies);
			bool leftWallTouching = collidingBodies.Size() != 0;
			collidingBodies.Clear();
			activeRight->GetCollidingBodies(collidingBodies);
			bool rightWallTouching = collidingBodies.Size() != 0;
			collidingBodies.Clear();

			// Just for ground
			touchBottom->GetCollidingBodies(collidingBodies);
			bool touchingGround = collidingBodies.Size() != 0;
			collidingBodies.Clear();

			// Touching a wall
			// Will pass to function at some point
			bool touchingWall = frontWallTouching || backWallTouching || leftWallTouching || rightWallTouching;

			int physicsType = AIR;
			if (touchingGround && touchingWall) {
				// Wall overrides
				physicsType = WALL;
			}

			if (touchingGround) {
				physicsType = GROUND;
			}

			if (touchingWall) {
				physicsType = WALL;
			}
			// Pass inputs to every move
			std::vector<MoveBase*> movesReady;
			for (MoveBase* move : moveset) {
				// Give inputs to each move
				move->addInput(physicsType, inputNum, moving);
				if (move->canPerform()) {
					// Insert to end
					movesReady.push_back(move);
				}
			}

			if (movesReady.size() != 0) {
				// Some moves can run
				if (movesReady.size() != 1) {
					// There are multiple moves that can run
					// The ones with the longest requirement list have presidence
					// clang-format off
					// Clang format can't do lambdas right
					std::sort(movesReady.begin(), movesReady.end(), [](MoveBase* a, MoveBase* b) {
						return a->getLength() > b->getLength();
					});
					// clang-format on
				}

				// Only the first move is actually performed
				currentMove = movesReady.front();
				// Don't delete instances
				movesReady.clear();
				// Start up move (will be updated)
				currentMove->performMove();
				currentMove->flushInputs();
			}
		}

		if (currentMove->isDone()) {
			// End current move
			currentMove->finish();
			// Dont delete instance, but empty the pointer for later use
			currentMove = nullptr;
		}
	}

	// Set the input system
	void setInputSystem(inputHandler* iS) {
		inputSystem = iS;
	}
};