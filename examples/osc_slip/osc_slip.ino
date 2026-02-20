/*
| Command                                           | Description                                                                  | Example                                  |
| ------------------------------------------------- | ---------------------------------------------------------------------------- | ---------------------------------------- |
| `microParamOscDispatch(msg, inputs, inputsCount)` | Update local parameters from received OSC messages. Returns true if handled. | `/input/a 42` → sets `input_a = 42`      |
| `receivedOscMessage.checkOscAddress("/?")`        | "Query" for current parameter values.                                        | `/?"` → triggers sending all inputs back |
| `microParamOscSend(myOsc, inputs, inputsCount)`   | Send the current values of input parameters via OSC (when queried).          | `/input/a 2`, `/input/b 400`             |
| `microParamOscSend(myOsc, outputs, outputsCount)` | Send the current values of output parameters via OSC.                        | `/output/value 127`                      |
*/


#include <Arduino.h>
#include <MicroParam.h>
#include <MicroParamOsc.h>

#include <MicroOscSlip.h>
// The number 128 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscSlip<128> myOsc(&Serial);

unsigned long chrono;

MicroParamInt input_a(2, 0, 127);
MicroParamInt input_b(400, 1, 2000);

MicroParamInt int0(2, -127, 127);
MicroParamInt int1(400, 1, 2000);
MicroParam* twoInts[] = { &int0, &int1 };

MicroParamBindOsc inputs[] = {
    {"/input/a", "i", input_a},
    {"/input/b", "i", input_b},
    {"/twoInts", "ii", twoInts},
};
const size_t inputsCount = sizeof(inputs) / sizeof(MicroParamBindOsc);

MicroParamInt output_value(127, 0, 255);

MicroParamBindOsc outputs[] = {
    {"/output/value", "i", output_value},
    {"/twoInts", "ii", twoInts},
};
const size_t outputsCount = sizeof(outputs) / sizeof(MicroParamBindOsc);


// FUNCTION THAT WILL BE CALLED WHEN AN OSC MESSAGE IS RECEIVED:
void myOscMessageParser(MicroOsc & source, MicroOscMessage & receivedOscMessage)
{
    if ( microParamOscDispatch(receivedOscMessage, inputs, inputsCount) )   
    {
        // successfully dispatched so we return
        return;
    }

    // handle other messages here:
    if ( receivedOscMessage.checkOscAddress("/?") )
    {
        // echo inputs
        microParamOscSend(myOsc, inputs, inputsCount);
    }
}

void setup()
{
    Serial.begin(115200);

    chrono = millis();
}

void loop()
{

    myOsc.onOscMessageReceived(myOscMessageParser);

    if (millis() - chrono >= 500)
    {
        chrono = millis();

        output_value = (int32_t) random(0, 256);

        microParamOscSend(myOsc, outputs, outputsCount);

        myOsc.sendInt("/a0", analogRead(A0));

    }
}
