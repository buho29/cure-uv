// 
// 
// 

#include "Esp32Stepper.h"

const int StepperEsp::stepsLookup[8] = { B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001 };

void IRAM_ATTR StepperEsp::setOutput(int step)
{
	digitalWrite(_pin1, bitRead(stepsLookup[step], 0));
	digitalWrite(_pin2, bitRead(stepsLookup[step], 1));
	digitalWrite(_pin3, bitRead(stepsLookup[step], 2));
	digitalWrite(_pin4, bitRead(stepsLookup[step], 3));
}


StepperEsp::StepperEsp(uint16_t motorSteps, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4)
{
	_motorSteps = motorSteps;
	_pin1 = pin1;
	_pin2 = pin2;
	_pin3 = pin3;
	_pin4 = pin4;

	pinMode(pin1, OUTPUT);
	pinMode(pin2, OUTPUT);
	pinMode(pin3, OUTPUT);
	pinMode(pin4, OUTPUT);
	
	timerMux = portMUX_INITIALIZER_UNLOCKED;

}

void StepperEsp::Rotate(float angleDelta)
{
	Rotate(angleDelta,nullptr);
}

void StepperEsp::Rotate(float angleDelta, StepCallback callback)
{
	portENTER_CRITICAL(&timerMux);
	_stopped = false;
	_direction = (angleDelta>0) ? 1 : -1;
	angleDelta *= _direction;
	_goalStep = angleDelta / 360.f * _motorSteps;
	_callback = callback;
	_startime = micros();
	portEXIT_CRITICAL(&timerMux);
}

void StepperEsp::RotateContinuos(int8_t direction)
{
	portENTER_CRITICAL(&timerMux);
	_stopped = false;
	_direction = direction;
	_goalStep = 0;
	_startime = micros();
	portEXIT_CRITICAL(&timerMux);
}

void StepperEsp::SetSpeedRpm(float rpm)
{
	SetSpeedDegreesBySecond(rpm * 360 / 60);
}

void StepperEsp::SetSpeedDegreesBySecond(float degreesBySecond)
{
	float degreeByStep = 360.0f / _motorSteps;

	portENTER_CRITICAL(&timerMux);
	microsByStep = degreeByStep / degreesBySecond * 1000000;
	portEXIT_CRITICAL(&timerMux);
	
}

void StepperEsp::SetSpeedRadiansBySecong(float radiansBySecond)
{
	SetSpeedDegreesBySecond(360.0f / 2 / PI * radiansBySecond);
}

void StepperEsp::SetMetersBySecond(float metersBySecond, float radius)
{
	SetSpeedRadiansBySecong(metersBySecond / radius);
}

void StepperEsp::Stop()
{

	portENTER_CRITICAL(&timerMux);
	_stopped = true;
	portEXIT_CRITICAL(&timerMux);
}

float StepperEsp::GetCurrentAngle() const
{
	return 360.0f * _currentStep / _motorSteps;
}

bool IRAM_ATTR StepperEsp::Update()
{
	portENTER_CRITICAL(&timerMux);
	if (_stopped) {

		portEXIT_CRITICAL(&timerMux);
		return false;
	}
	if (static_cast<unsigned long>(micros() - _startime) >= microsByStep)
	{
		_startime += microsByStep;
		
		if (_direction > 0) {
			//StepCW();
			stepCounter++;
			if (stepCounter >= numSteps) stepCounter = 0;
			setOutput(stepCounter);
			_currentStep++;
			if (_currentStep > _motorSteps) _currentStep = 0;
		}
		else {
			//StepCCW();
			stepCounter--;
			if (stepCounter < 0) stepCounter = numSteps - 1;
			setOutput(stepCounter);
			//_currentStep--;
			//if (_currentStep > _motorSteps) _currentStep = _motorSteps;
			_currentStep = _currentStep == 0 ? _motorSteps : _currentStep - 1;
		}

		if (_goalStep > 0)
		{
			_goalStep--;
			if (_goalStep == 0)
			{
				_stopped = true;
				if (_callback != nullptr) _callback();
			}
		}
		portEXIT_CRITICAL(&timerMux);
		return true;
	}
	portEXIT_CRITICAL(&timerMux);
	return false;
}

void IRAM_ATTR Steppers::tick_handler(void)
{
	if (_steppers != nullptr)
	{
		_steppers->Update();
	}
}

StepperEsp * Steppers::_steppers = nullptr;

void Steppers::init()
{
	timer = timerBegin(0, 80, true);
	// Attach onTimer function to our timer.
	timerAttachInterrupt(timer, &tick_handler, true);
	// Set alarm to call onTimer function every second (value in microseconds).
	// Repeat the alarm (third parameter)
	timerAlarmWrite(timer, 1000, true);
	// Start an alarm
	timerAlarmEnable(timer);
}

void Steppers::Add(StepperEsp * stepper)
{
	_steppers = stepper;
}

Steppers steppers;
