#include <Urho3D/Input/Input.h>
#include <cmath>

class inputHandler {
private:
	bool jumpHeld = false;
	bool powerHeld = false;
	bool squatHeld = false;
	// -1 means no directional input
	int angleFacing = -1;
	// angle facing when interpolitated
	int interAngleFacing = -1;
	// speed when interpolitating
	int turnSpeed = 1;
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
	int squatKeyboard = Urho3D::KEY_SHIFT;

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

	int getInterpolitatedMovement() {
		// angles are only handled in integers
		// gets shortest angle
		int angle = 180 - abs(abs(angleFacing - interAngleFacing) - 180);
		// true clockwize, false not clockwize
		int delta = 360 - interAngleFacing;
		int beta = angleFacing + delta;
		beta = beta % 360;
		bool direction = beta < 180;
		if (direction) {
			// turn clockwise
			interAngleFacing = (interAngleFacing + turnSpeed) % 360;
		} else {
			// turn counter clockwise
			int temp = (interAngleFacing - turnSpeed);
			interAngleFacing = (temp < 0) ? 360 + temp : temp;
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

	int getAngle() {
		return angleFacing;
	}

	int getInterAngle() {
		return interAngleFacing;
	}

	void setAngleType(bool aT) {
		angleType = aT;
	}

	bool getAngleType() {
		return angleType;
	}

	void handleInput() {
		// runs every frame
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
		if (angleType) {
			// interpolitate it all
			if (interAngleFacing == -1) {
				// There is no angle
				interAngleFacing = angleFacing;
			} else if (angleFacing = -1) {
				// No movement is needed
				interAngleFacing = -1;
			} else if (angleFacing != interAngleFacing) {
				// interpolitate the directions
				getInterpolitatedMovement();
			}
		}
	}
};