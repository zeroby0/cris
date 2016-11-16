#include <EEPROM.h>
#include <string.h>
#include "Arduino.h"
#include "Adafruit_FONA.h"
#include "api.h"

using namespace std;

extern Adafruit_FONA fona;
extern char replybuffer[];
/********* COMM ********************************************/
void API::send(char * message){
  Serial.println(message);
}
/********* AUTH number ********************************************/
API::API(){
  EEPROM.get(AUTH_Addr, AUTH);
  if(! (this->AUTH).isInit ){
    this->clearEEPROM();
    this->clearSMS();


  }
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
}


bool API::isAuthMessage(uint8_t t_id){

  this->getSenderNumber(t_id, replybuffer);
  if( ( ! strcmp( replybuffer, this->getAuthNumber() ) )  ){
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


/********* PARSE ********************************************/
void API::parse(char t_buffer[]){
  this->getTime(t_buffer);
  this->sendSMS(t_buffer);
  Serial.println(t_buffer);
}
void API::parseMessage(uint8_t t_id){
  this->getMessageText(t_id, replybuffer);
  this->parse(replybuffer);
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
  Serial.println("Blink Blink");
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
