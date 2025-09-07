#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
    // 논블로킹
    
    // 윈속 초기화 (ws2_32 라이브러리 초기화)
    // 관련 정보가 wsaData에 채워짐
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
        return 0;

    u_long on = 1;
    if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
        return 0;

    // 서버 주소 세팅
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = ::htons(7777); // port 7777 설정

    while (true)
    {
        if (::connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            if (::WSAGetLastError() == WSAEWOULDBLOCK)
                continue;

            // 이미 연결된 상태
            if (::WSAGetLastError() == WSAEISCONN)
                break;

            break;
        }
    }

    cout << "Connected to Server" << endl;

    // Send
    char sencBuffer[100] = "Hello World";

    while (true)
    {
        if (::send(clientSocket, sencBuffer, sizeof(sencBuffer), 0) == SOCKET_ERROR)
        {
            if (::WSAGetLastError() == WSAEWOULDBLOCK)
                continue;

            break;
        }

        cout << "Send Data Len = " << sizeof(sencBuffer) << endl;
        
        // Recv
        while (true)
        {
            char recvBuffer[1000];
            int32 arcvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            if (arcvLen == SOCKET_ERROR)
            {
                if (::WSAGetLastError() == WSAEWOULDBLOCK)
                    continue;

                break;
            }
            else if (arcvLen == 0)
            {
                // 연결 끊김
                break;
            }

            cout << "Recv Data Len : " << arcvLen << endl;
            break;
        }

        this_thread::sleep_for(::chrono::seconds(1));
    }
    
    ::closesocket(clientSocket);

    ::WSACleanup();
}
