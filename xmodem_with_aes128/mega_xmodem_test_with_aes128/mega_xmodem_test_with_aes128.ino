#include "xmodem_frame.h"


void setup() {
  // put your setup code here, to run once:
  HardwareSerial *modemChannel = &Serial;
  modemChannel->begin(9600);
  xmodem_set_channel(modemChannel);

  //Serial.print("Hello world.");     // "Hello world."
  //Serial.print("\n");


}


void loop() {
  // put your main code here, to run repeatedly:


  bool ret;
  ret = xmodem_receive_with_aes128();

  if(ret == true)
  {
    while(1)
    {
      //Serial.print("received\n");
      //Serial.write((const char*)modembuffer);

      unsigned char* modembuffer = xmodem_get_buffer();
      unsigned long numOfRxFrame128 = xmodem_last_indexed_Rx_frame_number();

      char textBuffer[1024];
      sprintf(textBuffer, "good, numOfRxFrame128:%d\n",numOfRxFrame128);
      strncat(textBuffer,modembuffer,128*numOfRxFrame128);

      xmodem_send_with_aes128(textBuffer, strlen(textBuffer)); //not sizeof, should be strlen

      while(1);
    }
  }

}
