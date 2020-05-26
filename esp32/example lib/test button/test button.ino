
//#define __AVR__

#include "AnalogButtons.h"
#include "testClass.h"
#include "Arduino.h"



testClass test;


void setup() {
	Serial.begin(115200);

	test.setup();
}

void loop() {
	// To check values when button are pressed
	inputButtons.check();

	// To configure the MAX/Min values for each button: 
	// Then in turn hold down each button, noting down the max/min values

	//configure();
}



void configure() {
	unsigned int value = analogRead(ANALOG_PIN);
	Serial.println(value);
	delay(50);
}