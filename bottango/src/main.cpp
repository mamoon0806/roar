// !!! DRIVER VERSION: 0.7.0a1 !!!
// !!! Api Version: 8 !!!
#include <Arduino.h>

#include "BottangoCore.h"
#include "BasicCommands.h"

void setup()
{
  BottangoCore::bottangoSetup();
}

void loop()
{
  BottangoCore::bottangoLoop();
}
