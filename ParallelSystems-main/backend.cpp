#include "backend.h"
#include "serverStuff.h"
#include "QRandomGenerator"

Backend::Backend(QObject *parent) : QObject(parent)
{
    server = new ServerStuff();
    connect(server, &ServerStuff::gotNewMesssage, this, &Backend::gotNewMesssage);
    connect(server, &ServerStuff::newConnectionToServer, this, &Backend::smbConnectedToServer);
    //connect(server->tcpServer, &QTcpServer::newConnection, this, &Backend::smbConnectedToServer);
    connect(server, &ServerStuff::smbDisconnected, this, &Backend::smbDisconnectedFromServer);

    connect(server, &ServerStuff::hasReadSome, this, &Backend::receivedSomething);
    connect(server, &ServerStuff::statusChanged, this, &Backend::setStatus);
    // FIXME change this connection to the new syntax
    connect(server->tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(gotError(QAbstractSocket::SocketError)));
    LampClock = new LamportClock();

}

void Backend::setStatus(bool newStatus)
{
    //qDebug() << "new status is:" << newStatus;
    if (newStatus)
        { emit statusChanged("CONNECTED"); }
    else
        { emit statusChanged("DISCONNECTED"); }
}

void Backend::set_timer(int type,int time)
{
    action=type;
    QTimer::singleShot(time*1000, this, &Backend::perform_action);
}

void Backend::set_timer()
{
    action=0;
    QTimer::singleShot(1*1000, this, &Backend::perform_action);
}

void Backend::perform_action()
{
    //qDebug("did something");

    LampClock->local_event();
    if(action==0){
        sendToClient(0,"Privet Olexey: " + QString::number(LampClock->get_time()));
    }

    if(action==1){
        sendClicked("Privet Darina: " + QString::number(LampClock->get_time()));
    }
    if(action==2){
        local_value+=1;
        gotNewMesssage("New local value = "+ QString::number(local_value));
    }
    set_timer(QRandomGenerator::global()->generate()%3,QRandomGenerator::global()->generate()%10+1);
}
void Backend::receivedSomething(QString msg)
{


    emit someMessage(msg);
}

QString Backend::startClicked()
{
        server->tcpServer->listen(QHostAddress::Any, 27015);


        QByteArray arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);
        //out.setVersion(QDataStream::Qt_5_10);
        out << quint16(0) << "Name:" + MyName;

        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));

        server->tcpSocket->write(arrBlock);

        connect(server->tcpServer, &QTcpServer::newConnection, server, &ServerStuff::newConnection);
        return "Server started, port is openned";

}
void Backend::connect2server()
{
        server->connect2host();

}

QString Backend::stopClicked()
{
    if(server->tcpServer->isListening())
    {
        disconnect(server->tcpServer, &QTcpServer::newConnection, server, &ServerStuff::newConnection);

        QList<QTcpSocket *> clients = server->getClients();
        for(int i = 0; i < clients.count(); i++)
        {
            //server->sendToClient(clients.at(i), "Connection closed");
            server->sendToClient(clients.at(i), "0");
        }

        server->tcpServer->close();
        return "Server stopped, post is closed";
    }
    else
    {
        return "Error! Server was not running";
    }
}

QString Backend::testClicked()
{
    if(server->tcpServer->isListening())
    {
        return QString("%1 %2")
                .arg("Server is listening, number of connected clients:")
                .arg(QString::number(server->getClients().count()));
    }
    else
    {
        return QString("%1 %2")
                .arg("Server is not listening, number of connected clients:")
                .arg(QString::number(server->getClients().count()));
    }
}

void Backend::smbConnectedToServer(QString msg)
{
    emit smbConnected(msg);
}

void Backend::smbDisconnectedFromServer(QString msg)
{
    emit smbDisconnected(msg);
}

void Backend::gotNewMesssage(QString msg)
{
    LampClock->receive_event(msg.right(msg.length()-15).toInt());

    emit newMessage(msg);
}

int Backend::getLT()
{
    return LampClock->get_time();
}

qint64 Backend::sendToClient(int socketid, const QString &str)
{
    if(server->clients.length()>0){
    server->sendToClient(0,str);
    }
}

void Backend::sendClicked(QString msg)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    //out.setVersion(QDataStream::Qt_5_10);
    if(msg != "-Name:" + MyName){
    out << quint16(0) << MyName + ": " + msg;
    }
    else{
        out << quint16(0) << msg;
    }
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    server->tcpSocket->write(arrBlock);
}


