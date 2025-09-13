#include "pch.h"
#include <iostream>      
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

const int32 BUFSIZE = 100;

struct Session
{
    SOCKET socket = INVALID_SOCKET;
    char recvBuffer[BUFSIZE];
    int32 recvBytes = 0;
    int32 sendBytes = 0;
};

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

    std::vector<Session> sessions;
    sessions.reserve(100);
    
    fd_set reads;
    fd_set writes;

    while (true)
    {
         // 소켓 셋 초기화
        FD_ZERO(&reads);
        FD_ZERO(&writes);

        // ListenSocket 등록
        FD_SET(listenSocket, &reads);

        for (Session& s : sessions)
        {
            if (s.recvBytes <= s.sendBytes)
                FD_SET(s.socket, &reads);
            else
                FD_SET(s.socket, &writes);
        }
        
        int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);

        if (retVal == SOCKET_ERROR)
            break;

        // Listener 소켓 체크
        if (FD_ISSET(listenSocket, &reads))
        {
            SOCKADDR_IN clientAddr;
            int addrLen = sizeof(clientAddr);
            SOCKET clientSocket = ::accept(listenSocket, (sockaddr*)&clientAddr, &addrLen);
            if (clientSocket != INVALID_SOCKET)
            {
                cout << "Client Connected" << endl;
                sessions.push_back(Session{ clientSocket });
            }
        }

        // 나머지 소켓 체크
        for (Session& s : sessions)
        {
            // Read
            if (FD_ISSET(s.socket, &reads))
            {
                int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
                if (recvLen <= 0)
                {
                    // TODO : sessions 제거
                    continue;
                }

                s.recvBytes = recvLen;
            }

            // Write
            if (FD_ISSET(s.socket, &writes))
            {
                int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
                if (sendLen == INVALID_SOCKET)
                {
                    continue;
                }

                s.sendBytes += sendLen;

                if (s.sendBytes == s.recvBytes)
                {
                    s.sendBytes = 0;
                    s.recvBytes = 0;
                }
            }
        }
        
        
    }
    
    

    // 윈속 종료
    ::WSACleanup();
    
    return 0;
}
