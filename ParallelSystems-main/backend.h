#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include "serverStuff.h"
#include "lamport_clock.h"

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);

signals:
    void smbConnected(QString msg);
    void smbDisconnected(QString msg);
    void newMessage(QString msg);
    void newConnectionToServer(QString msg);
    void statusChanged(QString newStatus);
    void someMessage(QString msg);

public slots:
    QString stopClicked();
    QString startClicked();
    QString testClicked();
    void connect2server();
    void setStatus(bool newStatus);
    void receivedSomething(QString msg);

    int getLT();

    void set_timer();
    void set_timer(int type,int time);
    void perform_action();

    void sendClicked(QString msg);
    qint64 sendToClient(int socketid, const QString &str);
    void smbConnectedToServer(QString msg);
    void smbDisconnectedFromServer(QString msg);
    void gotNewMesssage(QString msg);


private:
    ServerStuff *server;
    QString MyName = "Kovalov";
    QTimer *timer = new QTimer(this);
    int action=0;
    LamportClock *LampClock;
    int local_value=0;
};

#endif // BACKEND_H
