#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

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

private:
    Ui::MainWindow *ui;

public slots:
    void ReadMessageFromServer(alarm_message_struct alarm_message);
    void TimerBlinkerEvent();
    void DisconnectedState();
    void ButtonConfirmClicked();
    void ButtonExpandClicked();
    void ButtonReportClicked();

};

#endif // MAINWINDOW_H
