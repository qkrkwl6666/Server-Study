#include "pch.h"
#include <iostream>      
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSAData wsaData;

    if (::WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        return 0;

    // AF_INET (IPv4), SOCK_STREAM (TCP), 
    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
        return 0;

    u_long on = 1;
    if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
        return 0;

    // 서버 주소 세팅
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // 모든 주소 허용
    serverAddr.sin_port = ::htons(7777); // port 7777 설정

    // 주소 설정
    if (::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        return 0;

    // 연결 대기
    if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        return 0;

    cout << "Accept" << endl;

    SOCKADDR_IN clientAddr;
    int32 addrLen = sizeof(clientAddr);

    while (true)
    {
        SOCKET clientSocket = ::accept(listenSocket, (sockaddr*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            if (::WSAGetLastError() == WSAEWOULDBLOCK)
                continue;

            // Error
            break;
        }

        std::cout << "Client Connected" << endl;

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

            // Send
            while (true)
            {
                if (::send(clientSocket, recvBuffer, arcvLen, 0) == SOCKET_ERROR)
                {
                    if (::WSAGetLastError() == WSAEWOULDBLOCK)
                        continue;

                    break;
                }

                cout << "Send Data Len = " << arcvLen << endl;

                this_thread::sleep_for(::chrono::seconds(1));
            }
        }
        
    }
    
    // 윈속 종료
    ::WSACleanup();
    
}
