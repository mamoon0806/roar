#include "FeedbackLinearActuatorEffector.h"
#include "Log.h"
#include "BottangoArduinoModules.h"

#define FEEDBACK_PROPORTIONAL_CONSTANT 4

FeedbackLinearActuatorEffector::FeedbackLinearActuatorEffector(byte inputPin, byte outputPin, short minPosition, short maxPosition, int maxPositionSec, short startPosition): LoopDrivenEffector(minPosition, maxPosition, maxPositionSec, startPosition)
{
    this->inputPin = inputPin;
    this->outputPin = outputPin;

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
    //TODO Drive

    LoopDrivenEffector::driveOnLoop();
    AbstractEffector::callbackOnDriveComplete(currentSignal, didChange);
}

void FeedbackLinearActuatorEffector::getIdentifier(char *outArray, short arraySize)
{
    snprintf(outArray, arraySize, "lin%02d%02d", (int)inputPin, (int)outputPin);
}

void FeedbackLinearActuatorEffector::destroy(bool systemShutdown)
{
    AbstractEffector::destroy(systemShutdown);
}