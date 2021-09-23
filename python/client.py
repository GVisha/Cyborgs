#!/usr/bin/python
#! python3

import socket
import datetime
import time
from client_caching import MyCache

def create_socket(timeout=2):
	# Create a UDP socket at client side
	UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
	UDPClientSocket.settimeout(2)
	return UDPClientSocket
	
	
def msg_timeout_handling(UDPClientSocket, request):
	
	while(True):
#		UDPClientSocket = create_socket(timeout)
		UDPClientSocket = message_to_server(UDPClientSocket, request)
		msg = message_from_server(UDPClientSocket)
		
		if msg is not None:
			msg = msg.decode("utf-8")
			break;	
					
	return UDPClientSocket,msg


def message_to_server(UDPClientSocket,msg):
	msgFromClient       = msg
	print("Message sent to server: {}".format(msgFromClient))
	bytesToSend         = str.encode(msgFromClient)
	
	# Send to server using created UDP socket
	UDPClientSocket.sendto(bytesToSend, serverAddressPort)
	return UDPClientSocket


def message_from_server(UDPClientSocket):
	try:
		server_msg, server_address = UDPClientSocket.recvfrom(bufferSize)
		return server_msg
	except socket.timeout:
		print("Requst timeout! resend the request message!")


def msg_id_datetime():
	date_time = datetime.datetime.now()
	msg_id = str(date_time).replace(" ","").replace("-","").replace(":","").replace(".","")
	return msg_id


def read_content(task_id):
	pathname_msg = input("Please indicate the pathname of the file: ")
	offset_msg = input("Please indicate the offset: ")
	num_bytes = input("Please indicate the number of bytes to read from the file: ")
	
	current_time = int(time.time())

	if pathname_msg in cache_dict.cache and current_time - cache_dict.cache[pathname_msg]['time_validated'] <= freshness_interval:
		print("the file exits in the cache and is within the freshness interval!")
		print("cache_dict.cache[{}]: {}".format(pathname_msg,cache_dict.cache[pathname_msg]))
		return cache_dict.cache[pathname_msg]['content'][int(offset_msg): int(offset_msg) + int(num_bytes)]
	
	UDPClientSocket = create_socket(2)
	
	# request for the latest update time of the file
	msg_id = msg_id_datetime()
	request_msg = msg_id + ',' + file_updated_time_server + ',' + pathname_msg		
	UDPClientSocket, t_modified_at_server = msg_timeout_handling(UDPClientSocket, request_msg)
	t_modified_at_server = int(t_modified_at_server.rstrip('\x00'))	

	if pathname_msg in cache_dict.cache and current_time - cache_dict.cache[pathname_msg]['time_validated'] > freshness_interval and t_modified_at_server == cache_dict.cache[pathname_msg]['t_mclient']:	
		
		print("the cache entry exceeds the freshness interval! but it is valid")				
#		cache_dict.cache[pathname_msg]['time_validated'] = int(time.time())
#		print("'time_validated' is updated!")
		print("cache_dict.cache[{}]: {}".format(pathname_msg,cache_dict.cache[pathname_msg]))

		return cache_dict.cache[pathname_msg]['content'][int(offset_msg): int(offset_msg) + int(num_bytes)]

	else:
		print("the cache entry is invalid/ the file is not in the cache")
		msg_id = msg_id_datetime()
		req_msg_full_content = msg_id + ',' + file_full_content + ',' + pathname_msg
		_, full_content = msg_timeout_handling(UDPClientSocket, req_msg_full_content)
		
		msg_id_1 = msg_id_datetime()
		request_msg = msg_id_1 + ',' + task_id + ',' + pathname_msg + ',' + offset_msg + ',' + num_bytes		
		_, msg = msg_timeout_handling(UDPClientSocket, request_msg)
		
		cache_dict.update(pathname_msg, full_content, t_modified_at_server)
		print("the file is added into cache dict successfully")
		print("cache_dict.cache[{}]: {}".format(pathname_msg,cache_dict.cache[pathname_msg]))
		
		return msg
		
	
def insert_content(task_id):
	pathname_msg = input("Please indicate the pathname of the file: ")
	offset_msg = input("Please indicate the offset: ")
	sequence_bytes = input("Please key in the sequence of bytes to write into the file: ")
	
	msg_id = msg_id_datetime()
	request_msg = msg_id + ',' + task_id + ',' + pathname_msg + ',' + offset_msg + ',' + sequence_bytes

	UDPClientSocket = create_socket(2)
	UDPClientSocket, msg = msg_timeout_handling(UDPClientSocket, request_msg)

	if msg == "Successful insertion!":
		print("Insert successfully!")
		if pathname_msg in cache_dict.cache:
			print("the file exits in cache!")
		msg_id_1 = msg_id_datetime()
		request_msg = msg_id_1 + ',' + file_updated_time_server + ',' + pathname_msg		
		UDPClientSocket, t_modified_at_server = msg_timeout_handling(UDPClientSocket, request_msg)
		t_modified_at_server = int(t_modified_at_server.rstrip('\x00'))	
		msg_id_2 = msg_id_datetime()
		req_msg_full_content = msg_id_2 + ',' + file_full_content + ',' + pathname_msg
		_, full_content = msg_timeout_handling(UDPClientSocket, req_msg_full_content)
		cache_dict.update(pathname_msg, full_content, t_modified_at_server)
		print("the cache entry is updated successfully!")
		print("cache_dict.cache[{}]: {}".format(pathname_msg,cache_dict.cache[pathname_msg]))

	return msg

def monitor_updates(task_id):
	pathname_msg = input("Please indicate the pathname of the file: ")
	monitor_interval = input("Please indicate the monitor interval(in second): ")	
	
	msg_id = msg_id_datetime()
	msg = msg_id + ',' + task_id + ',' + pathname_msg + ',' + monitor_interval
	
	UDPClientSocket = create_socket(2)
	UDPClientSocket, msg = msg_timeout_handling(UDPClientSocket, msg)
	print(msg)
#	UDPClientSocket = message_to_server(UDPClientSocket, msg)
	msg = "No update on the file that we are monitoring!"
	

	while(1):
		try:
			UDPClientSocket.settimeout(float(monitor_interval))
			full_content, server_address = UDPClientSocket.recvfrom(bufferSize)
			full_content = full_content.decode("utf-8")
			msg = full_content
#			msg_id_1 = msg_id_datetime()
#			request_msg = msg_id_1 + ',' + file_updated_time_server + ',' + pathname_msg	
#			UDPClientSocket, t_modified_at_server = msg_timeout_handling(UDPClientSocket, request_msg)
			t_modified_at_server, server_address = UDPClientSocket.recvfrom(bufferSize)
			t_modified_at_server = int(t_modified_at_server.decode("utf-8").rstrip('\x00'))	
		
			print("message is:")
			cache_dict.update(pathname_msg, full_content, t_modified_at_server)
			print("cache_dict.cache[{}]: {}".format(pathname_msg,cache_dict.cache[pathname_msg]))
		except socket.timeout:
			print("Monitoring period ends!")
			break;
	
	return msg


def delete_char_at_offset(task_id):
	pathname_msg = input("Please indicate the pathname of the file: ")
	offset_msg = input("Please indicate one offset that you want to delete: ")
	
	msg_id = msg_id_datetime()
	msg = msg_id + ',' +  task_id + ',' + pathname_msg + ',' + offset_msg
	UDPClientSocket = create_socket(2)
	UDPClientSocket, msg = msg_timeout_handling(UDPClientSocket, msg)

	if msg == "Successful deletion!":
		print("Delete successfully!")
		if pathname_msg in cache_dict.cache:
			print("the file exits in cache!")
		msg_id_1 = msg_id_datetime()
		request_msg = msg_id_1 + ',' + file_updated_time_server + ',' + pathname_msg		
		UDPClientSocket, t_modified_at_server = msg_timeout_handling(UDPClientSocket, request_msg)
		t_modified_at_server = int(t_modified_at_server.rstrip('\x00'))	
		msg_id_2 = msg_id_datetime()
		req_msg_full_content = msg_id_2 + ',' + file_full_content + ',' + pathname_msg
		_, full_content = msg_timeout_handling(UDPClientSocket, req_msg_full_content)
		cache_dict.update(pathname_msg, full_content, t_modified_at_server)
		print("the cache entry is updated successfully!")
		print("cache_dict.cache[{}]: {}".format(pathname_msg,cache_dict.cache[pathname_msg]))	
	
	return msg

def get_char_frequency(task_id):
	pathname_msg = input("Please indicate the pathname of the file: ")
	char = input("Please indicate the char to its frequency: ")

	current_time = int(time.time())

	if pathname_msg in cache_dict.cache and current_time - cache_dict.cache[pathname_msg]['time_validated'] <= freshness_interval:
		print("the file exits in the cache and is within the freshness interval!")
		print("cache_dict.cache[{}]: {}".format(pathname_msg,cache_dict.cache[pathname_msg]))
		return cache_dict.cache[pathname_msg]['content'].count(char)
	
	UDPClientSocket = create_socket(2)
	# request for the latest update time of the file
	msg_id = msg_id_datetime()
	request_msg = msg_id + ',' + file_updated_time_server + ',' + pathname_msg		
	UDPClientSocket, t_modified_at_server = msg_timeout_handling(UDPClientSocket, request_msg)
	t_modified_at_server = int(t_modified_at_server.rstrip('\x00'))	

	if pathname_msg in cache_dict.cache and current_time - cache_dict.cache[pathname_msg]['time_validated'] > freshness_interval and t_modified_at_server == cache_dict.cache[pathname_msg]['t_mclient']:	
		
		print("the cache entry exceeds the freshness interval! but it is valid")			
#		cache_dict.cache[pathname_msg]['time_validated'] = int(time.time())
#		print("'time_validated' is updated!")
		print("cache_dict.cache[{}]: {}".format(pathname_msg,cache_dict.cache[pathname_msg]))

		return cache_dict.cache[pathname_msg]['content'].count(char)

	else:
		print("the cache entry is invalid/ the file is not in the cache")
		msg_id = msg_id_datetime()
		req_msg_full_content = msg_id + ',' + file_full_content + ',' + pathname_msg
		_, full_content = msg_timeout_handling(UDPClientSocket, req_msg_full_content)
		
		msg_id_1 = msg_id_datetime()
		request_msg = msg_id_1 + ',' +  task_id + ',' + pathname_msg + ',' + char		
		_, msg = msg_timeout_handling(UDPClientSocket, request_msg)
		
		cache_dict.update(pathname_msg, full_content, t_modified_at_server)
		print("the file is added into cache dict successfully")
		print("cache_dict.cache[{}]: {}".format(pathname_msg,cache_dict.cache[pathname_msg]))
		
		return msg

	
def call_task(task_id):

	if task_id == "1":
		return read_content(task_id)
	elif task_id == "2":
		return insert_content(task_id)
	elif task_id == "3":
		return monitor_updates(task_id)
	elif task_id == "4":
		return delete_char_at_offset(task_id)
	elif task_id == "5":
		return get_char_frequency(task_id)

		
serverAddressPort   = ('192.168.86.54', 20001)  # Edit IP address and port accordingly
bufferSize          = 1024

cache_dict = MyCache()
freshness_interval = 20   # Set freshness interval for client update is 20 seconds	
file_updated_time_server = '6'  #task_id
file_full_content = '7'  #task)_id
	
while(1):


	print("Welcome to the System for remote file access!")
	print("The services provided:\n 1. Specify an offset to read part of the file content\n 2. Insert a sequence of bytes into a file\n 3. Monitor updates made to a speciified file\n 4. Specify an offset to remove character at that offset \n 5. Specify a character to get its frequency")

	

	task_id = input("hello, please input your service ID: ")


	msg = call_task(task_id)
		
	print("\nMAIN REPLY FROM SERVER: {} \n".format(msg))
	
	print("The existing keys in the cache:")
	cache_dict.print_keys_existing()





