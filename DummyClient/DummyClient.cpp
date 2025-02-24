﻿#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <mswsock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
    // 윈속 초기화 (ws2_32 라이브러리 초기화)
    // 관련 정보가 wsaData에 채워짐
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    // ad(Address Family) AF_INET = IPv4, AF_INET6 = IPv6
    // type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
    // protocol : 0
    // return : descriptor
    SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();

        cout << "socketErrorCode : " << errCode << endl;
        return 0;
    }

    // 연결할 목적지는? (Ip 주소 + Port) -> XX 아파트 YY 호
    SOCKADDR_IN serverAddr; // IPv4
    ::memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    //serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = ::htons(7777);

    if (::connect(clientSocket, (sockaddr*)&serverAddr,
        sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "socketErrorCode : " << errCode << endl;
        return 0;
    }

    // 연결 성공! 이제부터 데이터 송수신 가능!

    cout << "Connected To Server!" << endl;

    while (true)
    {
        // TODO 
        this_thread::sleep_for(1s);
    }

    // ---------------------------------------
    // 소켓 리소스 반환
    closesocket(clientSocket);
    // 윈속 종료
    ::WSACleanup();
}
