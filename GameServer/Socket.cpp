#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <mutex>
#include <Windows.h>
#include <future>
#include "ThreadManager.h"
#include <WinSock2.h>
#include <mswsock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();

        cout << "listenErrorCode : " << errCode << endl;
        return 0;
    }

    // ���� �ּҴ�? (Ip �ּ� + Port) -> XX ����Ʈ YY ȸ��
    SOCKADDR_IN serverAddr; // IPv4
    ::memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htons(INADDR_ANY); // �ϰ� �˾Ƽ� ����
    serverAddr.sin_port = ::htons(7777);

    // �ȳ��� �� ����! �Ĵ��� ��ǥ ��ȣ
    if (::bind(listenSocket, (sockaddr*)&serverAddr,
        sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "bindErrorCode : " << errCode << endl;
        return 0;
    }

    // ���� ����!
    if (::listen(listenSocket, 10) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "ListenErrorCode : " << errCode << endl;
        return 0;
    }

    // -----------------------------------------------

    while (true)
    {
        SOCKADDR_IN clientAddr; // IPv4
        ::memset(&clientAddr, 0, sizeof(clientAddr));
        int addrLen = sizeof(clientAddr);
        SOCKET clientSocket = 
            ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

        if (clientSocket == INVALID_SOCKET)
        {
            int32 errCode = ::WSAGetLastError();
            cout << "AcceptErrorCode : " << errCode << endl;
            return 0;
        }

        // �մ� ����
        char ipAddress[16];

        ::inet_ntop(AF_INET, &clientAddr.sin_addr,
            ipAddress, sizeof(ipAddress));

        cout << "Client Connected IP = " << ipAddress << endl;

        // TODO 

    }

    // ���� ����
    ::WSACleanup();
}