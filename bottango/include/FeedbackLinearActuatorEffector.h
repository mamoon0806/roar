#ifndef FeedbackLinearActuatorEffector_h
#define FeedbackLinearActuatorEffector_h

#include "LoopDrivenEffector.h"
#include "Arduino.h"

class FeedbackLinearActuatorEffector : public LoopDrivenEffector
{
public:
    FeedbackLinearActuatorEffector(byte inputPin, byte outputPin, short minPosition, short maxPosition, int maxPositionSec, short startPosition);
    virtual void driveOnLoop() override;

    virtual void getIdentifier(char *outArray, short arraySize) override;
    virtual void destroy(bool systemShutdown) override;

protected:
private:
    byte outputPin = 0;
    byte inputPin = 0;
};

#endif