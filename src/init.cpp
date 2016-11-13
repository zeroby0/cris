#include "Arduino.h"
#include "Adafruit_FONA.h"
#include <EEPROM.h>
#include "string.h"


#include "init.h"
#include "api.h"

// completely incomplete

// TODO use serial ONLY for debugging and eliminate it completely in final build

// defaulting to SoftwareSerial
// comment out these lines and uncomment out following 3 lines
// and uncomment hardware Serial line
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

// Hardware serial is also possible!
// HardwareSerial *fonaSerial = &Serial1;

// for Fona 800 and 900
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
// Use this one for FONA 3G
//Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

API* api = new API();


/*____________________________________________________________________________*/
void setup()
{
  //while(1);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // keep turned off; use only to indicate

  //uncomment to prevent code from running till serial is available
  //while (!Serial);

  Serial.begin(115200);
  api->send("Serial connection started");

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    // couldn't find GSM Sheild
    api->send("couldn't find GSM Sheild");
    while (1);
  }

  api->send("Init");
  //clearEEPROM();
  api->clearSMS();
  //sendSMS("Hey");
  api->send("Cleared texts");
  delay(3000);

}



void loop()
{

  smsn = api->getNumberofSMS();
  if(smsn > 0 && api->isAuthMessage(smsn) ){
    api->send("Recd and Auth");
    api->parseMessage(smsn);

  }if(smsn){ api->send("Clearing SMS"); api->clearSMS(); }
  api->send("Skip");
  // TODO make Arduino sleep to conserve power
  delay(1000);



}
