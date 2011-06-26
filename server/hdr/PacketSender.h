/// @file:  PacketSender.h

#pragma once

#include "WTSEServer.h"

namespace WTSEServer {
    /// @brief  CPacketSender
    class WTSESERVER_API CPacketSender
    {
        // ctor/dtor
    public:
        CPacketSender(int port);
        virtual ~CPacketSender();

        // operations
    public:
        /// @brief  start operation (repeatedly)
        int Start();
        /// @brief  order to cease operation
        void Halt() { m_bHalting = true; }

        // implementations
    private:
        bool m_bHalting;
    };
}
