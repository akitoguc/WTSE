/// @file:  EnumerateLogonSession.cpp

#include "stdafx.h"

#include <wtsapi32.h>

#include "ServerFunc.h"
#include "CommonDef.h"

/// @brief  collect logon-session informations
/// @return 0: normal end, >0: error
int WTSEServer::EnumerateLogonSession(LogonSession::InfoList& logonSessionList)
{
    DWORD nSession = 0;
    PWTS_SESSION_INFO pSessionInfo = NULL;
    BOOL bRet = WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &nSession);
    if (bRet == FALSE) {
        fprintf(stderr, "Error: WTSEnumerateSessions, GetLastError > %d\n", GetLastError());
        return WTSECommon::ERR_UNKNOWN;
    }

    for (DWORD i = 0; i < nSession; ++i) {
        DWORD sessionId = pSessionInfo[i].SessionId;
        if (0xFFFFFFFF == sessionId) {
            // invalid sessions
            continue;
        }

        LogonSession::InfoEntry info;

        // session state
        info.SetConnectionState(pSessionInfo[i].State);

        LPTSTR pBuffer = NULL;
        DWORD bytesReturned = 0;

        // user name
        bRet = WTSQuerySessionInformation(
                WTS_CURRENT_SERVER_HANDLE, sessionId, WTSUserName, &pBuffer, &bytesReturned);
        if (bRet) {
            info.SetUserName(pBuffer);
            WTSFreeMemory(pBuffer);
        }
        else {
            fprintf(stderr, "Error: WTSQuerySessionInformation, WTSUserName, GetLastError > %d\n", GetLastError());
        }

        // domain name
        bRet = WTSQuerySessionInformation(
                WTS_CURRENT_SERVER_HANDLE, sessionId, WTSDomainName, &pBuffer, &bytesReturned);
        if (bRet) {
            info.SetDomain(pBuffer);
            WTSFreeMemory(pBuffer);
        }
        else {
            fprintf(stderr, "Error: WTSQuerySessionInformation, WTSDomainName, GetLastError > %d\n", GetLastError());
        }

        // win station name
        bRet = WTSQuerySessionInformation(
                WTS_CURRENT_SERVER_HANDLE, sessionId, WTSWinStationName, &pBuffer, &bytesReturned);
        if (bRet) {
            info.SetWinStationName(pBuffer);
            WTSFreeMemory(pBuffer);
        }
        else {
            fprintf(stderr, "Error: WTSQuerySessionInformation, WTSWinStationName, GetLastError > %d\n", GetLastError());
        }

        // client name
        bRet = WTSQuerySessionInformation(
                WTS_CURRENT_SERVER_HANDLE, sessionId, WTSClientName, &pBuffer, &bytesReturned);
        if (bRet) {
            info.SetClientName(pBuffer);
            WTSFreeMemory(pBuffer);
        }
        else {
            fprintf(stderr, "Error: WTSQuerySessionInformation, WTSClientName, GetLastError > %d\n", GetLastError());
        }

        // logon time
        bRet = WTSQuerySessionInformation(
                WTS_CURRENT_SERVER_HANDLE, sessionId, WTSLogonTime, &pBuffer, &bytesReturned);
        if (bRet && bytesReturned == sizeof(LARGE_INTEGER)) {
            info.SetLogonTime(*reinterpret_cast<LARGE_INTEGER*>(pBuffer));
            WTSFreeMemory(pBuffer);
        }
        else {
            bRet = WTSQuerySessionInformation(
                    WTS_CURRENT_SERVER_HANDLE, sessionId, WTSSessionInfo, &pBuffer, &bytesReturned);
            if (bRet) {
                WTSINFO* wtsInfo = reinterpret_cast<WTSINFO*>(pBuffer);
                info.SetLogonTime(wtsInfo->LogonTime);
                WTSFreeMemory(pBuffer);
            }
            else {
                fprintf(stderr, "Error: WTSQuerySessionInformation, WTSLogonTime|WTSSessionInfo, GetLastError > %d\n", GetLastError());
            }
        }

        logonSessionList.push_back(info);
    }

    WTSFreeMemory(pSessionInfo);

    return WTSECommon::NORMAL_END;
}
