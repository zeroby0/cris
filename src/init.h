#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

// this is a large buffer for replies
char replybuffer[255];

uint8_t smsn;



int switchON_Motor();
int switchOFF_Motor();
void getSMS();
