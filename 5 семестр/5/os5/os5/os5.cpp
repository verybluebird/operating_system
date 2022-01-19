#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;
#include <iostream>

#include <stdio.h>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <locale.h>
#include <Windows.h>
#include <sys/types.h>
      

int main()
{
    setlocale(LC_ALL, "Russian");
    WORD ver = MAKEWORD(2, 2);
    WSADATA wsaData;
    DWORD retVal = 0, replySize = 0;
    HANDLE ICMPlog; // дескриптор протокола ICMP
    LPVOID replyBuf = NULL;
    UINT hostInAddr = 0;
    char SendAdr[256];
    char hostname[256];
    int sd, err;

    WSAStartup(ver, &wsaData);
    printf("Введите имя домена: ");
    scanf("%s", SendAdr);
    struct hostent* hostAddr;
    hostAddr = gethostbyname(SendAdr);
    
    if (hostAddr)
    {
        cout << "Обмен пакетами с " << SendAdr  << "\n";
        for (int i = 0; i < 4; i++)
        {
            strcpy(hostname, inet_ntoa(*((in_addr*)hostAddr->h_addr_list[0])));
            hostInAddr = inet_addr(hostname); // IPAddr формат
            ICMPlog = IcmpCreateFile(); // создаем лог
            replySize = sizeof(ICMP_ECHO_REPLY); // размер возвращаемого пакета
            replyBuf = (void*)malloc(replySize);
            retVal = IcmpSendEcho(ICMPlog, hostInAddr, NULL, NULL, NULL, replyBuf, replySize, 1000);
            if (retVal)
            {
                PICMP_ECHO_REPLY reply = (PICMP_ECHO_REPLY)replyBuf;
                struct in_addr replyAddr;
                replyAddr.S_un.S_addr = reply->Address;
                time_t timei = reply->RoundTripTime;
                char* receivedIP = inet_ntoa(replyAddr);
               
                cout << "Ответ от: " << receivedIP;
                cout << " время: " << timei << "мс" << '\n';
                
            }
            else
                printf("Во время выполнения запроса произошла ошибка");
        }
       
    }
    else
        printf("Данный домен не существует");
}
