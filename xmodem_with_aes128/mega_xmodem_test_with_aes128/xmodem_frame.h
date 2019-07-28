#include <HardwareSerial.h>
#include "XModem.h"



void xmodem_set_channel(HardwareSerial *channel);

bool xmodem_send(unsigned char* data, int len);
bool xmodem_receive(void);

bool xmodem_send_with_aes128(unsigned char* data, int len);
bool xmodem_receive_with_aes128(void);

unsigned char* xmodem_get_buffer(void);
unsigned long xmodem_last_indexed_Rx_frame_number(void);
