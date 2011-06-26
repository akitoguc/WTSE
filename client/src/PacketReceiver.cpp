/// @file:  PacketReceiver.cpp

#include "stdafx.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#include "PacketReceiver.h"
#include "CommonDef.h"

using namespace WTSEClient;

// CPacketReceiver

/// @brief  data size which is sent/received for a time
const int CPacketReceiver::s_bufferSize = 32;

CPacketReceiver::CPacketReceiver()
{
}

CPacketReceiver::~CPacketReceiver()
{
}

BOOL CPacketReceiver::Receive()
{
    WSADATA wsaData;
    int iRet = WSAStartup(WTSECommon::SOCKET_VERSION, &wsaData);
    if (0 != iRet) {
        fprintf(stderr, "Error: WSAStartup()\n");
        WSACleanup();
        return FALSE;
    }

    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == listen_socket) {
        fprintf(stderr, "Error: socket(), WSAGetLastError > %d\n", WSAGetLastError());
        WSACleanup();
        return FALSE;
    }

    ADDRINFOW hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;

    ADDRINFOW* addrInfo;
    iRet = GetAddrInfo(m_csServer, NULL, &hints, &addrInfo);
    if (SOCKET_ERROR == iRet) {
        fprintf(stderr, "Error: GetAddrInfoW(), returned > %d\n", iRet);
        WSACleanup();
        return FALSE;
    }

    SOCKADDR_IN server;
    server.sin_family = AF_INET;
    server.sin_port = htons(_wtoi(m_csPort));
    server.sin_addr.S_un.S_addr = (reinterpret_cast<SOCKADDR_IN*>(addrInfo->ai_addr))->sin_addr.S_un.S_addr;

    iRet = connect(listen_socket, (sockaddr *)&server, sizeof(server));
    if (SOCKET_ERROR == iRet) {
        fprintf(stderr, "Error: connect(), WSAGetLastError > %d\n", WSAGetLastError());
        WSACleanup();
        return FALSE;
    }

    std::wstring receivedData;

    while (true) {
        char buffer[s_bufferSize];

        iRet = recv(listen_socket, buffer, s_bufferSize, 0);
        if (SOCKET_ERROR == iRet) {
            fprintf(stderr, "Error: recv(), WSAGetLastError > %d\n", WSAGetLastError());
            WSACleanup();
            return FALSE;
        }
        if (0 == iRet) {
            break;
        }

        ASSERT(iRet % 2 == 0);
        receivedData += std::wstring((wchar_t*)&buffer[0], (wchar_t*)&buffer[iRet]);
    }

    m_logonSessionInfoList = LogonSession::InfoEntry::RetrieveFromString(receivedData);

    FreeAddrInfo(addrInfo);
    WSACleanup();

    return TRUE;
}

