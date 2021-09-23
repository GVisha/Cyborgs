#include <ctime>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <cstring>


// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

using namespace std;

// To simulate loss of messages
int rand_number(){
	int max = 1;
//	int min = 0;	//use when implementing at-least-once or at-most-once invocation
	int min = 1;   // use when not implementing any invocation
	int randNum = std::rand() % (max - min + 1) + min;
	return randNum;	
}
int invocation_method = 1; // use when implementing any at-most-once invocation
// int invocation_method = 2; // use when implementing any at-least-once invocation

map<string, string> History_Map;

string service1(string filePath, int offset, int size) {
	ifstream inFile;
	char* data = 0;
	string result;

	cout << "Entered service1 : " << filePath << endl;
	inFile.open(filePath, ios::in | ios::binary | ios::ate);
	if (!inFile) {
		cerr << "Cannot open the File : sample" << endl;
		return "Cannot open/find file " + filePath;
	}

	inFile.seekg(0, ios::end); // set pointer to end of file
	int fileSize = inFile.tellg(); // get length of the file
	cout << "Size of file: " << fileSize << endl;
	inFile.seekg(0, ios::beg); // set pointer to beginning of file

	data = new char[fileSize + 1]; // for '\0'
	inFile.read(data, fileSize);
	data[fileSize] = '\0'; // set '\0' 

	if (offset > fileSize) {
		cout << "Offset exceeds file length!" << endl;
		return "Offset exceeds file length!";
	}
	else if (offset + size > fileSize)
		return "Length of offset + size exceeds file length!";
	else {
		//print data
		for (int i = 0; i < size + offset; i++)
		{
			if (i >= offset) {
				result = result + data[i];
			}
		}
		cout << endl;
	}
	inFile.close();
	return result;
}

string service2(string filePath, int offset, char inputString[]) {
	ifstream inFile2;
	ofstream outFile;
	vector <char> tempVec;

	inFile2.open(filePath, ios::in | ios::binary | ios::ate);
	if (!inFile2) {
		cerr << "Cannot open the File : sample" << endl;
		return "Cannot open/find file " + filePath;
	}

	inFile2.seekg(0, ios_base::end);
	streampos fileSize = inFile2.tellg();
	tempVec.resize(fileSize);
	if (offset > fileSize) {
		cout << "Offset exceeds file length!" << endl;
		return "Offset exceeds file length!";
	}

	inFile2.seekg(0, ios_base::beg);
	inFile2.read(&tempVec[0], fileSize);

	tempVec.insert(tempVec.begin() + offset, inputString, inputString + strlen(inputString));

	outFile.open(filePath, ios::out | ios::binary | ios::ate);
	outFile.write(&tempVec[0], tempVec.size());

	cout << "Successful insertion!" << endl;

	inFile2.close();
	outFile.close();

	return "Successful insertion!";
}

// Deletes a char at given offset of file
string service4(string filePath, int offset) {
	ifstream inFile2;
	ofstream outFile;
	vector <char> tempVec;

	inFile2.open(filePath, ios::in | ios::binary | ios::ate);
	if (!inFile2) {
		cerr << "Cannot open the File : sample" << endl;
		return "Cannot open/find file " + filePath;
	}

	inFile2.seekg(0, ios_base::end);
	streampos fileSize = inFile2.tellg();
	tempVec.resize(fileSize);
	if (offset > fileSize) {
		cout << "Offset exceeds file length!" << endl;
		return "Offset exceeds file length!";
	}

	inFile2.seekg(0, ios_base::beg);
	inFile2.read(&tempVec[0], fileSize);

	for (int i = 0; i < tempVec.size(); i++)
	{
		if (i == offset) {
			vector<char>::iterator itr = tempVec.begin();
			itr += i;
			prev(tempVec.erase(itr));
			break;
		}
	}

	outFile.open(filePath, ios::out | ios::binary | ios::ate);
	outFile.write(&tempVec[0], tempVec.size());
	cout << "Successful deletion!" << endl;
	
	inFile2.close();
	outFile.close();

	return "Successful deletion!";
}


// Returns number of times a char occurs in file
string service5(string filePath, char element){
	ifstream inFile2;
	ofstream outFile;
	vector <char> tempVec;

	inFile2.open(filePath, ios::in | ios::binary | ios::ate);
	if (!inFile2) {
		cerr << "Cannot open the File : sample" << endl;
		return "Cannot open/find file " + filePath;
	}

	inFile2.seekg(0, ios_base::end);
	streampos fileSize = inFile2.tellg();
	tempVec.resize(fileSize);

	inFile2.seekg(0, ios_base::beg);
	inFile2.read(&tempVec[0], fileSize);

	int freq = count(tempVec.begin(), tempVec.end(), element);
	string result = "The element ";
	result += element;
	result += " appears ";
	result += to_string(freq);
	result += " times";

	return result;
}

void main()
{
	///////////////////// START WINSOCK /////////////////////
	WSADATA data; // Structure to store WinSock version.
	/*Server is going to use Winsock version 2. Creating a word that 
	will store 2 and 2 in hex i.e. 0x0202*/
	WORD version = MAKEWORD(2, 2);

	int wsResult = WSAStartup(version, &data);
	if (wsResult != 0){
		cout << "Cannot start Winsock! " << wsResult;
		return;
	}

	////////////// CREATE SOCKET AND BIND //////////////////
	// Create user datagram socket (UDP)
	SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);

	// Structure for server
	sockaddr_in saddr;
	saddr.sin_addr.S_un.S_addr = ADDR_ANY; // Use any IP address available on machine
	saddr.sin_family = AF_INET; // Address format is IPv4
	saddr.sin_port = htons(20001); // Convert from little to big endian

	// Bind socket to IP and port
	if (bind(in, (sockaddr*)&saddr, sizeof(saddr)) == SOCKET_ERROR){
		cout << "Can't bind socket! " << WSAGetLastError() << endl;
		return;
	}

	/////////// MAIN LOOP SETUP AND ENTRY ////////////////

	sockaddr_in client; // To hold client information (port / ip address)
	int clientLength = sizeof(client); // Size of client information
	char buf[1024];

	// Struct to hold information of client who has registered for monitoring (service 3)
	struct registered_client{
		int monitor_time;
		time_t current_time;
		struct sockaddr_in addr;
		string filePath;
	};
	vector<registered_client> clients_update;

	// Data structure to support approximate one-copy update in client
	map<string, time_t> fileUpdates;
	srand( (unsigned)time(NULL) );  // used to generate random integer when use 
	
	while (true)
	{
		ZeroMemory(&client, clientLength); // Clear the client structure
		ZeroMemory(buf, 1024); // Clear the receive buffer

		// Wait for message
		int bytesIn = recvfrom(in, buf, 1024, 0, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR)
		{
			cout << "Error receiving from client " << WSAGetLastError() << endl;
			continue;
		}

		// Display the message / who sent it
		cout << "Message received: " << buf << endl;
		char *next_token = NULL;
		char *token = strtok_s(buf, ",", &next_token);
		cout << "first token (Message ID):" << *token << endl;
		string msgID(token);

		// If invocation_method = at most once, execute if loop
		if (invocation_method == 1) {
			map<string, string>::iterator i = History_Map.find(msgID);
			// If message is a duplicate, send its result string to client without execution
			if (i != History_Map.end()) {
				string result = i->second;
				cout << "result: " << result << endl;
				int lengthOfBytes = result.length() + 1;
				char * ch = new char[lengthOfBytes];
				strcpy_s(ch, lengthOfBytes, result.c_str());
				cout << "ch: " << ch << endl;

				int sendOk = sendto(in, ch, lengthOfBytes, 0, (sockaddr*)&client, clientLength);
				if (sendOk == SOCKET_ERROR)
					cout << "Send back to client didn't work! " << WSAGetLastError() << endl;
				continue;
			}
		}

		token = strtok_s(NULL, ",", &next_token);
		cout << "second token (Service Number):" << *token << endl;
		int service_number = atoi(token);

		string message = buf;

		switch (service_number){
			case 1: {
				cout << "service 1 triggered: " << endl;

				token = strtok_s(NULL, ",", &next_token);
				cout << "third token (filepath):" << *token << endl;
				string filePath(token);

				token = strtok_s(NULL, ",", &next_token);
				cout << "fourth token (offset):" << *token << endl;
				int offset = atoi(token);

				token = strtok_s(NULL, ",", &next_token);
				cout << "fifth token (size):" << *token << endl;
				int size = atoi(token);

				string result = service1(filePath, offset, size);
				cout << "result: " << result << endl;
				History_Map[msgID] = result;
				int lengthOfBytes = result.length()+1;
				char * ch = new char [lengthOfBytes];
				strcpy_s(ch, lengthOfBytes, result.c_str());
				cout << "ch: " << ch << endl;

				// Edit rand_number() function to simulate loss of messages
				int randNum = rand_number();
				if (randNum == 1){
					int sendOk = sendto(in, ch, lengthOfBytes, 0, (sockaddr*)&client, clientLength);
					if (sendOk == SOCKET_ERROR)
						cout << "Send back to client didn't work! " << WSAGetLastError() << endl;
				}
				
				break;
			}

			case 2: {
				cout << "service 2 triggered: " << endl;
				
				token = strtok_s(NULL, ",", &next_token);
				cout << "third token:" << *token << endl;
				string filePath(token);
				
				token = strtok_s(NULL, ",", &next_token);
				cout << "fourth token:" << *token << endl;
				int offset = atoi(token);
				
				token = strtok_s(NULL, ",", &next_token);
				cout << "fifth token:" << *token << endl;
				string temp(token);
				
				char cstr[1000];
				strcpy_s(cstr, temp.length() +1, temp.c_str());

				string result = service2(filePath,offset,cstr);


				/* Support approximate one-copy update in client ( upon writing a file, update 
				its modified timing in current calendar time (seconds since Jan 1, 1970) ) */
				map<string, time_t>::iterator it;
				pair <map<string, time_t>::iterator, bool> ptr; 
				ptr = fileUpdates.insert(pair<string, time_t>(filePath, time(NULL)));
				//if filepath recorded before, update existing key-value pair in map
				if(!ptr.second) {
					fileUpdates[filePath]=time(NULL);
				}
				

				cout << "result: " << result << endl;
				History_Map[msgID] = result;
				int lengthOfBytes = result.length()+1;
				char * ch = new char [lengthOfBytes];
				strcpy_s(ch, lengthOfBytes, result.c_str());
				cout << "ch: " << ch << endl;

				// Edit rand_num() function to simulate loss of messages
				int randNum = rand_number();
				if (randNum == 1)
				{
					int sendOk = sendto(in, ch, lengthOfBytes, 0, (sockaddr*)&client, clientLength);
					if (sendOk == SOCKET_ERROR)
					{
						cout << "Send back to client didn't work! " << WSAGetLastError() << endl;
					}
				}


				/* Code to send file content after updates to all clients who have registered for monitoring.
				Whenever service 2 invoke, delete expired registered clients and send file content to unexpired clients */
				ifstream t(filePath);
				string file_content((std::istreambuf_iterator<char>(t)),istreambuf_iterator<char>());
				int len_content = file_content.length() + 1;
				char * file_ch = new char [len_content];
				strcpy_s(file_ch, len_content, file_content.c_str());
				
				std::stringstream ss;
				if (fileUpdates.find(filePath) == fileUpdates.end()) {
					fileUpdates.insert(pair<string, time_t>(filePath, time(NULL)));
				}
				ss << fileUpdates.find(filePath)->second;
				std::string updated_time = ss.str();
//				History_Map[msgID] = updated_time;
				lengthOfBytes = updated_time.length()+1;
				ch = new char [lengthOfBytes];
				strcpy_s (ch, lengthOfBytes, updated_time.c_str());
//				cout << "File modified timestamp: " << ch << endl;

				//update all the other clients
				for (vector<registered_client>::iterator it=clients_update.begin();it!=clients_update.end();){
					if ((time(NULL) - it->current_time) > it->monitor_time) {
						cout << "time elapsed over monitoring interval. " << time(NULL) - it->current_time << endl;
						it = clients_update.erase(it);
					}
					else if(it->filePath == filePath) {
						cout << "time elapsed within monitoring interval. " << time(NULL) - it->current_time << endl;
						int randNum = rand_number();
						if (randNum == 1)
						{
//							int sendOk = sendto(in, file_ch, lengthOfBytes, 0, (sockaddr*)&(it->addr), clientLength);
							int sendOk = sendto(in, file_ch, len_content, 0, (sockaddr*)&(it->addr), clientLength);
							int sendOk_1 = sendto(in, ch, lengthOfBytes, 0, (sockaddr*)&(it->addr), clientLength);
							if (sendOk == SOCKET_ERROR)
							{
								cout << "Send full content of file back to client didn't work! " << WSAGetLastError() << endl;
							}
							if (sendOk_1 == SOCKET_ERROR)
							{
								cout << "Send updated time back to client didn't work! " << WSAGetLastError() << endl;
							}
						}
						++it;
					}
				}

				break;

			}

			case 3: {
				cout << "service 3 triggered: " << endl;
 
				token = strtok_s(NULL, ",", &next_token);
				cout << "third token:" << *token << endl;
				string filePath(token);
				token = strtok_s(NULL, ",", &next_token);
				cout << "fourth token:" << *token << endl;
				int monitor_interval = atoi(token);

				/*No service3() function needed because is only 2 lines of code needed to 
				register client sending response back to client is from service2()*/
				registered_client rc = {monitor_interval, time(NULL), client, filePath};
				clients_update.push_back(rc);
				
				// START: add by Yuxin without testing//
				string reply_msg = "Interval monitoring starts!";
				History_Map[msgID] = reply_msg;
				int lengthOfBytes = reply_msg.length()+1;
				char * ch = new char [lengthOfBytes];
				strcpy_s(ch, lengthOfBytes, reply_msg.c_str());
				// Edit rand_num() function to simulate loss of messages
				int randNum = rand_number();
				if (randNum == 1)
				{
					int sendOk = sendto(in, ch, lengthOfBytes, 0, (sockaddr*)&client, clientLength);
					if (sendOk == SOCKET_ERROR)
					{
						cout << "Send back to client didn't work! " << WSAGetLastError() << endl;
					}
				}
					
				break;

			}

			case 4: {
				cout << "service 4 triggered: " << endl;
 
				token = strtok_s(NULL, ",", &next_token);
				cout << "third token:" << *token << endl;
				string filePath(token);
				token = strtok_s(NULL, ",", &next_token);
				cout << "fourth token:" << *token << endl;
				int offset = atoi(token);

				string result = service4(filePath,offset);

				cout << "result: " << result << endl;
				History_Map[msgID] = result;
				int lengthOfBytes = result.length()+1;
				char * ch = new char [lengthOfBytes];
				strcpy_s(ch, lengthOfBytes, result.c_str());
				cout << "ch: " << ch << endl;

				// Edit rand_num() function to simulate loss of messages
				int randNum = rand_number();
				if (randNum == 1)
				{
					int sendOk = sendto(in, ch, lengthOfBytes, 0, (sockaddr*)&client, clientLength);
					if (sendOk == SOCKET_ERROR)
					{
						cout << "Send back to client didn't work! " << WSAGetLastError() << endl;
					}
				}

				/*Code to send file content after updates to all clients who have registered for monitoring.
				Whenever service 4 is invoked, delete expired registered clients and send file content to unexpired clients*/
				ifstream t(filePath);
				string file_content((std::istreambuf_iterator<char>(t)),istreambuf_iterator<char>());
				int len_content = file_content.length() + 1;
				char * file_ch = new char [len_content];
				strcpy_s(file_ch, len_content, file_content.c_str());
				
				std::stringstream ss;
				if (fileUpdates.find(filePath) == fileUpdates.end()) {
					fileUpdates.insert(pair<string, time_t>(filePath, time(NULL)));
				}
				ss << fileUpdates.find(filePath)->second;
				std::string updated_time = ss.str();
//				History_Map[msgID] = updated_time;
				lengthOfBytes = updated_time.length()+1;
				ch = new char [lengthOfBytes];
				strcpy_s (ch, lengthOfBytes, updated_time.c_str());
//				cout << "File modified timestamp: " << ch << endl;

				//update all the other clients
				for (vector<registered_client>::iterator it=clients_update.begin();it!=clients_update.end();){
					if ((time(NULL) - it->current_time) > it->monitor_time) {
						cout << "time elapsed over monitoring interval. " << time(NULL) - it->current_time << endl;
						it = clients_update.erase(it);
					}
					else if(it->filePath == filePath) {
						cout << "time elapsed within monitoring interval. " << time(NULL) - it->current_time << endl;

						// Edit rand_num() function to simulate loss of messages
						int randNum = rand_number();
						if (randNum == 1)
						{
//							int sendOk = sendto(in, file_ch, lengthOfBytes, 0, (sockaddr*)&(it->addr), clientLength);
							int sendOk = sendto(in, file_ch, len_content, 0, (sockaddr*)&(it->addr), clientLength);
							int sendOk_1 = sendto(in, ch, lengthOfBytes, 0, (sockaddr*)&(it->addr), clientLength);
							if (sendOk == SOCKET_ERROR)
							{
								cout << "Send full content of the file back to client didn't work! " << WSAGetLastError() << endl;
							}
							if (sendOk_1 == SOCKET_ERROR)
							{
								cout << "Send updated time back to client didn't work! " << WSAGetLastError() << endl;
							}

						}
						++it;
					}
				}

				break;

			}

			case 5: {
				cout << "service 5 triggered: " << endl;

				token = strtok_s(NULL, ",", &next_token);
				cout << "third token:" << *token << endl;
				string filePath(token);
				token = strtok_s(NULL, ",", &next_token);
				cout << "fourth token:" << *token << endl;
				char element = *token;

				string result = service5(filePath,element);
				

				cout << "result: " << result << endl;
				History_Map[msgID] = result;
				int lengthOfBytes = result.length()+1;
				char * ch = new char [lengthOfBytes];
				strcpy_s(ch, lengthOfBytes, result.c_str());
				cout << "ch: " << ch << endl;

				// Edit rand_num() function to simulate loss of messages
				int randNum = rand_number();
				if (randNum == 1)
				{
					int sendOk = sendto(in, ch, lengthOfBytes, 0, (sockaddr*)&client, clientLength);
					if (sendOk == SOCKET_ERROR)
					{
						cout << "Send back to client didn't work! " << WSAGetLastError() << endl;
					}
				}

				break;
			}


			/* support approximate one-copy update in client ( send back most recent modified 
			returns the current calendar time (seconds since Jan 1, 1970) ) */
			case 6: {
				cout << "service 6 triggered to return T(mserver) timing specific to filepath" << endl;
				token = strtok_s(NULL, ",", &next_token);
				cout << "third token:" << *token << endl;
				string filePath(token);


				std::stringstream ss;
				if (fileUpdates.find(filePath) == fileUpdates.end()) {
					fileUpdates.insert(pair<string, time_t>(filePath, time(NULL)));
				}
				ss << fileUpdates.find(filePath)->second;
				std::string result = ss.str();
				History_Map[msgID] = result;

				int lengthOfBytes = result.length()+1;
				char * ch = new char [lengthOfBytes];
				strcpy_s (ch, lengthOfBytes, result.c_str());
				cout << "File modified timestamp: " << ch << endl;

				// Edit rand_num() function to simulate loss of messages
				int randNum = rand_number();
				if (randNum == 1)
				{
					int sendOk = sendto(in, ch, lengthOfBytes, 0, (sockaddr*)&client, clientLength);
					if (sendOk == SOCKET_ERROR)
					{
						cout << "Send back to client didn't work! " << WSAGetLastError() << endl;
					}
				}

				break;
			}

			/* support approximate one-copy update in client ( send back whole file content )*/
			case 7: {
				cout << "service 7 triggered" << endl;
				token = strtok_s(NULL, ",", &next_token);
				cout << "third token:" << *token << endl;
				string filePath(token);

				ifstream t(filePath);
				string file_content((std::istreambuf_iterator<char>(t)),istreambuf_iterator<char>());
				History_Map[msgID] = file_content;
				int len_content = file_content.length() + 1;
				char * file_ch = new char [len_content];
				strcpy_s (file_ch, len_content, file_content.c_str());

				// Edit rand_num() function to simulate loss of messages
				int randNum = rand_number();
				if (randNum == 1)
				{
					int sendOk = sendto(in, file_ch, len_content, 0, (sockaddr*)&client, clientLength);
					if (sendOk == SOCKET_ERROR)
					{
						cout << "Send back to client didn't work! " << WSAGetLastError() << endl;
					}
				}

				break;
			}
			
		}

	}


	// Close socket
	closesocket(in);

	// Shutdown winsock
	WSACleanup();
}
