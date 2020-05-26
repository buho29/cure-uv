// 
// simple test non-static member
// 
#include "testClass.h"

void testClass::onClick(uint8_t id, bool isHeldDown) {

	Buttons b = (Buttons)id;

	char * str;

	switch (b)
	{
	case btUp:
		str = "up";
		break;
	case btCenter:
		str = "center";
		break;
	case btDown:
		str = "down";
		break;
	case btLeft:
		str = "left";
		break;
	case btRigth:
		str = "rigth";
		break;
	}

	Serial.printf("button %s clicked global, isHeldDown %s\n", str, isHeldDown ? "true" : "false");
}

/*
0 = 3278 = up
1 = 2745 = center
2 = 2372 = down
3 = 2095 = left
4 = 1870 = rigth
*/
void testClass::setup()
{
	inputButtons.setup(ANALOG_PIN, INPUT);

	AnalogButton b = AnalogButton(btUp, 3278); // id,value
	inputButtons.addButton(b);

	b = AnalogButton(btCenter, 2745);
	inputButtons.addButton(b);

	b = AnalogButton(btDown, 2372);
	inputButtons.addButton(b);

	b = AnalogButton(btLeft, 2095);
	inputButtons.addButton(b);

	b = AnalogButton(btRigth, 1870);
	inputButtons.addButton(b);

	inputButtons.addEvent(this);
}

