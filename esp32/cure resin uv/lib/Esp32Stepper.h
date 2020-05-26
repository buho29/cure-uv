// Esp32Stepper.h

/*

based in https://github.com/luisllamasbinaburo/Arduino-AsyncStepper 

it uses interrupts (timer) to control the steppers in esp32

this library is not full tested
multiple stepper was not implemented

buho29

example:

#include "Esp32Stepper.h"t int motorPin1 = 14;
const int motorPin2 = 12;
const int motorPin3 = 13;
const int motorPin4 = 21;


const int stepsPerRevolution = 4076;


StepperEsp stepper1(stepsPerRevolution, motorPin1, motorPin2, motorPin3, motorPin4);


void rotateCW()
{
	Serial.println("yea 1");
	stepper1.Rotate(90 * 3, rotateCCW);
}

void rotateCCW()
{
	Serial.println("yea 2");
	stepper1.Rotate(-90 * 3, rotateCW);
}

void setup()
{
	Serial.begin(115200);


	stepper1.SetSpeedRpm(15);
	stepper1.RotateContinuos(StepperEsp::cw);
	//rotateCW();

	steppers.Add(&stepper1);
	steppers.init();

}

void loop()
{
}

*/

#ifndef _ESP32STEPPER_h
#define _ESP32STEPPER_h

#include "arduino.h"

typedef void(*StepCallback)();

class StepperEsp
{

private:

	static const int numSteps = 8;
	static const int stepsLookup[8];

	portMUX_TYPE timerMux ;

	uint8_t _pin1;
	uint8_t _pin2;
	uint8_t _pin3;
	uint8_t _pin4;

	int stepCounter;
	uint16_t _motorSteps;
	unsigned int _currentStep;

	volatile unsigned long microsByStep;
	volatile int8_t _direction = 1;
	volatile unsigned int _goalStep;
	volatile bool _stopped = true;
	volatile unsigned long _startime;

	StepCallback _callback;

	void IRAM_ATTR setOutput(int step);

public:

	static const int8_t cw = 1;
	static const int8_t ccw = -1;

	StepperEsp(uint16_t motorSteps,
		uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4);

	void Rotate(float angleDelta);
	void Rotate(float angleDelta,StepCallback callback);
//	void RotateToAngle(float angle, StepCallback callback);
//	void RotateToAngle(float angle);
	void RotateContinuos(int8_t direction);

	void SetSpeedRpm(float rpm);
	void SetSpeedDegreesBySecond(float degreesBySecond);
	void SetSpeedRadiansBySecong(float radiansBySecond);
	void SetMetersBySecond(float metersBySecond, float radius);

	void Stop();
	float GetCurrentAngle() const;

	bool IRAM_ATTR Update();

};
class Steppers {
private:
	hw_timer_t * timer;
	//static const uint8_t MAXSIZE = 10;
	static StepperEsp * _steppers;//[MAXSIZE];
	//static uint8_t _stepperCount;

	static void IRAM_ATTR tick_handler(void);
public:
	void init();
	void Add(StepperEsp * stepper);
};

extern Steppers steppers;
#endif

