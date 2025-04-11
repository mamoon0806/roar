
#include "EffectorPool.h"
#include "Log.h"
#include "Errors.h"
#include "FreeRam.h"
#include "StepDirStepperEffector.h"

EffectorPool::EffectorPool()
{
    pinMode(STEPPER_DIR_PIN, OUTPUT);
}

void EffectorPool::addEffector(AbstractEffector *inEffector)
{
    if (effectors.size() >= MAX_REGISTERED_EFFECTORS)
    {
        Error::reportError_NoSpaceAvailable();
        return;
    }

    char inEffectorIdentifier[9];
    inEffector->getIdentifier(inEffectorIdentifier, 9);

    AbstractEffector *existingEffector = getEffector(inEffectorIdentifier);

    if (existingEffector == NULL)
    {
        LOG_MKBUF;
        LOG(F("Add effector "))
        LOG_LN(inEffectorIdentifier);

        effectors.pushBack(inEffector);
    }
    else
    {
        Error::reportError_ServoCollision();
        return;
    }
}

void EffectorPool::removeEffector(char *identifier)
{
    LOG_LN(F("remove Effector"))

#ifdef __AVR__
    cli(); // stop interrupts
#endif

    AbstractEffector *effector = getEffector(identifier);
    if (effector == NULL)
    {
        Error::reportError_NoServoOnPin();
        return;
    }

    effectors.remove(effector);
    effector->destroy(false);
    delete effector;

#ifdef __AVR__
    sei(); // allow interrupts
#endif
}

void EffectorPool::addCurveToEffector(char *identifier, Curve *curve)
{
    AbstractEffector *effector = getEffector(identifier);
    if (effector == NULL)
    {
        Error::reportError_NoServoOnPin();
        return;
    }
    effector->addCurve(curve);
}

void EffectorPool::updateEffectorSignalBounds(char *identifier, int minSignal, int maxSignal, int signalSpeed)
{
    AbstractEffector *effector = getEffector(identifier);
    if (effector == NULL)
    {
        Error::reportError_NoServoOnPin();
        return;
    }
    effector->updateSignalBounds(minSignal, maxSignal, signalSpeed);
}

void EffectorPool::syncEffector(char *identifier, int syncValue)
{
    AbstractEffector *effector = getEffector(identifier);
    if (effector == NULL)
    {
        Error::reportError_NoServoOnPin();
        return;
    }
    effector->setSync(syncValue);
}

void EffectorPool::clearCurvesForEffector(char *identifier)
{
    AbstractEffector *effector = getEffector(identifier);
    if (effector == NULL)
    {
        Error::reportError_NoServoOnPin();
        return;
    }
    effector->clearCurves();
}

bool EffectorPool::effectorUsesFloatCurve(char *identifier)
{
    AbstractEffector *effector = getEffector(identifier);
    if (effector == NULL)
    {
        Error::reportError_NoServoOnPin();
        return true;
    }
    return effector->useFloatCurve();
}

void EffectorPool::updateAllDriveTargets()
{
    // TODO Maybe make stepper dir sharing logic more optimized?
    for (byte i = 0; i < effectors.size(); i++)
    {
        effectors.get(i)->updateOnLoop();

        if (effectors.get(i)->effectorType() != EFFECTORTYPE_STEPDIR)
        {
            effectors.get(i)->driveOnLoop();
        }
    }

    // Do all HIGH DIRs first
    digitalWrite(STEPPER_DIR_PIN, HIGH);
    delayMicroseconds(1);
    for (byte i = 0; i < effectors.size(); i++)
    {
        if (effectors.get(i)->effectorType() == EFFECTORTYPE_STEPDIR)
        {
            StepDirStepperEffector *stepEffector = dynamic_cast<StepDirStepperEffector*>(effectors.get(i));
            if ((stepEffector->getDriveValue() > 0 && !stepEffector->clockwiseIsLow) || (stepEffector->getDriveValue() < 0 && stepEffector->clockwiseIsLow))
            {
                effectors.get(i)->driveOnLoop();
            }
        }
    }
    delayMicroseconds(1);

    // Then do all LOW DIRs
    digitalWrite(STEPPER_DIR_PIN, LOW);
    delayMicroseconds(1);
    for (byte i = 0; i < effectors.size(); i++)
    {
        if (effectors.get(i)->effectorType() == EFFECTORTYPE_STEPDIR)
        {
            StepDirStepperEffector *stepEffector = dynamic_cast<StepDirStepperEffector*>(effectors.get(i));
            if ((stepEffector->getDriveValue() > 0 && stepEffector->clockwiseIsLow) || (stepEffector->getDriveValue() < 0 && !stepEffector->clockwiseIsLow))
            {
                effectors.get(i)->driveOnLoop();
            }
        }
    }
}

void EffectorPool::deregisterAll()
{
    LOG_LN(F("deregister All"))

#ifdef __AVR__
    cli(); // stop interrupts
#endif

    for (int i = 0; i < effectors.size(); i++)
    {
        AbstractEffector *effector = effectors.get(i);
        effector->destroy(true);
        delete effectors.get(i);
    }
    effectors.clear();

#ifdef __AVR__
    sei(); // allow interrupts
#endif
}

void EffectorPool::clearAllCurves()
{
    for (byte i = 0; i < effectors.size(); i++)
    {
        effectors.get(i)->clearCurves();
    }
}

AbstractEffector *EffectorPool::getEffector(char *identifier)
{
    for (byte i = 0; i < effectors.size(); i++)
    {
        if (effectors.get(i)->respondsToIdentifier(identifier))
        {
            return effectors.get(i);
        }
    }
    return NULL;
}