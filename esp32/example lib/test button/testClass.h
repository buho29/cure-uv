// testClass.h

#ifndef _TESTCLASS_h
#define _TESTCLASS_h

#include "arduino.h"
#include "AnalogButtons.h"

#define ANALOG_PIN 36

enum Buttons {
	btUp, btCenter, btDown, btLeft, btRigth
};

class testClass:public EventClick
{
 public:
	 testClass() {};
	void setup();
	void onClick(uint8_t id, bool isHeldDown);
};

//
#endif

