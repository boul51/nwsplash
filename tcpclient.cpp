#include "tcpclient.h"
#include "command.h"
#include "common.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>

namespace {
const int ConnectTimeout_ms = 2000;
}  // namespace

bool TcpClient::sendCommand(const Command& command, quint16 port)
{
    QTcpSocket socket;
    socket.connectToHost(QHostAddress::LocalHost, port);

    if (!socket.waitForConnected(ConnectTimeout_ms)) {
        qWarning() << Q_FUNC_INFO << "Failed connecting to port " << port;
        return false;
    }

    QByteArray ba = command.toByteArray();

    ba.append(common::frameEndMarker());

    if (ba.size() == 0) {
        qWarning() << Q_FUNC_INFO << "Failed converting command to byte array";
        return false;
    }

    if (socket.write(ba) != ba.size()) {
        qWarning() << Q_FUNC_INFO << "Failed writing command";
        return false;
    }

    socket.flush();

    return true;
}
