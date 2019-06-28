#include <Urho3D/Input/Input.h>
#include <cmath>

class inputHandler {
private:
	bool jumpHeld = false;
	// Not yet used
	bool powerHeld = false;
	bool squatHeld = false;

	bool lastJumpInput = false;
	bool lastPowerInput = false;
	bool lastSquatInput = false;

	bool jumpFirstFrame = false;
	bool powerFirstFrame = false;
	bool squatFirstFrame = false;

	// -1 means no directional input
	int angleFacing = -1;
	// angle facing when interpolitated
	int interAngleFacing = -1;
	// speed when interpolitating
	int turnSpeed = 3;
	/*
		Inter or not
		true: inter
		false: non-inter
	 */
	bool angleType = true;
	/*
		Control types:
		0: Keyboard
		1: Gamepad
	*/
	int controlType = 0;
	int playerNum;

	int upKeyboard = Urho3D::KEY_W;
	int downKeyboard = Urho3D::KEY_S;
	int leftKeyboard = Urho3D::KEY_A;
	int rightKeyboard = Urho3D::KEY_D;
	int jumpKeyboard = Urho3D::KEY_SPACE;
	int powerKeyboard = Urho3D::KEY_I;
	int squatKeyboard = Urho3D::KEY_U;

	int gamepadJoystick = Urho3D::CONTROLLER_BUTTON_LEFTSTICK;
	/*
	Joystick mapping
	When one changes, all change
	Different because all of these are for the same control
		0
	  3   1
		2
	*/
	int upGamepad = 0;
	int downGamepad = 2;
	int leftGamepad = 3;
	int rightGamepad = 1;
	int jumpGamepad = Urho3D::CONTROLLER_BUTTON_A;
	int powerGamepad = Urho3D::CONTROLLER_BUTTON_RIGHTSHOULDER;
	int squatGamepad = Urho3D::CONTROLLER_BUTTON_LEFTSHOULDER;

	Urho3D::Input* inputSubsystem;

	void getInterpolitatedMovement() {
		// angles are only handled in integers
		// gets shortest angle
		int angle = 180 - abs(abs(angleFacing - interAngleFacing) - 180);
		// true clockwize, false not clockwize
		int delta = 360 - interAngleFacing;
		int beta = angleFacing + delta;
		beta = beta % 360;
		bool direction = beta < 180;
		// For overshooting calculations
		int overshoot = angleFacing - interAngleFacing;
		// Determines if overshoot was clockwise, use negation for CC
		bool didOvershootClockwise = overshoot < 180 || overshoot > -180;
		if (direction) {
			// turn clockwise
			interAngleFacing = (interAngleFacing + turnSpeed) % 360;
			if (didOvershootClockwise) {
				// Overshot, correct movement
				interAngleFacing = angleFacing;
			}
		} else {
			// turn counter clockwise
			int temp = interAngleFacing - turnSpeed;
			interAngleFacing = (temp < 0) ? 360 + temp : temp;
			if (!didOvershootClockwise) {
				interAngleFacing = angleFacing;
			}
		}
	}

public:
	inputHandler(int pN, Urho3D::Input* iS) {
		// pass GetSubsystem<Input>()
		playerNum = pN;
		inputSubsystem = iS;
	};

	void setTurnSpeed(int speed) {
		turnSpeed = speed;
	}

	int getTurnSpeed() {
		return turnSpeed;
	}

	int getRawAngle() {
		// For gamepads, this is the wanted angle most of the time
		return angleFacing;
	}

	int getInterAngle() {
		return interAngleFacing;
	}

	// Get angle based on angleType
	int getAngle() {
		if (angleType) {
			return interAngleFacing;
		} else {
			return angleFacing;
		}
	}

	void setAngleType(bool aT) {
		angleType = aT;
	}

	bool getAngleType() {
		return angleType;
	}

	bool isJumpHeld() {
		return jumpHeld;
	}

	bool isPowerHeld() {
		return powerHeld;
	}

	bool isSquatHeld() {
		return squatHeld;
	}

	bool isJumpPressed() {
		return jumpFirstFrame;
	}

	bool isPowerPressed() {
		return powerFirstFrame;
	}

	bool isSquatPressed() {
		return squatFirstFrame;
	}

	void handleInput() {
		// runs every frame

		lastJumpInput = jumpHeld;
		lastPowerInput = powerHeld;
		lastSquatInput = squatHeld;

		if (controlType == 0) {
			// keyboard
			jumpHeld = inputSubsystem->GetKeyDown(jumpKeyboard);
			// refers to things like fire flower
			powerHeld = inputSubsystem->GetKeyDown(powerKeyboard);
			squatHeld = inputSubsystem->GetKeyDown(squatKeyboard);

			// for direction
			bool upHeld = inputSubsystem->GetKeyDown(upKeyboard);
			bool downHeld = inputSubsystem->GetKeyDown(downKeyboard);
			bool leftHeld = inputSubsystem->GetKeyDown(leftKeyboard);
			bool rightHeld = inputSubsystem->GetKeyDown(rightKeyboard);

			/*
					 0
			   315       45

			  270   DIR   90

			   225       135
					180
			*/

			// handle directions
			if (upHeld && !downHeld) {
				if (leftHeld && !rightHeld) {
					// up and left
					angleFacing = 315;
				} else if (rightHeld && !leftHeld) {
					// up and right
					angleFacing = 45;
				} else if (!leftHeld && !rightHeld) {
					// up
					angleFacing = 0;
				}
			} else if (downHeld && !upHeld) {
				if (leftHeld && !rightHeld) {
					// down and left
					angleFacing = 225;
				} else if (rightHeld && !leftHeld) {
					// down and right
					angleFacing = 135;
				} else if (!leftHeld && !rightHeld) {
					// down
					angleFacing = 180;
				}
			} else if (!upHeld && !downHeld) {
				if (leftHeld && !rightHeld) {
					// left
					angleFacing = 270;
				} else if (rightHeld && !leftHeld) {
					// right
					angleFacing = 90;
				} else if (!leftHeld && !rightHeld) {
					// NOTHING
					angleFacing = -1;
				}
			}
		}

		// Handle first frame inputs
		// As in, stand as an onPressed handler
		if (lastJumpInput == false && jumpHeld == true) {
			jumpFirstFrame = true;
		} else {
			jumpFirstFrame = false;
		}
		if (lastPowerInput == false && powerHeld == true) {
			powerFirstFrame = true;
		} else {
			powerFirstFrame = false;
		}
		if (lastSquatInput == false && squatHeld == true) {
			squatFirstFrame = true;
		} else {
			squatFirstFrame = false;
		}

		if (angleType) {
			// interpolitate it all
			if (interAngleFacing == -1) {
				// There is no angle
				interAngleFacing = angleFacing;
			} else if (angleFacing == -1) {
				// No movement is needed
				interAngleFacing == -1;
			} else if (angleFacing != interAngleFacing) {
				// interpolitate the directions
				getInterpolitatedMovement();
			}
		}
	}
};