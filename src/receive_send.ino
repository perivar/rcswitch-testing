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

void setup()
{
    // print fast to console
    Serial.begin(115200);

    // Receiver on interrupt 0 => that is pin #2
    mySwitch.enableReceive(digitalPinToInterrupt(rxPin));

    // Transmitter is connected to Arduino Pin #10
    mySwitch.enableTransmit(txPin);

    // Optional set protocol (default is 1, will work for most outlets)
    // 8 = Nexa
    // 9 = Everflourish
    mySwitch.setProtocol(8);

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
        for (unsigned int i = 0; i < mySwitch.getReceivedBitlength() - 1; i++)
        {
            Serial.print(rawBits[i]);
        }
        Serial.println();

        mySwitch.resetAvailable();
    }

    // Nexa repeats 10 times.
    // Everflourish repeates only 2 times.

    // split binary string into longs
    // disable repeats and send one long after eachother
    mySwitch.setRepeatTransmit(1);

    //const char *nexaCode = "1001100101101010100101101010011001011001100110100110010110101010";
    //send(nexaCode, 10);
    //delay(5000);
} 

void sendChunk(unsigned long code, unsigned int length, RCSwitch::Protocol protocol)
{
    Serial.print(RCSwitch::dec2binWzerofill(code, length));
    for (int i = length - 1; i >= 0; i--)
    {
        if (code & (1L << i))
            mySwitch.transmit(protocol.one);
        else
            mySwitch.transmit(protocol.zero);
    }
}

void send(const char *sCodeWord, unsigned int repeatCount)
{
    const unsigned int chunkBitSize = sizeof(unsigned long) * 8; // 32 bits
    unsigned long code = 0;
    unsigned int length = 0;
    RCSwitch::Protocol protocol = mySwitch.getProtocol();

    Serial.print("Sending ");
    Serial.print(strlen(sCodeWord));
    Serial.print(" bits.");
    Serial.println();

#if not defined(RCSwitchDisableReceiving)
    // make sure the receiver is disabled while we transmit
    int nReceiverInterrupt_backup = mySwitch.getReceiverInterrupt();
    if (nReceiverInterrupt_backup != -1)
    {
        mySwitch.disableReceive();
    }
#endif
    // Disable transmit after sending (i.e., for inverted protocols)
    digitalWrite(txPin, LOW);

    for (int nRepeat = 0; nRepeat < repeatCount; nRepeat++)
    {
        for (const char *p = sCodeWord; *p; p++)
        {
            if (length < chunkBitSize)
            {
                code <<= 1L;
                if (*p != '0')
                {
                    code |= 1L;
                }
                length++;
            }
            else
            {
                // transmit bits
                sendChunk(code, length, protocol);

                // reset and add last bit
                length = 0;
                code = 0;
                if (*p != '0')
                {
                    code |= 1L;
                }
                length++;
            }
        }
        // transmit bits
        sendChunk(code, length, protocol);

        // transmit sync at the end
        mySwitch.transmit(protocol.pause);
    }

    // Disable transmit after sending (i.e., for inverted protocols)
    digitalWrite(txPin, LOW);

#if not defined(RCSwitchDisableReceiving)
    // enable receiver again if we just disabled it
    if (nReceiverInterrupt_backup != -1)
    {
        mySwitch.enableReceive(nReceiverInterrupt_backup);
    }
#endif
}
