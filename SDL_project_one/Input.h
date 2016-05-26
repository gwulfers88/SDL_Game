#ifndef INPUT_H
#define INPUT_H

#include "common.h"
#include "vec2.h"

#define MAX_ENTITIES 4

namespace Input
{
	//Button for controller.
	struct Button
	{
		bool isDown;
	};

	//Mouse struct
	struct MouseInput
	{
		Button leftButton;
		Button rightButton;
		Button middleButton;
		vec2 pos;
	};

	// Controller for input processing.
	struct Controller
	{
		MouseInput mouse;

		Button numOne;
		Button numTwo;

		Button start;
		Button back;
		Button rightBumper;
		Button leftBumper;

		Button actionUp;
		Button actionDown;
		Button actionRight;
		Button actionLeft;
	};
};

#endif