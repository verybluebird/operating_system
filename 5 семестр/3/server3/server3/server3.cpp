#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment (lib,"Ws2_32.lib")
#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

using namespace std;
const int N = 100; // Количество пользователей
const int M = 1000; // Размер запроса/ответа

const int maxAmount = 20; //макс число пользователей на сервере
SOCKET servSock;
int curAmount; //текущее число пользователей

//информация о пользователях
SOCKET clSockets[maxAmount];
SOCKADDR_IN clSADDR[maxAmount];
USHORT ports[maxAmount];
char names[maxAmount + 1][N];

DWORD WINAPI chatClient(LPVOID clientSocket)
{
    int retVal;
    char szReq[M];
    SOCKET clientSock;
    int cur;
    int i, j;
    char szResp[M];

    clientSock = *((SOCKET*)clientSocket);

    while (true)
    {
        //получаем сообщение от клиента
        retVal = recv(clientSock, szReq, M, 0);
        if (retVal == SOCKET_ERROR)
        {
            printf("Unable to recv\n");
            closesocket(clientSock);
            printf("Connection closed\n");
            return SOCKET_ERROR;
        }
        else
        {
            if (retVal >= M) retVal = M - 1;
            szReq[retVal] = '\0';
        }

        printf("Data received\n");

        SOCKADDR_IN sin;
        for (i = 0; i < curAmount; i++)
        {
            if (clSockets[i] == clientSock)
                sin = clSADDR[i];
            
        }

        cur = 0;


        //поиск имени пользователя
        while (ports[cur] != sin.sin_port)
             cur++;
         //

        //если пользователь решил выйти
        if (!strcmp(szReq, "s"))
        {
            //оповестить всех других пользователей о выходе пользователя
            szResp[0] = '\0';
            strcat_s(szResp, names[cur]);
            strcat_s(szResp, " leaved chat");

            for (i = 0; i < curAmount; i++)
            {
                if (clSockets[i] != clientSock) retVal = send(clSockets[i], szResp, M, 0);
            }

            if (retVal == SOCKET_ERROR)
            {
                printf("Unable to send\n");
                return SOCKET_ERROR;
            }

            printf("Client disconnected\n");
            closesocket(clientSock);
            printf("Connection closed\n");

            //удаляем информацию о пользователе
            for (j = cur; j < curAmount; j++)
            {
                clSockets[j] = clSockets[j + 1];
                clSADDR[j] = clSADDR[j + 1];
                ports[j] = ports[j + 1];
                strcpy_s(names[j], names[j + 1]);
            }
            clSockets[curAmount - 1] = SOCKET_ERROR;
            curAmount--;
            printf("Current amount of clients: %i\n", curAmount);
            return SOCKET_ERROR;
        }

        //если принятое сообщение - не пустая строка
        if (szReq[0] != '\0')
        {
            //выводим имя пользователя и его сообщение
            printf("%s: %s\n", names[cur], szReq);
            szResp[0] = '\0';
            strcat_s(szResp, names[cur]);
            strcat_s(szResp, ": ");
            strcat_s(szResp, szReq);

            printf("Sending response from server\n");

            //и отсылаем всем пользователям, кроме него самого
            for (i = 0; i < curAmount; i++)
            {
                if (clSockets[i] != clientSock) 
                    retVal = send(clSockets[i], szResp, M, 0);
                    
            }

            if (retVal == SOCKET_ERROR)
            {
                printf("Unable to send\n");

                return SOCKET_ERROR;
            }
        }
    }
}


int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    WORD sockVer;
    WSADATA wsaData;
    int retVal;
    int i;
    curAmount = 0;
    sockVer = MAKEWORD(2, 2);
    WSAStartup(sockVer, &wsaData);
    //Создаем сокет
    servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (servSock == INVALID_SOCKET)
    {
        printf("Unable to create socket\n");
        WSACleanup();
        system("pause");
        return SOCKET_ERROR;
    }

    for (i = 0; i < maxAmount; i++)
    {
        clSockets[i] = SOCKET_ERROR;
    }

    SOCKADDR_IN sin;

    sin.sin_family = PF_INET;
    sin.sin_port = htons(2011);
    sin.sin_addr.s_addr = INADDR_ANY;

    retVal = bind(servSock, (LPSOCKADDR)&sin, sizeof(sin));
    if (retVal == SOCKET_ERROR)
    {
        printf("Unable to bind\n");
        WSACleanup();
        system("pause");
        return SOCKET_ERROR;
    }


    char host[N];
    char HostName[1024]; //создаем буфер для имени хоста
    if (!gethostname(HostName, 1024)) //получаем имя хоста
    {
        if (LPHOSTENT lphost = gethostbyname(HostName)) //получаем IP сервера
            strcpy(host, inet_ntoa(*((in_addr*)lphost->h_addr_list[0])));
        //преобразуем переменную типа LPIN_ADDR в DWORD
    }
    printf("Server is working at %s, port %d\n", host, htons(sin.sin_port));

    while (true)
    {
        //Пытаемся начать слушать сокет
        retVal = listen(servSock, 10);
        if (retVal == SOCKET_ERROR)
        {
            printf("Unable to listen\n");
            WSACleanup();
            system("pause");
            return SOCKET_ERROR;
        }

        //Ждем клиента
        SOCKET clientSock;
        SOCKADDR_IN from;
        int fromlen = sizeof(from);
        clientSock = accept(servSock, (struct sockaddr*)&from, &fromlen);
        if (clientSock == INVALID_SOCKET)
        {
            printf("Unable to accept\n");
            WSACleanup();
            system("pause");
            return SOCKET_ERROR;
        }

        //установлено новое соединение
        printf("New client from %s, port %d\n", inet_ntoa(from.sin_addr),
            htons(from.sin_port));
        printf("Current amount of clients: %i\n", curAmount + 1);
        char NewClient[M];

        //получаем имя нового клиента
        retVal = recv(clientSock, names[curAmount], N, 0);
        //Пытаемся получить данные от клиента
        if (retVal == SOCKET_ERROR)
        {
            printf("Unable to recv\n");
            system("pause");
            return SOCKET_ERROR;
        }

        //если имя - команда на закрытие сервера
        if (!strcmp(names[curAmount], "stop"))
        {
            char* szResp = (char*)"Server closed.";
            //посылаем сообщение о закрытии всем клиентам и закрываем сокеты  
            for (i = 0; i < curAmount; i++)
            {
                retVal = send(clSockets[i], szResp, M, 0);
                closesocket(clSockets[i]);
            }
            retVal = send(clientSock, szResp, M, 0);
            closesocket(clientSock);
            break;
        }
        else
        {
            //если не достиг максимум пользователей
            if (curAmount < maxAmount)
            {
                //сохраняем и выводим информацию о текущем пользователе
                ports[curAmount] = from.sin_port;
                clSockets[curAmount] = clientSock;
                clSADDR[curAmount] = from;

                NewClient[0] = '\0';
                strcat_s(NewClient, names[curAmount]);
                strcat_s(NewClient, " join to chat! IP: ");
                strcat_s(NewClient, inet_ntoa(from.sin_addr));

                printf_s("%s\n", NewClient);
                curAmount++;

                //рассылаем всем клиентам сообщение о новом пользователе
                for (i = 0; i < curAmount; i++)
                    if (clSockets[i] != clientSock) retVal = send(clSockets[i], NewClient, M, 0);
            }
            else
            {
                //если достигнут максимум, отправляем пользователю сообщение о достижении максимума и закрываем сокет
                printf("Maximum amount of clients\n");
                retVal = send(clientSock, "FULL.", N, 0);
                closesocket(clientSock);
                printf("Connection closed\n");
            }

            DWORD threadName;
            CreateThread(NULL, NULL, chatClient, &clientSock, NULL, &threadName);
            
        }
    }


    closesocket(servSock);
    WSACleanup();
    return 0;
}
