// SampleClass.h

#ifndef _SAMPLECLASS_h
#define _SAMPLECLASS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Task.h"

class SampleClass
{
 private:
	 uint8_t seconds;
	 uint8_t minutes;
	 uint8_t hours;
	 Task * task;
	void onSetInterval(Task * t);
 public:
	void setup(uint16_t s);
	void setup(uint16_t h, uint16_t m, uint16_t s);
};

#endif

