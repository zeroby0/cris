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

API* api;


/*____________________________________________________________________________*/
void setup()
{

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // keep turned off; use only to indicate
  randomSeed(analogRead(0));  // genarate random seed
  // to use in password generation, if required.



  //uncomment to prevent code from running till serial is available
  // don't uncomment in production
  // module wont start until serial is available
  //while (!Serial);

  digitalWrite(13, LOW);

  Serial.begin(115200);
  Serial.println("{{Start}}");
  api = new API();

  // to reset
  // api->clearEEPROM();
  // api->send("Cleared");
  // while(1);

  api->send("{Serial connection started}");


  // COMM with GSM module
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    // couldn't find GSM Sheild
    api->send("[couldn't find GSM Sheild]");
    while (1);
  }

  // TODO see if any messages are received from AuthNumber when offline
  api->clearSMS(); // clear all sms in sim
  api->send("{Setup complete}");
}



void loop()
{

  digitalWrite(LED_BUILTIN, LOW);
  smsn = api->getNumberofSMS();

  /* check if any sms arrived
   * if yes, send it for parsing
   * clear sms so that any new sms has id 0
   */
  if(smsn > 0){
    api->send("{Message received}");
    api->parse(smsn);
    api->send("{Clearing SMS}");
    api->clearSMS();
    api->send("[done]");
  }


  api->send("}\n \n{");
  delay(1000);
}
