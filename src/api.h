#include "Arduino.h"
class API{
  public:

    API();
    // comm
    void send(char*);

    // AUTH number
    char* getAuthNumber();


    // SMS
    bool clearSMS();
    bool sendSMS(char*);
    uint8_t getNumberofSMS();

    bool isAuthMessage(uint8_t);
    bool getSenderNumber(uint8_t, char*);
    bool getMessageText(uint8_t, char*);

    // Time
    void getTime(char*);
    unsigned short int getYear();
    unsigned short int getMonth();
    unsigned short int getDay();
    unsigned short int getHour();
    unsigned short int getMinute();
    unsigned short int getSecond();

    // Local
    void blink();
    void clearEEPROM();
    void checkStatus();

    // processing
    void parseMessage(char*);
    void parse(uint8_t);
    virtual ~API();

    // Persistance


    // bufferProvided by API
    //char buffer[255];







  private:
    bool updateNeeded = false;
    bool setAuthNumber(char*);

    bool sendSMS_custom(char*, char*);
    unsigned short int getNum(char, char);

    void saveState();

    typedef struct Auth_creds {
      int isInit; // default value is 0, so false
      // 16 bits, all
      char AuthNumber[14];
      char AuthPass[4];
    } Auth_creds;
    Auth_creds AUTH;
    uint8_t AUTH_Addr = 10;



};
