#ifndef SERVERSTUFF_H
#define SERVERSTUFF_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QList>
#include <QTimer>
#include "lamport_clock.h"

class ServerStuff : public QObject
{
    Q_OBJECT

public:
    ServerStuff();
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    QList<QTcpSocket *> getClients();
    bool getStatus();

public slots:
    virtual void newConnection();
    void readClient();
    void gotDisconnection();
    qint64 sendToClient(int socketid, const QString &str);
    qint64 sendToClient(QTcpSocket *socket, const QString &str);
    void closeConnection();
    void connect2host();
    void connectionTimeout();
    void readyRead();
    void connected();

signals:
    void gotNewMesssage(QString msg);
    void newConnectionToServer(QString msg);
    void smbDisconnected(QString msg);
    void statusChanged(bool);
    void hasReadSome(QString msg);

public:
    quint16 m_nNextBlockSize;
    QList<QTcpSocket*> clients;
    QList<QString> clientsName;
    QString host;
    QString MyName = "Kovalov";
    int port;
    bool status;
    QTimer *timeoutTimer;
    LamportClock *LampClock;
};

#endif // SERVERSTUFF_H
