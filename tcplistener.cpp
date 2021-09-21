#include "tcplistener.h"
#include "common.h"

#include <QDebug>

TcpListener::TcpListener(QObject *parent) : QObject(parent)
{
    connect(&m_server, &QTcpServer::newConnection, this, &TcpListener::onServerNewConnection);
}

bool TcpListener::listen(quint16 port)
{
    if (!m_server.listen(QHostAddress::LocalHost, port)) {
        qWarning() << Q_FUNC_INFO << "Failed listening on port " << port;
        return false;
    }

    return true;
}

void TcpListener::onServerNewConnection()
{
    qDebug() << "Got a new connection";

    while (QTcpSocket* socket = m_server.nextPendingConnection()) {
        m_sockets.append(socket);
        connect(socket, &QTcpSocket::readyRead, this, &TcpListener::onSocketReadyRead);
        connect(socket, &QTcpSocket::aboutToClose, this, &TcpListener::onSocketAboutToClose);
    }
}

void TcpListener::onSocketReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

    if (!socket) {
        qWarning() << Q_FUNC_INFO << "Failed casting sender to QTcpSocket";
        return;
    }

    auto data = socket->readAll();

    if (!data.endsWith(common::frameEndMarker())) {
        qWarning() << Q_FUNC_INFO << "Received incomplete command";
        return;
    }

    data.chop(common::frameEndMarker().size());

    Command command;

    if (!command.parse(data))
        return;

    emit commandReceived(&command);
}

void TcpListener::onSocketAboutToClose()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

    if (!socket) {
        qWarning() << Q_FUNC_INFO << "Failed casting sender to QTcpSocket";
        return;
    }

    m_sockets.removeAll(socket);
}
