#include <EEPROM.h>
#include <string.h>
#include "Arduino.h"
#include "Adafruit_FONA.h"
#include "api.h"

using namespace std;

extern Adafruit_FONA fona;

/********* COMM ********************************************/
void API::send(char * message){
  // casual debug messages
  Serial.println(message);
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

// API
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
  return AUTH.AuthNumber;
}

// private
bool API::setAuthNumber(char* t_number){
  strcpy(AUTH.AuthNumber, t_number);
  EEPROM.put(AUTH_Addr, this->AUTH);
  return true;
}


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

// private
bool API::sendSMS_custom(char* number, char* message){

  if (!fona.sendSMS( number, message)) {
    // Failed!
    return false;
  } else {
    // Sent!
    return true;
  }
}

bool API::sendSMS(char * t_message) {
  // send message to authorised number
  return this->sendSMS_custom( this->getAuthNumber(), t_message);
}

uint8_t API::getNumberofSMS(){
  return fona.getNumSMS();
}

bool API::getSenderNumber(uint8_t t_id, char t_buffer[]){
  if (! fona.getSMSSender(t_id, t_buffer, 250)) {
    // Failed
    return false;
  } return true;
}

bool API::getMessageText(uint8_t t_id, char t_buffer[]){
  uint16_t smslen;
  if (! fona.readSMS(t_id, t_buffer, 250, &smslen)) { // pass in buffer and max len!
    // failed
    return false;
  } return true;
}



/********* LOCAL ********************************************/
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

void API::clearEEPROM(){

  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}
/********* TIME ********************************************/
void API::getTime(char* t_buffer){
  fona.getTime(t_buffer, 23);
}

unsigned short int API::getYear(){
  return this->getNum('a' + 1, 'a' + 2);
}

unsigned short int API::getMonth(){
    return this->getNum('a' + 4, 'a' + 5);
}

unsigned short int API::getDay(){
    return this->getNum('a' + 7, 'a' + 8);
}

unsigned short int API::getHour(){
    return this->getNum('a' + 10, 'a' + 11);
}

unsigned short int API::getMinute(){
    return this->getNum('a' + 13, 'a' + 14);
}

unsigned short int API::getSecond(){
    return this->getNum('a' + 16, 'a' + 17);
}

//private
unsigned short int API::getNum(char t_A, char t_B){
  char buffer[23];
  this->getTime(buffer);
  return (buffer[ t_A - 'a' ] - '0')*10 + (buffer[ t_B - 'a' ] - '0');
  /* sigh! the things we do for memory.. */
}

/********* OTHER ********************************************/
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

void API::saveState(){
  EEPROM.put(AUTH_Addr, this->AUTH);
}

uint8_t API::getAUTH_Addr(){
  return AUTH_Addr;
}

/********* PARSE ********************************************/
void API::parseMessage(const char t_buffer[]){
  Serial.println(t_buffer);

  

}

void API::parse(uint8_t t_id){

  send("[parsing]");
  this->getMessageText(t_id, api_buffer);

  if( (this->AUTH).isInit  == 0){
    if( !strcmp("INIT", api_buffer) ){
      send("{Setting up for the first time}");
      AUTH.isInit = 1;
      getSenderNumber(t_id, api_buffer);
      strcpy( AUTH.AuthNumber , api_buffer );

      sprintf(AUTH.AuthPass, "%d", random(1001, 9999) );

      saveState();
      sprintf(api_buffer, "%s %s\n", "Initialised succesfully! Use this number to send instructions from now on. Your password is " , AUTH.AuthPass);
      sendSMS(api_buffer);

      send("{Clearing SMS}");
      clearSMS();
      sprintf(api_buffer, "[ Done, number: %s ;password: %s; ]\n", AUTH.AuthNumber, AUTH.AuthPass);
      send(api_buffer);
      return;

    }
  }

  else if( isAuthMessage(t_id) ){
    send("{Message from Auth number}");
    this->getMessageText(t_id, api_buffer);
    parseMessage(api_buffer);
  }

  return;
}
