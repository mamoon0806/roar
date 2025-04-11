#include "FeedbackLinearActuatorEffector.h"
#include "Log.h"
#include "BottangoArduinoModules.h"

#define FEEDBACK_PROPORTIONAL_CONSTANT 4

FeedbackLinearActuatorEffector::FeedbackLinearActuatorEffector(byte inputPin, byte outAPin, byte outBPin, short minPosition, short maxPosition, int maxPositionSec, short startPosition) : LoopDrivenEffector(minPosition, maxPosition, maxPositionSec, startPosition)
{
    this->inputPin = inputPin;
    this->outAPin = outAPin;
    this->outBPin = outBPin;

    pinMode(inputPin, INPUT);
    pinMode(outAPin, OUTPUT);
    pinMode(outBPin, OUTPUT);

    Callbacks::onEffectorRegistered(this);
}

void FeedbackLinearActuatorEffector::driveOnLoop()
{
    bool didChange = false;
    if (currentSignal != targetSignal)
    {
        currentSignal = targetSignal;
        didChange = true;
    }

    int encoderPosition = analogRead(inputPin);
    int delta = currentSignal - encoderPosition;
    int output = delta * FEEDBACK_PROPORTIONAL_CONSTANT;
    int pwmSignal = abs(output);
    if (pwmSignal > 255)
    {
        pwmSignal = 255;
    }
    analogWrite(13, pwmSignal);
    if (output < 0)
    {
        digitalWrite(this->outAPin, LOW);
        analogWrite(this->outBPin, pwmSignal);
    }
    else if (output > 0)
    {
        digitalWrite(this->outBPin, LOW);
        analogWrite(this->outAPin, pwmSignal);
    }
    else
    {
        digitalWrite(this->outAPin, LOW);
        digitalWrite(this->outBPin, LOW);
    }

    LoopDrivenEffector::driveOnLoop();
    AbstractEffector::callbackOnDriveComplete(currentSignal, didChange);
}

void FeedbackLinearActuatorEffector::getIdentifier(char *outArray, short arraySize)
{
    snprintf(outArray, arraySize, "lin%0d", (int)inputPin);
}

void FeedbackLinearActuatorEffector::destroy(bool systemShutdown)
{
    AbstractEffector::destroy(systemShutdown);
}