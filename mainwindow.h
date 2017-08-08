#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMovie>
#include <QtMultimedia/QSound>
#include <QtMultimedia/QMediaPlayer>

#include "alarmclient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void alarmsChanged(QList < alarm_message_struct > EnabledAlarmList, bool onlyColorChange);

    QTimer timerBlinker;


    AlarmClient *alarmClient;
    QSound *sound;
    QString reportCmd;
    int alwaysStayOnTop;

private:
    Ui::MainWindow *ui;
    QMovie *movie;

public slots:
    void ReadMessageFromServer(alarm_message_struct alarm_message);
    void TimerBlinkerEvent();
    void DisconnectedState();
    void ButtonConfirmClicked();
    void ButtonExpandClicked();
    void ButtonReportClicked();

};

#endif // MAINWINDOW_H
