/// @file:  PacketSender.cpp

#include "stdafx.h"

#include <winsock2.h>

#include "PacketSender.h"
#include "ServerFunc.h"
#include "CommonDef.h"

using namespace WTSEServer;

// CPacketSender

CPacketSender::CPacketSender(int port)
    : m_bHalting(true)
{
    WTSECommon::PORT_NO = port;
}

CPacketSender::~CPacketSender()
{
}

int CPacketSender::Start()
{
    if (!m_bHalting) {
        fprintf(stderr, "Error: sender thread is already active.\n");
        return WTSECommon::SVR_ERR_THREAD_START;
    }

    m_bHalting = false;

    WSADATA wsaData;
    int iRet = WSAStartup(WTSECommon::SOCKET_VERSION, &wsaData);
    if (0 != iRet) {
        fprintf(stderr, "Error: WSAStartup()\n");
        WSACleanup();
        return WTSECommon::SVR_ERR_SOCKET_INIT;
    }

    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == listen_socket) {
        fprintf(stderr, "Error: socket(), WSAGetLastError > %d\n", WSAGetLastError());
        WSACleanup();
        return WTSECommon::SVR_ERR_SOCKET_GEN;
    }

    SOCKADDR_IN local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(WTSECommon::PORT_NO);

    iRet = bind(listen_socket, (sockaddr*)&local, sizeof(local));
    if (SOCKET_ERROR == iRet) {
        fprintf(stderr, "Error: bind(), WSAGetLastError > %d\n", WSAGetLastError());
        WSACleanup();
        return WTSECommon::SVR_ERR_SOCKET_BIND;
    }

    while (!m_bHalting) {
        iRet = listen(listen_socket, 5);
        if (SOCKET_ERROR == iRet) {
            fprintf(stderr, "Error: listen(), WSAGetLastError > %d\n", WSAGetLastError());
            WSACleanup();
            return WTSECommon::SVR_ERR_SOCKET_LISTEN;
        }

        fprintf(stdout, "Status: listening port %d ...\n", WTSECommon::PORT_NO);

        SOCKADDR_IN client;
        int len = sizeof(client);

        SOCKET msgsock = accept(listen_socket, (sockaddr*)&client, &len);
        if (INVALID_SOCKET == msgsock) {
            fprintf(stderr, "Error: accept(), WSAGetLastError > %d\n", WSAGetLastError());
            WSACleanup();
            return WTSECommon::SVR_ERR_SOCKET_ACCEPT;
        }

        fprintf(stdout, "Status: server %s, connected to port %d\n", inet_ntoa(client.sin_addr), htons(client.sin_port));

        LogonSession::InfoList logonSessionList;
        iRet = WTSEServer::EnumerateLogonSession(logonSessionList);
        if (0 != iRet) {
            fprintf(stderr, "Error: getting logon session info > %d\n", iRet);
            WSACleanup();
            return WTSECommon::ERR_UNKNOWN;
        }

        std::wstring data = LogonSession::InfoEntry::SerializeToString(logonSessionList);

        int sentSoFar = 0;
        int needToSend = static_cast<int>(data.length()) * 2;
        const char* addr = (const char*)data.c_str();

        while (sentSoFar < needToSend) {
            iRet = send(msgsock, addr + sentSoFar, needToSend - sentSoFar, 0);
            if (SOCKET_ERROR == iRet) {
                fprintf(stderr, "Error: send(), WSAGetLastError > %d\n", WSAGetLastError());
                break;
            }
            sentSoFar += iRet;
        }

        closesocket(msgsock);
    }

    WSACleanup();

    return WTSECommon::NORMAL_END;
}
