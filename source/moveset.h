#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/Node.h>

#include <vector>

#include "controls.h";

enum MOVESET_CONSTANTS : int {
	GROUND = 0,
	AIR = 1,
	WALL = 2,
	SQUAT = 0,
	JUMP = 1,
	YES_MOVEMENT = 0,
	NO_MOVEMENT = 1,
};

class MoveBase {
protected:
	Urho3D::Node* playerNode;
	Urho3D::RigidBody* player;
	// Not the camera itself
	Urho3D::Node* camera;

	inputHandler* inputSystem;

	/*
		Meant to be overridden by the moves

		requirementLength: Number of requirements for the move
		physicsWorldMatching:
			0 = Ground
			1 = Air
			2 = Wall
			3 = Air or Wall
		inputMatching:
			0 = Squat
			1 = Jump
		movementMatching:
			0 = Yes
			1 = No
			2 = Yes or No
	*/

	int requirementLength;
	int physicsWorldMatching[];
	int inputMatching[];
	int movementMatching[];

	/*
		Array of int arrays:
			Each array has the latest input in it (delayed so the trick cant be performed 60 times a second)
			The array has the format of:
				physicsWorldMatching, inputMatching, movementMatching
			For example:
				[
					[0, 1, 0],
					[0, 1, 0],
					[0, 1, 0]
				]
			This set of inputs represents a triple jump because it is three jumps on the ground in succession (with movement as well)
			This array of strings has the same length as `requirementLength`

			How 2D string arrays work
			char strs[requirementLength][4];
			strcpy(strs[0], "010");
			The 4 is because it has to be one more than the string length

			One of the values has to be hardcoded

			This is meant to be overridden
	 */

	int stockpiledInputs[][3];

	// Set on start of move by performMove
	bool moveIsOccurring = false;

	Urho3D::Vector3 rotateForce(Urho3D::Vector3 force) {
		return playerNode->GetWorldRotation() * force;
	}

	// Just for helper to jump
	float timeSinceStart = 0;
	void helperHoldToJump(Urho3D::Vector3 velocityToAdd, float delta, float timeToEnd) {
		if (inputSystem->isJumpHeld()) {
			timeSinceStart += delta;
			if (timeSinceStart < timeToEnd) {
				// Only keep going if the maximum time to hold is not exceeded
				// Number of milliseconds
				float divisionFactor = (timeSinceStart * 1000);
				// Division factor shouldn't be less than 0, or weird things will happen
				// Shouldn't happen, though
				Urho3D::Vector3 forceToApply = (velocityToAdd * delta) / divisionFactor;
				// Should add force to the rigidbody
				player->ApplyImpulse(rotateForce(forceToApply));
			}
		} else {
			timeSinceStart = 0;
			// End move
			moveIsOccurring = false;
		}
	}

public:
	MoveBase(Urho3D::RigidBody* p, inputHandler* iH) {
		// Create player rigidbody
		player = p;
		inputSystem = iH;
	}

	virtual void init() = 0;

	void start() {
		moveIsOccurring = true;
	}

	// Preforms the move's action
	virtual void performMove() = 0;

	// Included so recieved inputs can be handled when recieved
	// For example, holding jump so a higher jump can be achieved
	virtual void update(float delta) = 0;

	// Included so the move can let the main program know when it is done performing
	// (Usually when the jump button is not held anymore)
	bool isDone() {
		return !moveIsOccurring;
	}

	// Let the move know that it has just ended
	// (Not permamently, but for this cycle)
	virtual void finish() = 0;

	int getLength() {
		return requirementLength;
	}

	void flushInputs() {
		// Removes all earlier inputs
		// Only the first has to be set because the script figures it out
		stockpiledInputs[0][0] = -1;
	}

	void addInput(int physics, int input, int movement) {
		// Shift inputs
		for (int i = 0; i < requirementLength; i++) {
			if (i != 0) {
				// Shift inputs down
				stockpiledInputs[i - 1][0] = stockpiledInputs[i][0];
				stockpiledInputs[i - 1][1] = stockpiledInputs[i][1];
				stockpiledInputs[i - 1][2] = stockpiledInputs[i][2];
			}
		}
		// Add input to end
		stockpiledInputs[requirementLength - 1][0] = physics;
		stockpiledInputs[requirementLength - 1][1] = input;
		stockpiledInputs[requirementLength - 1][2] = movement;
	}

	bool canPerform() {
		bool canDo = true;
		for (int i = 0; i < requirementLength; i++) {
			if (physicsWorldMatching[i] == 3) {
				// Air or Wall
				if (stockpiledInputs[i][0] == 0) {
					// Ground
					canDo = false;
				}
			} else {
				// Checking if equivilent
				// Also checks for -1
				// If -1, the input doesn't exist because they have been flushed
				if (stockpiledInputs[i][0] != physicsWorldMatching[i]) {
					canDo = false;
				}
			}

			if (stockpiledInputs[i][1] != inputMatching[i]) {
				// Checking if equivilent
				canDo = false;
			}

			if (stockpiledInputs[i][2] != movementMatching[i] && movementMatching[i] != 2) {
				// Check that it isn't Yes or No
				canDo = false;
			}
		}

		return canDo;
	}
};