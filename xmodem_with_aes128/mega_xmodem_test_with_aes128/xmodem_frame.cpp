#include "xmodem_frame.h"
#include <Arduino.h>
#include "AESLib.h"

HardwareSerial *gModemSerial = &Serial;

void xmodem_set_channel(HardwareSerial *channel)
{
  gModemSerial = channel;
}

int recvChar (int msDelay)
{

  while(msDelay)
  {
    if(gModemSerial->available() == 0)
    {
      msDelay--;
      delay(1); //1ms
      continue;
    }
    else
    {
      //Serial.setTimeout(msDelay);
      char ret = 0;
      gModemSerial->readBytes(&ret, 1); // 1 byte return
      return ret;
    }
  }
  return -1;
}

void sendChar(char sym)
{
  gModemSerial->write(sym);
  gModemSerial->flush();
}

#define MODEM_BUFFER_SIZE     1024
#define MAX_RX_BUFFER_FRAME  (MODEM_BUFFER_SIZE/128) //8

#define RX_BUFFER_MODE 0
#define TX_BUFFER_MODE 1

int gMode = RX_BUFFER_MODE;
int gMaxTxBufferFrameLimit = 1;

unsigned char gModemBuffer[MODEM_BUFFER_SIZE] = {0};
unsigned long LastIndexedRxFrameNumber = 0;

unsigned char* xmodem_get_buffer(void)
{
  return gModemBuffer;
}

unsigned long xmodem_last_indexed_Rx_frame_number(void)
{
  return LastIndexedRxFrameNumber;
}

bool dataHandler(unsigned long number, char *buffer, int len)
{
  int offset = len*(number -1);//number starts from 1
  int i = 0;



    if(gMode == RX_BUFFER_MODE)
    {
      if(number > MAX_RX_BUFFER_FRAME)
          return false;

      LastIndexedRxFrameNumber = number;

      for(i=0; i<len; i++)
      {
        gModemBuffer[offset + i] = buffer[i];
      }
    }
    else//TX_BUFFER_MODE
    {
        if(number > gMaxTxBufferFrameLimit)
          return false;

      for(i=0; i<len; i++)
      {
        buffer[i] = gModemBuffer[offset + i];
      }
    }



  return true;
}


static XModem x = XModem(recvChar, sendChar, dataHandler);


//xmodem use ^Z (0x1a) as empty symbol for 128 byte buffer
//Arduino library do not fill ^Z(0x1a), this code has to take care
//Arduino resets for every Serial connection.

bool xmodem_send(unsigned char* data, int len)
{
  int i;
  int sizeToFill_1a;
  int modValue;

  if((len > MODEM_BUFFER_SIZE) || (len == 0))
    return false;

  modValue = len % 128;
  sizeToFill_1a = 128 - modValue;

  gMode = TX_BUFFER_MODE;
  gMaxTxBufferFrameLimit = len / 128;
  if(modValue != 0)
    gMaxTxBufferFrameLimit += 1;


  memcpy(gModemBuffer, data, len);

  for (i=0; i<sizeToFill_1a; i++)
    gModemBuffer[len+i] = 0x1a;

  return x.transmit();
}

bool xmodem_receive()
{
  bool ret;
  gMode = RX_BUFFER_MODE;
  ret = x.receive();

  return ret;
}


static uint8_t key128[16] ={0x0, 0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF};
static uint8_t IV128[16] ={0x11, 0xa1,0x2d,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF};

void xmodem_set_aes128_key_and_iv(uint8_t* In_key128, uint8_t* In_IV128)
{
	memcpy(key128, In_key128, 16);
	memcpy(IV128, In_IV128, 16);
}


bool xmodem_send_with_aes128(unsigned char* data, int len)
{
  int i;
  int sizeToFill_1a;
  int modValue;

  if((len > MODEM_BUFFER_SIZE) || (len == 0))
    return false;

  modValue = len % 128;
  sizeToFill_1a = 128 - modValue;

  gMode = TX_BUFFER_MODE;
  gMaxTxBufferFrameLimit = len / 128;
  if(modValue != 0)
    gMaxTxBufferFrameLimit += 1;


  memcpy(gModemBuffer, data, len);

  for (i=0; i<sizeToFill_1a; i++)
    gModemBuffer[len+i] = 0x1a;

  aes128_cbc_enc(key128,IV128, gModemBuffer, 128*gMaxTxBufferFrameLimit);

  return x.transmit();
}

bool xmodem_receive_with_aes128()
{
  bool ret;
  gMode = RX_BUFFER_MODE;
  ret = x.receive();

  aes128_cbc_dec(key128, IV128, gModemBuffer, 128*LastIndexedRxFrameNumber);

  return ret;
}
