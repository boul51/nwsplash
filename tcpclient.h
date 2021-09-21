#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QtGlobal>

class Command;

class TcpClient
{
public:
    static bool sendCommand(const Command& command, quint16 port);
};

#endif // TCPCLIENT_H
