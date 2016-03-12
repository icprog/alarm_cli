#include "alarmclient.h"

AlarmClient::AlarmClient(QString IP_addr, int port)
{
    socket = new QTcpSocket(this);


    m_IP_addr=IP_addr;
    m_port=port;

    connect(socket, SIGNAL(connected()),this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error(QAbstractSocket::SocketError)));
    connect(&timerConnectLoop,SIGNAL(timeout()),this,SLOT(TimerConnectLoopEvent()));

    timerConnectLoop.start(1000);
}
//================================================================================
AlarmClient::~AlarmClient()
{

    timerConnectLoop.stop();
    delete socket;

}
//===================================================================================
void AlarmClient::doConnect()
{

    //qDebug() << "connecting...";

    // this is not blocking call
    socket->connectToHost(m_IP_addr, m_port);
    //isConnecting=true;
    // we need to wait...
    //if(!socket->waitForConnected(5000))
    //{
    //    qDebug() << "Error: " << socket->errorString();
    //}
}
//==================================================================================
void AlarmClient::TimerConnectLoopEvent()
{
//    qDebug() << "timer event: ";

        if (getStateConnected()==false && getStateConnecting()==false)
        {
            doConnect();
        }

}
//====================================================================================
void AlarmClient::connected()
{
    qDebug() << "connected...";

}
//=====================================================================================
void AlarmClient::error(QAbstractSocket::SocketError err)
{
    socket->disconnectFromHost();
    qDebug() << "error " + QString::number(err);
}
//=======================================================================================
void AlarmClient::disconnected()
{
    enabledAlarmsList.clear();
    emit DisconnectedFromServer();
    qDebug() << "disconnected...";
}
//=======================================================================================
void AlarmClient::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}
//=======================================================================================
void AlarmClient::readyRead()
{
    qDebug() << "reading..."+QString::number(socket->bytesAvailable());
    alarm_message_struct alarm_message;

    // read the data from the socket
    while (socket->bytesAvailable() >= sizeof(alarm_message_struct)) // || socket->bytesAvailable()==number_float_tags*4+4) //ribalci gluk
    {
        socket->read((char *)&alarm_message,sizeof(alarm_message_struct));
        emit MessageFromServer(alarm_message);
        qDebug() << alarm_message.ID << "  " << alarm_message.text;
    }
    if (socket->bytesAvailable()>0 && socket->bytesAvailable()<sizeof(alarm_message_struct))  //осталось байт не кратное одной посылке, - херня,ошибка, не тот сервер
    {
        socket->disconnectFromHost();
    }
}
//===========================================================================================
/*
QAbstractSocket::UnconnectedState	0	The socket is not connected.
QAbstractSocket::HostLookupState	1	The socket is performing a host name lookup.
QAbstractSocket::ConnectingState	2	The socket has started establishing a connection.
QAbstractSocket::ConnectedState     3	A connection is established.
QAbstractSocket::BoundState         4	The socket is bound to an address and port.
QAbstractSocket::ClosingState       6	The socket is about to close (data may still be waiting to be written).
QAbstractSocket::ListeningState     5	For internal use only.
*/
//=============================================================
bool AlarmClient::getStateConnected()
{
   return socket->state()==QAbstractSocket::ConnectedState;
}
//=============================================================
bool AlarmClient::getStateConnecting()
{
   return socket->state()==QAbstractSocket::HostLookupState || socket->state()==QAbstractSocket::ConnectingState;
}
//=============================================================
void AlarmClient::ConfirmAlarm(alarm_message_struct alarm_message)
{


    if (socket->write((char *)&alarm_message,sizeof(alarm_message_struct))==sizeof(alarm_message_struct))
    {
        qDebug() << "confirm last alarm" << alarm_message.text << alarm_message.ID;
    }
    else
    {
        qDebug() << "confirm last alarm ERROR" << alarm_message.text << alarm_message.ID;
    }
    //socket->waitForBytesWritten(5000);


}
