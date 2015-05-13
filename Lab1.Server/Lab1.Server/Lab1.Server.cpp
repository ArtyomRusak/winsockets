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

  struct addrinfo *result = NULL;
  struct addrinfo hints;

  int iSendResult;
  char recvbuf[DEFAULT_BUFLEN];
  int recvbuflen = DEFAULT_BUFLEN;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    printf("WSAStartup failed with error: %d\n", iResult);
    return 1;
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the server address and port
  iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
  if (iResult != 0) {
    printf("getaddrinfo failed with error: %d\n", iResult);
    WSACleanup();
    return 1;
  }

  // Create a SOCKET for connecting to server
  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (ListenSocket == INVALID_SOCKET) {
    printf("socket failed with error: %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return 1;
  }

  // Setup the TCP listening socket
  iResult = bind(ListenSocket, result->ai_addr, (int) result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    printf("bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(result);
    closesocket(ListenSocket);
    WSACleanup();
    return 1;
  }

  freeaddrinfo(result);

  iResult = listen(ListenSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) {
    printf("listen failed with error: %d\n", WSAGetLastError());
    closesocket(ListenSocket);
    WSACleanup();
    return 1;
  }

  // Accept a client socket
  ClientSocket = accept(ListenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
    printf("accept failed with error: %d\n", WSAGetLastError());
    closesocket(ListenSocket);
    WSACleanup();
    return 1;
  }

  // No longer need server socket
  closesocket(ListenSocket);

  char *dataFromClient;
  char* serverResult;
  // Receive until the peer shuts down the connection
  do {
    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
      dataFromClient = new char[iResult + 1];
      strncpy_s(dataFromClient, (iResult + 1) * sizeof(char), recvbuf, iResult);

      cout << "Receive " << dataFromClient << endl;
      serverResult = DoLab(dataFromClient);

      // Echo the buffer back to the sender
      iSendResult = send(ClientSocket, serverResult, strlen(serverResult), 0);
      
      cout << "Send to client information..." << endl;

      if (iSendResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
      }
    }
    else if (iResult == 0)
      printf("Connection closing...\n");
    else  {
      printf("recv failed with error: %d\n", WSAGetLastError());
      closesocket(ClientSocket);
      WSACleanup();
      return 1;
    }

  } while (iResult > 0);

  // shutdown the connection since we're done
  iResult = shutdown(ClientSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
    printf("shutdown failed with error: %d\n", WSAGetLastError());
    closesocket(ClientSocket);
    WSACleanup();
    return 1;
  }

  delete serverResult;
  delete dataFromClient;

  // cleanup
  closesocket(ClientSocket);
  WSACleanup();

  system("pause");

  return 0;
}