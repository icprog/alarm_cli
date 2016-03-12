#ifndef ALARMCLIENT_H
#define ALARMCLIENT_H

#include <QTcpSocket>
#include <QTimer>
#include <QList>
#include <QString>
#include <QColor>

const uint ALARM_MESSAGE_TEXT=128;

enum AlarmLevel
{
   Critical,
   Warning,
   Information
};

enum AlarmCondition
{
    Bigger,
    Less,
    Equal
};

enum AlarmType
{
    OnValueChanged,
    OnQualityChanged
};


struct alarm_message_struct
{
  int ID; //ID от 1 до alarm_count... - идентификаторы алармов, меньше 0 - команды, пока команда одна = -1 - clear all alarms у клиента
  bool active;
  bool confirmed;
  char text[ALARM_MESSAGE_TEXT];

  AlarmLevel alarmLevel;
  AlarmCondition alarmCondition;
  AlarmType alarmType;
  float alarmValue;
  char DT_activate[20];
  char DT_confirmate[20];
  char DT_deactivate[20];
  int actColor_r;
  int actColor_g;
  int actColor_b;
  int act2Color_r;
  int act2Color_g;
  int act2Color_b;
  int nonactColor_r;
  int nonactColor_g;
  int nonactColor_b;

};


class AlarmClient : public QObject
{
    Q_OBJECT
public:
    explicit AlarmClient(QString IP_addr, int port);
    virtual ~AlarmClient();

    QString m_IP_addr;
    unsigned int m_port;

    QTcpSocket *socket;
    QTimer timerConnectLoop;

    QList<alarm_message_struct> enabledAlarmsList;

    void doConnect();
    bool getStateConnected();
    bool getStateConnecting();
    void ConfirmAlarm(alarm_message_struct alarm_message);

public slots:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError er);
    void bytesWritten(qint64 bytes);
    void readyRead();

    void TimerConnectLoopEvent();

    //void ConfirmingLastAlarm();

signals:
    void MessageFromServer(alarm_message_struct alarm_message);
    void DisconnectedFromServer();

};



#endif // ALARMCLIENT_H
