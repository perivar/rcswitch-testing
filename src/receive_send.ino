/*
  Example for sending and receiving
  Modified library to support Nexa and Everflourish
  https://github.com/perivar/rc-switch
  
  https://z4ziggy.wordpress.com/2014/06/27/rf-sniffer-open-gates-cars-and-rf-remote-controlled-devices-with-ease/

  If you want to visualize a telegram copy the rawBits data and 
  paste it into http://test.sui.li/oszi/
*/

#include <Arduino.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

// pins for the 433 Mhz module
int rxPin = 2;
int txPin = 10;

unsigned long previousMillis = 0;
const long waitInterval = 15000;

void setup()
{
    // print fast to console
    Serial.begin(115200);

    // Receiver on interrupt 0 => that is pin #2
    mySwitch.enableReceive(digitalPinToInterrupt(rxPin));

    // Transmitter is connected to Arduino Pin #10
    mySwitch.enableTransmit(txPin);

    Serial.println("[+] Listening");
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
        unsigned int *rawTimings = mySwitch.getReceivedRawdata();
        for (unsigned int i = 0; i < mySwitch.getReceivedBitlength() * 2; i++)
        {
            Serial.print(rawTimings[i]);
            Serial.print(",");
        }
        Serial.println();

        Serial.print("Raw bitdata: ");
        char *rawBits = mySwitch.getReceivedRawBits();
        for (unsigned int i = 0; i < mySwitch.getReceivedBitlength(); i++)
        {
            Serial.print(rawBits[i]);
        }
        Serial.println();

        mySwitch.resetAvailable();
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > waitInterval)
    {
        // save the last time you entered this routine
        previousMillis = currentMillis;

        // 8 = Nexa
        // 9 = Everflourish
        mySwitch.setProtocol(8);
        // Nexa repeats 10 times.
        mySwitch.setRepeatTransmit(10);
        const char *nexaCode = "1001100101101010100101101010011001011001100110100110010110101010";
        Serial.print("Sending Nexa: ");
        Serial.print(strlen(nexaCode));
        Serial.print(" bits.");
        Serial.println();
        mySwitch.send(nexaCode);

        delay(1000);

        // 8 = Nexa
        // 9 = Everflourish
        mySwitch.setProtocol(9);
        // Everflourish repeates only 4 times.
        mySwitch.setRepeatTransmit(4);
        const char *everflourish4On = "0000011010100110100101100110010110101010100110101010";
        Serial.print("Sending 4 ON: ");
        Serial.print(strlen(everflourish4On));
        Serial.print(" bits.");
        Serial.println();
        mySwitch.send(everflourish4On);

        delay(2000);

        const char *everflourish4Off = "0000011010100110100101100110010110101010100101010101";
        Serial.print("Sending 4 OFF: ");
        Serial.print(strlen(everflourish4Off));
        Serial.print(" bits.");
        Serial.println();
        mySwitch.send(everflourish4Off);
    }
}
