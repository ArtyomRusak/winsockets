// Lab1.Client.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ConnectSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	sockaddr_in add;
	add.sin_family = AF_INET;
	add.sin_port = htons(1024);
	add.sin_addr.s_addr = inet_addr("127.0.0.1");

	int t = sizeof(add);

	

	// Receive until the peer closes the connection
	do {
		cout << "Type string: ";
		char *sendData = new char[100];
		cin.getline(sendData, 100 * sizeof(char));

		cout << "Sending data..." << endl;
		// Send an initial buffer
		iResult = sendto(ConnectSocket, sendData, (int)strlen(sendData), 0, (struct sockaddr*)&add, t);

		delete sendData;

		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		/*printf("Bytes Sent: %ld\n", iResult);*/

		iResult = recvfrom(ConnectSocket, recvbuf, recvbuflen, 0, (struct sockaddr*)&add, &t);
		if (iResult > 0){
			char *serverData = new char[iResult + 1];
			strncpy_s(serverData, (iResult + 1) * sizeof(char), recvbuf, iResult);
			cout << serverData << endl;
			delete serverData;
		}
		else if (iResult == 0){
			cout << "Connection closed" << endl;
		}
		else{
			cout << "recv failed with error: " << WSAGetLastError() << endl;
		}

	} while (iResult > 0);

	closesocket(ConnectSocket);
	WSACleanup();

	system("pause");

	return 0;
}