/// @file:  WTSEServer.cpp

#include "stdafx.h"
#include <winsock.h>
#include "PacketSender.h"

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc != 2) {
        fprintf(stdout, "Elysium (R) Logon Session Enumeration Server\n"
                        "\n"
                        "Usage: WTSEServer.exe port_number\n"
                        "\n");
        return 0;
    }

    WTSEServer::CPacketSender sender(_wtoi(argv[1]));
    return sender.Start();
}

