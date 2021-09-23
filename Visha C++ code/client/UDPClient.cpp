/**********************************************************************
Name		: Example UDP Client
Author		: Sloan Kelly
Date		: 2017-12-16
Purpose		: Example of a bare bones UDP client

***********************************************************************/

#include <iostream>
#include <WS2tcpip.h>

// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

// Saves us from typing std::cout << etc. etc. etc.
using namespace std;

void main(int argc, char* argv[]) // We can pass in a command line option!! 
{
	////////////////////////////////////////////////////////////
	// INITIALIZE WINSOCK
	////////////////////////////////////////////////////////////

	// Structure to store the WinSock version. This is filled in
	// on the call to WSAStartup()
	WSADATA data;

	// To start WinSock, the required version must be passed to
	// WSAStartup(). This server is going to use WinSock version
	// 2 so I create a word that will store 2 and 2 in hex i.e.
	// 0x0202
	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		// Not ok! Get out quickly
		cout << "Can't start Winsock! " << wsOk;
		return;
	}

	////////////////////////////////////////////////////////////
	// CONNECT TO THE SERVER
	////////////////////////////////////////////////////////////

	// Create a hint structure for the server
	sockaddr_in server;
	server.sin_family = AF_INET; // AF_INET = IPv4 addresses
	server.sin_port = htons(54000); // Little to big endian conversion
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); // Convert from string to byte array

													   // Socket creation, note that the socket type is datagram
	SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

	// Write out to that socket
	/* 
	Message Formats: 
	Service 1: int Msg ID, int Service Number, string filePathName, 
	int offset, int size
	Service 2: int Msg ID, int Service Number, string filePathName,
	int offset, char inputString[]
	Service 4: int Msg ID, int Service Number, string filePathName,
	int offset
	Service 5: int Msg ID, int Service Number, string filePathName,
	int offset, char element)
	*/
	// string s = "1 1 C:\\Users\\sewer\\OneDrive\\documents\\visual studio 2015\\Projects\\CZ4013 Youtube Server C++\\sample.txt 8, 5";
	string s = "1 1 sample.txt 8 5";
	int sendOk = sendto(out, s.c_str(), s.size() + 1, 0, (sockaddr*)&server, sizeof(server));

	if (sendOk == SOCKET_ERROR)
	{
		cout << "That didn't work! " << WSAGetLastError() << endl;
	}

	/*while (true) {
		cout << "Output of s.c_str: " << s.c_str() << endl;
	}*/

	// Close the socket
	closesocket(out);

	// Close down Winsock
	WSACleanup();
}