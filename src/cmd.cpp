/* 
 * The command object
 * Add your custom functions here
 *
 * to add foo(),
 * create a case, before `default` case
 *
 * case 010:
 *      foo(); // optionally, arguments
 *      break;
 *
 * then create a CMD::foo() down below
 *
 * `return type` CMD:foo( `args` ){
      // mama mia!
      // itsa me, Mario!
    }
 *
 * to call your function foo(), id = xyz from sms,
 * send sms `xyz args`
 */


#include "Arduino.h"
#include "API.h"
#include "cmd.h"

extern API* api;

uint8_t CMD::execute(uint8_t command, const char buffer[]){  // message text in  arg: `buffer`
  api->lsend("called\n"); //dBug
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
    // case 080:
    //           foo(buffer);
    //           break;
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

// void CMD::foo(const char buffer[]){   
     // api->send('Hola!\n');
     // sms "080 hello foo bar" calls me
     // buffer[] contains the full message text
// }
