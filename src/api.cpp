#include <EEPROM.h>
#include <string.h>
#include "Arduino.h"
#include "Adafruit_FONA.h"
#include "api.h"
#include "cmd.h"

using namespace std;

extern Adafruit_FONA fona;

/********* COMM ********************************************/
/* communication
   send alerts and messages from here */
void API::send(char * message){
  // casual debug messages
  Serial.println(message);
}

void API::lsend(char * message){
  Serial.print(message);
}

void API::send(const char* message){
  Serial.println(message);
}

void API::send(uint8_t number){
  Serial.println(number);
}

void API::alert(char * message){
  // Alert the user
  Serial.print( "ALERT: ");
  Serial.print(message);
}

// API constructor
API::API(){
  send("[API Init]");
  AUTH_Addr = 10;

  Auth_creds AUTH = {1, "0", "0"};

  // Initial setup
  // AUTH.isInit = 1;
  // strcpy(AUTH.AuthNumber, "+918095074504");
  // strcpy(AUTH.AuthPass ,"1234");
  //EEPROM.put(AUTH_Addr, AUTH);

  EEPROM.get(AUTH_Addr, AUTH);

  this->AUTH =  AUTH;

  // clear the EEPROM is pin not already set
  if( (this->AUTH).isInit  == 0){
    //send("OOOOOOOOOOOOOOOOO");
    this->clearEEPROM();
  }

  sprintf(api_buffer, "%d %s\n", AUTH.isInit, AUTH.AuthNumber);
  send(api_buffer);

}
API::~API(){}


/********* AUTH number ********************************************/
char*  API::getAuthNumber(){
  return AUTH.AuthNumber; // returns Authorised Phone Number
}

// private
bool API::setAuthNumber(char* t_number){  // set Authorised Phone Number
  strcpy(AUTH.AuthNumber, t_number);
  EEPROM.put(AUTH_Addr, this->AUTH); // save to EEPROM
  return true;
}

/* check if the message arrived from the Authorised Phone Number
 * Accepts a number ( phone number ) and verifies it against the stored Number
 */
bool API::isAuthMessage(uint8_t t_id){

  this->getSenderNumber(t_id, api_buffer);
  if( ( ! strcmp( api_buffer, this->getAuthNumber() ) )  ){
    return true;
  } return false;
}

/********* SMS ********************************************/
bool API::clearSMS(){
  // clear ALL SMS in sim
  // get number of SMS
  int8_t smsnum = fona.getNumSMS();
  while(smsnum > 0){
      if (fona.deleteSMS(smsnum)) {
        // delete by id(number) of sms
      } else {
        // failed
        // return false?
      }
      smsnum = fona.getNumSMS();
  } return true;
}
 
/* private
 * Send SMS to a number other that the Authorised Phone number
 *
 */
bool API::sendSMS_custom(char* number, char* message){
  if (!fona.sendSMS( number, message)) {
    // Failed!
    return false;
  } else {
    // Sent!
    return true;
  }
}

/*
 * Send sms to Authorised Number
 */
bool API::sendSMS(char * t_message) {
  // send message to authorised number
  return this->sendSMS_custom( this->getAuthNumber(), t_message);
}

/*
 * get number of sms stored in sim
 */
uint8_t API::getNumberofSMS(){
  return fona.getNumSMS();
}

/*
 * get sender of a SMS with id: t_id
 * (id of message, return buffer with phone number)
 */
bool API::getSenderNumber(uint8_t t_id, char t_buffer[]){
  if (! fona.getSMSSender(t_id, t_buffer, 250)) {
    // Failed
    return false;
  } return true;
}

/*
 * Get message text of message with id: t_id
 * (id of message, return buffer with message text)
 */ 
bool API::getMessageText(uint8_t t_id, char t_buffer[]){
  uint16_t smslen;
  if (! fona.readSMS(t_id, t_buffer, 250, &smslen)) { // pass in buffer and max len!
    // failed
    return false;
  } return true;
}



/********* LOCAL ********************************************/
/*
 * Wait till module is ready
 */
void API::checkStatus(){
  uint8_t n = fona.getNetworkStatus();
  while(true){
      // n == 1: roaming, registered; n ==5: local, registered;
      if(n == 1 || n == 5){
        // registered!
        break;
      } else {
        delay(1000);
        n = fona.getNetworkStatus();
      }

  }
}

/*
 * Clear contents of EEPROM
 * Warning! Removes all authorisation
 */
void API::clearEEPROM(){

  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

/********* TIME ********************************************/

/* 
 * get time provided by network provider
 */
void API::getTime(char* t_buffer){
  fona.getTime(t_buffer, 23);
}

/* 
 * Unsigned short int
 */

/*
 * get year as YYYY
 */
unsigned short int API::getYear(){
  return this->getNum('a' + 1, 'a' + 2);
}

/*
 * get Month
 */
unsigned short int API::getMonth(){
    return this->getNum('a' + 4, 'a' + 5);
}
/*
 * duh! get day
 */
unsigned short int API::getDay(){
    return this->getNum('a' + 7, 'a' + 8);
}

/*
 * get hour
 */
unsigned short int API::getHour(){
    return this->getNum('a' + 10, 'a' + 11);
}

/*
 * get minute
 */
unsigned short int API::getMinute(){
    return this->getNum('a' + 13, 'a' + 14);
}

/*
 * get second
 */
unsigned short int API::getSecond(){
    return this->getNum('a' + 16, 'a' + 17);
}

/* 
 * PRIVATE
 * internal calculation
 */

unsigned short int API::getNum(char t_A, char t_B){
  char buffer[23];
  this->getTime(buffer);
  return (buffer[ t_A - 'a' ] - '0')*10 + (buffer[ t_B - 'a' ] - '0');
  /* sigh! the things we do for memory.. */
}

/********* OTHER ********************************************/
/* 
 * blink the led
 * for debugging and event notification
 */
void API::blink(){
  send("[Blink Blink]");
  digitalWrite(13, HIGH);
  delay(300);
  digitalWrite(13, LOW);
  delay(100);
  digitalWrite(13, HIGH);
  delay(700);
  digitalWrite(13, LOW);
  delay(100);
}

/********* Persistance ********************************************/

/*
 * save the current state in EEPROM
 */
void API::saveState(){
  EEPROM.put(AUTH_Addr, this->AUTH);
}

/*
 * get bit address of AUTH
 * in EEPROM
 */
uint8_t API::getAUTH_Addr(){
  return AUTH_Addr;
}

/********* PARSE ********************************************/

/* API parser functions.
 * checks if the number is authorised
 * and performs tasks according to the
 * instruction
 */

/*
 * to be called by API:parse() if message is from AUTH seder
 */
void API::parseMessage(const char t_buffer[]){ // takes in message text

  Serial.println(t_buffer);  // print message text. Dbug
  /* execution of tasks is delegated to command object
   * calculating argument to be passed
   * to cmd object
   * supports 999 commands
   * calculating id of command ( see cmd.cpp or cmd.h for more info)
   */

  uint8_t command = (t_buffer[0] - '0') * 100  + (t_buffer[1]  - '0') * 10  + (t_buffer[2] - '0' ) * 1;

  CMD cmd; // in scope cmd object ; destroyed as soon as function scopes out
  cmd.execute(command, t_buffer); // sending buffer incase further arguments are available

}

/*
 * to be called from init script
 * for further validation
 * takes in message id as arg
 */
void API::parse(uint8_t t_id){

  send("[parsing]");
  this->getMessageText(t_id, api_buffer);

  if( (this->AUTH).isInit  == 0){  // if module is still not initialised
    if( !strcmp("INIT", api_buffer) ){ // if the message sent is "INIT", initialise the module

      /*
       * module initialisation
       * store sender number as auth number
       * generate a random 4 digit password
       * message it back to sender
       * return;
       */
      send("{Setting up for the first time}");
      AUTH.isInit = 1;
      getSenderNumber(t_id, api_buffer); // get sender number
      strcpy( AUTH.AuthNumber , api_buffer ); // copy number to AUTH object

      sprintf(AUTH.AuthPass, "%d", random(1001, 9999) );  // gen password

      saveState(); // save AUTH object ( password + auth number) to EEPROM
      sprintf(api_buffer, "%s %s\n", "Initialised succesfully! Use this number to send instructions from now on. Your password is " , AUTH.AuthPass);
      sendSMS(api_buffer);

      send("{Clearing SMS}");
      clearSMS();
      sprintf(api_buffer, "[ Done, number: %s ;password: %s; ]\n", AUTH.AuthNumber, AUTH.AuthPass);
      send(api_buffer);
      return;

    }
  }

  /*
   * if module has already been initialised
   * and the sender is authorised,
   * forward the message for text parsing
   */
  else if( isAuthMessage(t_id) ){
    send("{Message from Auth number}");
    this->getMessageText(t_id, api_buffer);
    parseMessage(api_buffer);
  }

  return;
}
