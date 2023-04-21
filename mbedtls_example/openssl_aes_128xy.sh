#!/bin/bash


inputx='\x65\x66\x67\x33\x34\x35\x36\x37\x30\x31\x32\x33\x34\x35\x36\x37\x51' 
#echo -n -e $inputx >  inputx.log

openssl enc -v -aes-128-cbc -in inputx.log -out outputx.log -K 71727374757677786162636465666768 -iv 33333333333333333333333333333333 | od -Ax -tx1 
