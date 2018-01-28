/*
  Example for receiving
  Modified library to support Nexa and Everflourish

  https://github.com/perivar/rc-switch
  
  If you want to visualize a telegram copy the rawBits data and 
  paste it into http://test.sui.li/oszi/
*/

#include <Arduino.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup()
{
    Serial.begin(9600);
    mySwitch.enableReceive(0); // Receiver on interrupt 0 => that is pin #2
}

void loop()
{
    if (mySwitch.available())
    {
        Serial.print("Received ");
        Serial.print(mySwitch.getReceivedValue());
        Serial.print(" / ");
        Serial.print(mySwitch.getReceivedBitlength());
        Serial.print("bit ");
        Serial.print("Protocol: ");
        Serial.println(mySwitch.getReceivedProtocol());

        Serial.print("Raw timing data: ");
        // use http://test.sui.li/oszi/ to visualise
        unsigned int *rawTimings = mySwitch.getReceivedRawdata();
        for (unsigned int i = 0; i < mySwitch.getReceivedBitlength() * 2; i++)
        {
            Serial.print(rawTimings[i]);
            Serial.print(",");
        }
        Serial.println();

        Serial.print("Raw bitdata: ");
        char *rawBits = mySwitch.getReceivedRawBits();
        for (unsigned int i = 0; i < mySwitch.getReceivedBitlength() - 1; i++)
        {
            Serial.print(rawBits[i]);
        }
        Serial.println();

        mySwitch.resetAvailable();
    }
}
