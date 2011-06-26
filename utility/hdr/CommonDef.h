#pragma once

#include "WTSEUtility.h"

namespace WTSECommon {
    /// @brief  Error codes
    const int NORMAL_END    = 0;
    const int ERR_UNKNOWN   = 1;

    /// @brief  Server error codes
    const int SVR_ERR_SOCKET_INIT   = 0x01 << 0x1;
    const int SVR_ERR_SOCKET_GEN    = 0x02 << 0x1;
    const int SVR_ERR_SOCKET_BIND   = 0x03 << 0x1;
    const int SVR_ERR_SOCKET_LISTEN = 0x04 << 0x1;
    const int SVR_ERR_SOCKET_ACCEPT = 0x05 << 0x1;
    const int SVR_ERR_THREAD_START  = 0x11 << 0x1;

    /// @brief  Client error codes

    /// @breif  Port number
    extern WTSEUTILITY_API int PORT_NO; // = 3333;

    /// @brief  socket version
    const WORD SOCKET_VERSION = MAKEWORD(2, 0);

    /// @brief  security logon type
    const wchar_t* const SECURY_LOGON_TYPE_STR[] = {
        L"Unknown",
        L"Unknown",
        L"Interactive", // 2
        L"Network",
        L"Batch",
        L"Service",
        L"Proxy",
        L"Unlock",
        L"NetworkCleartext",
        L"NewCredentials",
        L"RemoteInteractive",
        L"CachedInteractive",
        L"CachedRemoteInteractive",
        L"CachedUnlock",
    };
    const wchar_t* const WTS_CONNECTIONSTATE_CLASS_STR[] = {
        L"Active",
        L"Connected",
        L"ConnectedQuery",
        L"Shadow",
        L"Disconnected",
        L"Idle",
        L"Listen",
        L"Reset",
        L"Down",
        L"Init",
    };
}
