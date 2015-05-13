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
	if (strlen(clientData) < 2 || strlen(clientData) == 0)
	{
		strcpy_s(result, "Not enough data.");
	}
	else
	{
		int startIndex = -1;
		int endIndex = -1;
		bool firstStep = true;
		for (int i = 0; i < strlen(clientData); i++)
		{
			if (clientData[i] == '\"' && firstStep)
			{
				startIndex = i + 1;
				firstStep = false;
			}
			else if (clientData[i] == '\"')
			{
				endIndex = i;
				break;
			}
		}

		if (startIndex == -1 || endIndex == -1)
		{
			strcpy_s(result, "Not enough data");
		}
		else
		{
			strncpy_s(result, clientData + startIndex, endIndex - startIndex);
		}
	}
	return result;
}

int main(void)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;

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
		iResult = recvfrom(ListenSocket, recvbuf, recvbuflen, 0, (struct sockaddr*) &ad, &l);
		if (iResult > 0) {
			dataFromClient = new char[iResult + 1];
			strncpy_s(dataFromClient, (iResult + 1) * sizeof(char), recvbuf, iResult);

			cout << "Receive " << dataFromClient << endl;
			serverResult = DoLab(dataFromClient);

			// Echo the buffer back to the sender
			iSendResult = sendto(ListenSocket, serverResult, strlen(serverResult), 0, (struct sockaddr*) &ad, l);

			cout << "Send to client information..." << endl;

			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ListenSocket);
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
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
	}

	delete serverResult;
	delete dataFromClient;

	// cleanup
	closesocket(ListenSocket);
	WSACleanup();

	system("pause");

	return 0;
}