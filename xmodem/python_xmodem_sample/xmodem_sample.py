#!/usr/bin/env python
# coding: utf-8

import serial
from xmodem import XMODEM

port = 'COM6'
ser = serial.Serial(port, timeout=1, baudrate=9600)  # or whatever port you need


def getc(size, timeout=2):
    toutCnt = 0
    toutCntMax = timeout*10
    while ser.in_waiting == 0:
        if toutCnt > toutCntMax:
            print('timeout')
        else:
            toutCnt +=1
            time.sleep(0.1)
        pass
    ret = ser.read(size)
    return ret or None


def putc(data, timeout=2):
    ret = ser.write(data)  # note that this ignores the timeout
    ser.flush()
    return ret or None


modem = XMODEM(getc, putc)

from io import BytesIO
text = b'testText'
stream = BytesIO(text)

#stream = open('text.txt', 'rb')   # send file over xmodem


ret = modem.send(stream)  # xmodem use ^Z (0x1a) as empty symbol for 128 byte buffer

print('send:', ret)
if ret is True:
    #recvBuffer = b''
    stream = BytesIO()
    #stream = open('get.txt', 'wb')  # receive file over xmodem
    ret = modem.recv(stream)
    print('recv:', ret)
    stream.seek(0)
    print(stream.read())



"""
stream = open('file.bin', 'rb')
modem.send(stream)


stream = open('output', 'wb')
modem.recv(stream)
"""
