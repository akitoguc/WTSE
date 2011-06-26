/// @file:  WTSEServerWorkerThread.cpp

#include "stdafx.h"

#include "WorkerThreadService.h"
#include "PacketSender.h"

using namespace WTSEServer;

// CWorkerThreadService

CWorkerThreadService::CWorkerThreadService()
    : m_hThread(NULL)
{
}

CWorkerThreadService::~CWorkerThreadService()
{
}

BOOL CWorkerThreadService::OnStart(int port)
{
    if (NULL != m_hThread) {
        return FALSE;
    }

    m_packetSender = new CPacketSender(port);
    if (NULL == m_packetSender) {
        return FALSE;
    }

    DWORD threadID;
    m_hThread = ::CreateThread(
            NULL, // default security
            0, // default stack size
            reinterpret_cast<LPTHREAD_START_ROUTINE>(WorkerThreadMain),
            reinterpret_cast<LPVOID>(m_packetSender),
            0, // start soon
            &threadID);
    if (NULL == m_hThread) {
        return FALSE;
    }

    fprintf(stdout, "Status: starting worker thread ...\n");

    return TRUE;
}

BOOL CWorkerThreadService::OnEnd()
{
    if (NULL == m_hThread) {
        return FALSE;
    }

    // order to halt
    m_packetSender->Halt();

    WaitForSingleObject(m_hThread, INFINITE);

    DWORD exitCode;
    GetExitCodeThread(m_hThread, &exitCode);
    if (exitCode == STILL_ACTIVE) {
        return FALSE;
    }

    m_hThread = NULL;
    return TRUE;
}

DWORD CWorkerThreadService::WorkerThreadMain(PVOID pvParam)
{
    CPacketSender* pSender = reinterpret_cast<CPacketSender*>(pvParam);
    if (NULL == pSender) {
        return FALSE;
    }

    DWORD dwErrorCode = pSender->Start();
    ::SetLastError(dwErrorCode);

    return TRUE;
}
