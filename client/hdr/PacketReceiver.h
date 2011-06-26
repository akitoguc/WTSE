/// @file:  PacketReceiver.h

#pragma once

#include "LogonSession.h"

namespace WTSEClient {
    /// @brief  CPacketReceiver
    class CPacketReceiver
    {
        // ctor/dtor
    public:
        CPacketReceiver();
        virtual ~CPacketReceiver();

        // operations
    public:
        BOOL Receive();

        // attributes
    public:
        void SetServer(const CString& cs) { m_csServer = cs; }
        void SetPort(const CString& cs) { m_csPort = cs; }
        const CString& GetServer() const  { return m_csServer; }
        const CString& GetPort() const { return m_csPort; }

    public:
        const LogonSession::InfoList& GetLogonSessionInfo() const { return m_logonSessionInfoList; }

        // implementations
    protected:
        LogonSession::InfoList m_logonSessionInfoList;

        /// @brief  name or id address of the server to connect
        CString m_csServer;
        /// @brief  port number
        CString m_csPort;

    protected:
        /// @brief  data size which is sent/received for a time
        static const int s_bufferSize;
    };
}
