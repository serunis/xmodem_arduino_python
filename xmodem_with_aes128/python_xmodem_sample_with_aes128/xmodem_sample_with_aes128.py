#!/usr/bin/env python
# coding: utf-8

import serial
from xmodem import XMODEM
from Crypto.Cipher import AES

port = 'COM3'
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

strp = 'testText'
text = strp.encode('utf-8')

#text = b'testText'

text = text.ljust(16, b'\x1a')
#set text to 128byte(xmodem) and also 16byte align(AES)
#text = b'testTexttestText'

key = b'\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F'
iv = b'\x11\xa1\x2d\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F'
encryptor = AES.new(key, AES.MODE_CBC, iv)  # key is 16 byte so that aes128
encrypted = encryptor.encrypt(text)


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

    received = stream.read()

    decryptor = AES.new(key, AES.MODE_CBC, iv)
    decrypted = decryptor.decrypt(received)

    print(decrypted)



"""
stream = open('file.bin', 'rb')
modem.send(stream)


stream = open('output', 'wb')
modem.recv(stream)
"""
