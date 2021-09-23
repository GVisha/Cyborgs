#!/usr/bin/python

import socket
from random import randint
 

localIP     = 'localhost'

localPort   = 20001

bufferSize  = 1024

f = open("text_test.txt", "r+")

msgFromServer       = f.readline()
print(msgFromServer)
bytesToSend         = str.encode(msgFromServer)

 

# Create a datagram socket

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

 

# Bind to address and ip

UDPServerSocket.bind((localIP, localPort))

 

print("UDP server up and listening")

 

# Listen for incoming datagrams


while(True):


	bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)

	message = bytesAddressPair[0]

	address = bytesAddressPair[1]

	clientMsg = "Message from Client:{}".format(message)
	clientIP  = "Client IP Address:{}".format(address)
	
	print(clientMsg)
	print(clientIP)

   

	# Sending a reply to client

	value = randint(1, 1)
	print(value)
	if value == 1:
		UDPServerSocket.sendto(bytesToSend, address)
#		UDPServerSocket.sendto(str.encode("1111111"), address)
	
	
	# test a continuous list of msg for task 3
#	for i in ["1111111","22222222","333333333","444444444"]:
#		print("here")
#		UDPServerSocket.sendto(str.encode(i), address)
