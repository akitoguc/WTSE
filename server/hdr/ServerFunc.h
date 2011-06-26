/// @file:  ServerFunc.h

#pragma once

#include "WTSEServer.h"
#include "LogonSession.h"

namespace WTSEServer {
    /// @brief  collect logon-session info
    /// @return 0: normal end, >0: error
    WTSESERVER_API int EnumerateLogonSession(LogonSession::InfoList& logonSessionList);
}
