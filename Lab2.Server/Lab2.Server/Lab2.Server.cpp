// Lab1.Server.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

using namespace std;

char* DoLab(char* clientData){
	char result[512];
	if (strlen(clientData) > 2)
	{
		strcpy_s(result, "This is not a number in range from 0 to 10.");
	}
	else
	{
		if (clientData[0] != '0' && clientData[0] != '1' && clientData[0] != '2' && clientData[0] != '3' && clientData[0] != '4' && clientData[0] != '5' && clientData[0] != '6'
			&& clientData[0] != '7' && clientData[0] != '8' && clientData[0] != '9')
		{
			strcpy_s(result, "This is not a number in range from 0 to 10.");
		}
		else
		{
			int number = atoi(clientData);
			switch (number){
			case 0:
				strcpy_s(result, "zero");
				break;
			case 1:
				strcpy_s(result, "one");
				break;
			case 2:
				strcpy_s(result, "two");
				break;
			case 3:
				strcpy_s(result, "three");
				break;
			case 4:
				strcpy_s(result, "four");
				break;
			case 5:
				strcpy_s(result, "five");
				break;
			case 6:
				strcpy_s(result, "six");
				break;
			case 7:
				strcpy_s(result, "seven");
				break;
			case 8:
				strcpy_s(result, "eight");
				break;
			case 9:
				strcpy_s(result, "nine");
				break;
			case 10:
				strcpy_s(result, "ten");
				break;
			}
		}
	}
	return result;
}

int main(void)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	struct sockaddr_in ad;
	ad.sin_port = htons(1024);
	ad.sin_family = AF_INET;
	ad.sin_addr.s_addr = 0;

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, (struct sockaddr*) &ad, sizeof(ad));
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	int l = sizeof(ad);

	char *dataFromClient;
	char* serverResult;
	// Receive until the peer shuts down the connection
	while (true){
		iResult = recvfrom(ClientSocket, recvbuf, recvbuflen, 0, (struct sockaddr*) &ad, &l);
		if (iResult > 0) {
			dataFromClient = new char[iResult + 1];
			strncpy_s(dataFromClient, (iResult + 1) * sizeof(char), recvbuf, iResult);

			cout << "Receive " << dataFromClient << endl;
			serverResult = DoLab(dataFromClient);

			// Echo the buffer back to the sender
			iSendResult = sendto(ClientSocket, serverResult, strlen(serverResult), 0, (struct sockaddr*) &ad, l);

			cout << "Send to client information..." << endl;

			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
		}
		else if (iResult == 0)
		{
			cout << "Connection closing...\n" << endl;
			continue;
		}
		else if (iResult == -1)
		{
			break;
		}
		else  {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	}

	delete serverResult;
	delete dataFromClient;

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	system("pause");

	return 0;
}