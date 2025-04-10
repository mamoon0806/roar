#include <Arduino.h>

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.print("Enter AT Commands\r\n");
    Serial2.begin(9600);
}

void loop()
{
    // put your main code here, to run repeatedly:
    if (Serial2.available())
    {
        Serial.write(Serial2.read());
    }
    if (Serial.available())
    {
        Serial2.write(Serial.read());
    }
}