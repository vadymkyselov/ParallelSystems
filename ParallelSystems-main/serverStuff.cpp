#include "serverStuff.h"

ServerStuff::ServerStuff() : QObject(), m_nNextBlockSize(0)
{
    tcpServer = new QTcpServer();


    status = false;
    tcpSocket = new QTcpSocket();

    host = "109.87.102.73";
    port = 27017;

    timeoutTimer = new QTimer();
    timeoutTimer->setSingleShot(true);
    connect(timeoutTimer, &QTimer::timeout, this, &ServerStuff::connectionTimeout);

    connect(tcpSocket, &QTcpSocket::disconnected, this, &ServerStuff::closeConnection);

}

QList<QTcpSocket *> ServerStuff::getClients()
{
    return clients;
}

void ServerStuff::connect2host()
{
    timeoutTimer->start(3000);

    tcpSocket->connectToHost(host, port);
    connect(tcpSocket, &QTcpSocket::connected, this, &ServerStuff::connected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &ServerStuff::readyRead);
}

void ServerStuff::connectionTimeout()
{
    //qDebug() << tcpSocket->state();
    if(tcpSocket->state() == QAbstractSocket::ConnectingState)
    {
        tcpSocket->abort();
        emit tcpSocket->error(QAbstractSocket::SocketTimeoutError);
    }
}

void ServerStuff::connected()
{
    status = true;
    emit statusChanged(status);
}

bool ServerStuff::getStatus() {return status;}

void ServerStuff::readyRead()
{
    QDataStream in(tcpSocket);
    //in.setVersion(QDataStream::Qt_5_10);
    for (;;)
    {
        if (!m_nNextBlockSize)
        {
            if (tcpSocket->bytesAvailable() < sizeof(quint16)) { break; }
            in >> m_nNextBlockSize;
        }

        if (tcpSocket->bytesAvailable() < m_nNextBlockSize) { break; }

        QString str; in >> str;
        emit gotNewMesssage(str);
        qDebug() <<str.left(2);
        /*
        qDebug()<< str;
        if (str == "0")
        {
            str = "Connection closed";
            closeConnection();
        }
        if(str == "Reply: received [-Name:" + MyName + "]"){
            str = "Connection closed";
            closeConnection();
        }
        if(str != "Reply: received [-Name:" + MyName + "]" && str != "Reply: received [Name:" + MyName + "]"){
        emit hasReadSome(str);
        }
        */
        if(str.left(2)!="Re"){
            if (sendToClient(tcpSocket, QString("Reply: received [%1]").arg(str)) == -1)
            {
                qDebug() << "Some error occured";
            }
       }
        m_nNextBlockSize = 0;
    }
}

void ServerStuff::closeConnection()
{

    timeoutTimer->stop();

    //qDebug() << tcpSocket->state();
    disconnect(tcpSocket, &QTcpSocket::connected, 0, 0);
    disconnect(tcpSocket, &QTcpSocket::readyRead, 0, 0);

    bool shouldEmit = false;
    switch (tcpSocket->state())
    {
        case 0:
            tcpSocket->disconnectFromHost();
            shouldEmit = true;
            break;
        case 2:
            tcpSocket->abort();
            shouldEmit = true;
            break;
        default:
            tcpSocket->abort();
    }

    if (shouldEmit)
    {
        status = false;
        emit statusChanged(status);
    }
}


void ServerStuff::newConnection()
{
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();

    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    connect(clientSocket, &QTcpSocket::readyRead, this, &ServerStuff::readClient);
    connect(clientSocket, &QTcpSocket::disconnected, this, &ServerStuff::gotDisconnection);

    clients << clientSocket;

    sendToClient(clientSocket, "Reply: connection established");
}

void ServerStuff::readClient()
{
    QTcpSocket *clientSocket = (QTcpSocket*)sender();
    QDataStream in(clientSocket);
    //in.setVersion(QDataStream::Qt_5_10);
    for (;;)
    {
        if (!m_nNextBlockSize)
        {
            if (clientSocket->bytesAvailable() < sizeof(quint16)) { break; }
            in >> m_nNextBlockSize;
        }

        if (clientSocket->bytesAvailable() < m_nNextBlockSize) { break; }
        QString str;
        in >> str;
        qDebug() << "Recived " + str;
        emit gotNewMesssage(str);

        /*
        if(str.left(5) == "Name:"){
            clientsName.append(str.right(str.length()-5));
            qDebug() << "Added " + str.right(str.length()-5);
            emit newConnectionToServer(str.right(str.length()-5));
            //qDebug() << str.right(str.length()-5);
        }
        if(str.left(6) == "-Name:"){
            qDebug() << "Removed " + str.right(str.length()-6);
            clients.removeAt(clients.indexOf((QTcpSocket*)sender()));
            emit smbDisconnected(str.right(str.length()-6));
        }

        for(int i=0;i<clients.length();i++){
            if(str.right(str.length()-5)==clientsName[i] && i!=clientsName.length()-1){

                qDebug() << "Changed " + clientsName[i] + " to " + str.right(str.length()-5);
                clientsName[i]=clientsName[clientsName.length()-1];
                clientsName.pop_back();
            }
        }

        if(str.left(5) != "Name:" && str.left(6) != "-Name:" ){
            emit gotNewMesssage(str);
        }
        */
        m_nNextBlockSize = 0;
        if(str.left(2)!="Re"){
        if (sendToClient(clientSocket, QString("Reply: received [%1]").arg(str)) == -1)
        {
            qDebug() << "Some error occured";
        }
        }
    }
}

void ServerStuff::gotDisconnection()
{
    clients.removeAt(clients.indexOf((QTcpSocket*)sender()));
    emit smbDisconnected("");
}

qint64 ServerStuff::sendToClient(int socketid, const QString &str)
{
    QTcpSocket *socket = clients[socketid];
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    //out.setVersion(QDataStream::Qt_5_10);
    //out << quint16(0) << QTime::currentTime() << str;
    if(str != "-Name:" + MyName){
    out << quint16(0) << MyName + ": " + str;
    }
    else{
        out << quint16(0) << str;
    }
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    return socket->write(arrBlock);
}
qint64 ServerStuff::sendToClient(QTcpSocket *socket, const QString &str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    //out.setVersion(QDataStream::Qt_5_10);
    //out << quint16(0) << QTime::currentTime() << str;
    out << quint16(0) << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    return socket->write(arrBlock);
}
