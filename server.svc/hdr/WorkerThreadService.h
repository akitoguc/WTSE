/// @file:  CWorkerThreadService.h

#pragma once

//#include <windows.h>

namespace WTSEServer {

    class CPacketSender;

    /// @brief  CWorkerThreadService
    class CWorkerThreadService
    {
        // ctor/dtor
    public:
        CWorkerThreadService();
        virtual ~CWorkerThreadService();

        // operations
    public:
        BOOL OnStart(int port);
        BOOL OnEnd();

        // implementations
    private:
        /// @brief  thread of server service
        HANDLE m_hThread;
        /// @brief  instance of server service
        CPacketSender* m_packetSender;

        static DWORD WorkerThreadMain(PVOID pvParam);
    };
}
