#include "Arduino.h"
#include "API.h"
#include "cmd.h"

extern API* api;

uint8_t CMD::execute(uint8_t command, const char buffer[]){
  api->lsend("called\n");
  api->send(command);

  switch( command ){
    case 001:
              motor_on();
              break;
    case 002:
              motor_off();
              break;
    case 003:
              motor_on_for();
              break;
    case 004:
              motor_on_after();
              break;
    default:
              api->send("Unknown command");
              break;
  }

  return command;

}

void CMD::motor_on(){
  api->send("(motor ON)");
}

void CMD::motor_off(){
  api->send("(motor OFF)");
}
void CMD::motor_on_for(){
  api->lsend("(motor on for abc)\n");
}
void CMD::motor_on_after(){
  api->lsend("(motor on after abc)\n");
}
