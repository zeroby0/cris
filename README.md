# Cris
Arduino and GSM based remote irrigation system, intended to be a cheaper replacement to existing commercial solutions

Remotely monitor and control your systems. Simple, affordable and extensible.


# development
Clone this repository using `git clone https://github.com/zeroby0/cris.git`. This is a complete [platformio](http://platformio.org) project folder, open in platformio IDE and start hacking!  
Source code is available in `src/`, `lib/` has libraries required by the project.  Using Adafruit FONA library at [https://github.com/adafruit/Adafruit_FONA](https://github.com/adafruit/Adafruit_FONA).

# API

Name : arg1; arg2; : returnType

Current API provides these functionalities
- sendSMS : char* : bool
  * Sends text in char buffer to authorised user
  * returns false if message cannot be sent
- clearSMS: :
  * Clears all SMS in Sim
- getNumberofSMS: : uint8_t
  * returns number of SMS in SIM
