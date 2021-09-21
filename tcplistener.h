#ifndef TCPLISTENER_H
#define TCPLISTENER_H

#include "command.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>

class TcpListener : public QObject
{
    Q_OBJECT

public:
    explicit TcpListener(QObject *parent = nullptr);
    bool listen(quint16 port);

signals:
    void stringReceived(QString s);
    void commandReceived(Command* command);
    void splashCommandChanged();

private slots:
    void onServerNewConnection();
    void onSocketReadyRead();
    void onSocketAboutToClose();

private:
    QTcpServer m_server;
    QVector<QTcpSocket*> m_sockets;
};

#endif // TCPLISTENER_H
