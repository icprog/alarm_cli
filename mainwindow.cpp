
#include <QDesktopWidget>
#include <QProcess>
#include <QFile>
#include <QSettings>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>

#include "mainwindow.h"
#include "ui_mainwindow.h"

//==================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QSettings settings(qApp->applicationDirPath()+"\\alarm_cli.ini",QSettings::IniFormat);
    //TO DO - remove next line and set ini file coding to UTF-8
    settings.setIniCodec("Windows-1251");
    settings.beginGroup("Main");
    QString IP=settings.value("IP").toString();
    uint port=settings.value("Port").toInt();
    reportCmd=settings.value("Report","Cpp_reporter.exe alarms_rep.ini").toString();
    alwaysStayOnTop=settings.value("AlwaysStayOnTop",1).toInt();
    widthStretch==settings.value("WidthStretch",0).toInt();

    alarmClient=new AlarmClient(IP,port);

    connect(alarmClient,SIGNAL(MessageFromServer(alarm_message_struct)),this,SLOT(ReadMessageFromServer(alarm_message_struct)));

    connect(ui->buttonConfirm,SIGNAL(clicked()),this,SLOT(ButtonConfirmClicked()));
    connect(ui->buttonExpand,SIGNAL(clicked()),this,SLOT(ButtonExpandClicked()));
    connect(ui->buttonReport,SIGNAL(clicked()),this,SLOT(ButtonReportClicked()));
    connect(ui->buttonClose,SIGNAL(clicked()),this,SLOT(close()));

    connect(alarmClient,SIGNAL(DisconnectedFromServer()),this,SLOT(DisconnectedState()));

    DisconnectedState();

    connect(&timerBlinker,SIGNAL(timeout()),this,SLOT(TimerBlinkerEvent()));
    timerBlinker.start(500);

    //поверх всех окон,

    QRect screen=QApplication::desktop()->availableGeometry();

    if (alwaysStayOnTop)
    {
        this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    }
    else
    {
        this->setWindowFlags(/*Qt::WindowStaysOnTopHint | */Qt::FramelessWindowHint);
    }

    if (widthStretch)
    {
        this->setGeometry(0,screen.height() - this->size().height(),screen.width(),this->size().height());
    }
    else
    {
        this->setGeometry(screen.width() - this->size().width(),screen.height() - this->size().height(),this->size().width(),this->size().height());
    }




    sound=new QSound(":/wavs/alarm.wav"); //from resources
    sound->setLoops(QSound::Infinite);

    //Allow acknowledge alarms only from IPs that present in ini file
    //example IpAllowedAck=172.16.4.23;172.16.49.200
    //to allow all use IpAllowedAck=127.0.0.1

    QString IpAllowedAck=settings.value("IpAllowedAck").toString();

    //found network interfaces
    QList<QNetworkInterface> ni_list=QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface ni, ni_list)
    {
        QList<QNetworkAddressEntry> nae_list=ni.addressEntries();
        foreach(QNetworkAddressEntry nae,nae_list)
        {
            qDebug() << "found interface, IP=" << nae.ip().toString();
            if (IpAllowedAck.contains(nae.ip().toString()))
            {
                ui->buttonConfirm->setEnabled(true);
            }
        }
    }

    ui->labelBiggerThen5Alarms->setVisible(false);
    movie = new QMovie(":/images/AnimRedArrow6.gif");
    ui->labelBiggerThen5Alarms->setMovie(movie);
}
//==================================================
MainWindow::~MainWindow()
{
    delete alarmClient;
    delete sound;  //added 12.03.2016
    movie->stop();
    delete movie;
    delete ui;
}
//==================================================
void MainWindow::ReadMessageFromServer(alarm_message_struct alarm_message)
{
    if (alarm_message.ID==-1) //erase command
    {
        alarmClient->enabledAlarmsList.clear();
    }

    if (alarm_message.ID >= 0) //alarm sended
    {
        alarmClient->enabledAlarmsList.push_back(alarm_message);

    }

    alarmsChanged(alarmClient->enabledAlarmsList,false);
}
//==================================================
void MainWindow::alarmsChanged(QList < alarm_message_struct > EnabledAlarmList, bool onlyColorChange)
{
    //QColor this_actTextColor(Qt::white);
    //QColor this_act2TextColor(Qt::black);
    int i=0;

    if (!onlyColorChange)
    {
    ui->alarmWidget->clear();
        foreach(alarm_message_struct alarm, EnabledAlarmList)
        {
            QColor this_actColor(alarm.actColor_r,alarm.actColor_g,alarm.actColor_b);
            QColor this_act2Color(alarm.act2Color_r,alarm.act2Color_g,alarm.act2Color_b);
            QColor this_nonactColor(alarm.nonactColor_r,alarm.nonactColor_g,alarm.nonactColor_b);

            if (i<200)
            {
                QString alarmstr;

                alarmstr.append(alarm.DT_activate);
                alarmstr.append(" ");
                alarmstr.append(alarm.text);
                while(alarmstr.length()<92) alarmstr.append(" ");

                alarmstr.append(QString::number(alarm.alarmValue,'f',2));

                while(alarmstr.length()<100) alarmstr.append(" ");

                if (alarm.active) alarmstr.append("АКТ");
                else alarmstr.append(QString("НЕАКТ ") + alarm.DT_deactivate);
                while(alarmstr.length()<128) alarmstr.append(" ");

                if (!alarm.confirmed) alarmstr.append("НЕКВИТ");
                else alarmstr.append(QString("КВИТ ")+alarm.DT_confirmate);


                ui->alarmWidget->addItem(alarmstr);
                if (alarm.active)
                {
                    ui->alarmWidget->item(i)->setBackground(this_act2Color);
                    //ui->alarmWidget->item(i)->setTextColor(this_act2TextColor);
                }
                else
                {
                    ui->alarmWidget->item(i)->setBackground(this_nonactColor);
                    //ui->alarmWidget->item(i)->setTextColor(this_act2TextColor);
                }
            }
        ++i;
        }
        //down arow is signalized then alarms > 5 and they are not all visible in list
        if (EnabledAlarmList.size()>5)
        {
            ui->labelBiggerThen5Alarms->setVisible(true);
            movie->start();
        }
        else
        {
            movie->stop();
            ui->labelBiggerThen5Alarms->setVisible(false);
        }

    }
    else  //мигание
    {
        foreach(alarm_message_struct alarm, EnabledAlarmList)
        {
            QColor this_actColor(alarm.actColor_r,alarm.actColor_g,alarm.actColor_b);
            QColor this_act2Color(alarm.act2Color_r,alarm.act2Color_g,alarm.act2Color_b);
            QColor this_nonactColor(alarm.nonactColor_r,alarm.nonactColor_g,alarm.nonactColor_b);


            if (i<200)
            {
                if (!alarm.confirmed && alarm.active)
                {
                    if (ui->alarmWidget->item(i)->background()==this_actColor)
                    {
                        ui->alarmWidget->item(i)->setBackground(this_act2Color);
                        //ui->alarmWidget->item(i)->setTextColor(this_act2TextColor);
                    }
                    else
                    {
                        ui->alarmWidget->item(i)->setBackground(this_actColor);
                        //ui->alarmWidget->item(i)->setTextColor(this_actTextColor);
                    }
                }
            }
        ++i;
        }
    }
}
//===============================================================
void MainWindow::TimerBlinkerEvent()
{
    alarmsChanged(alarmClient->enabledAlarmsList,true);

    bool is_active_alarms=false;
    foreach(alarm_message_struct alarm, alarmClient->enabledAlarmsList)
    {
        if (alarm.active && !alarm.confirmed)  //если хоть один аларм активен и не подтвержден - сирена
        {
            if (sound->isFinished())
            sound->play();
            is_active_alarms=true;

            //когда есть активные алармы - окно всегда сверху всех
            if (!alwaysStayOnTop)
            {
                Qt::WindowFlags flags = this->windowFlags();
                if(!(flags & Qt::WindowStaysOnTopHint))
                {
                    flags ^= Qt::WindowStaysOnTopHint;
                    this->setWindowFlags(flags);
                    this->show();
                    this->activateWindow();
                }
            }
        }
    }

    if (!is_active_alarms)
    {
        sound->stop();  //нет активных алармов, выключаем сирену
        //окно можно спрятать за остальные
        if (!alwaysStayOnTop)
        {
            //if (this->windowFlags() | Qt::WindowStaysOnTopHint)
            Qt::WindowFlags flags = this->windowFlags();
            if(flags & Qt::WindowStaysOnTopHint)
            {
                flags |= Qt::WindowStaysOnBottomHint;
                flags ^= Qt::WindowStaysOnTopHint;
                this->setWindowFlags(flags);
                this->show();
                this->activateWindow();
            }
        }

    }
}
//===============================================================
void MainWindow::ButtonConfirmClicked()
{
  if (alarmClient->enabledAlarmsList.size()>0)
  {
      if( !(alarmClient->enabledAlarmsList.at(0).confirmed) )
        alarmClient->ConfirmAlarm(alarmClient->enabledAlarmsList.at(0));
  }

}
//================================================================
void MainWindow::ButtonExpandClicked()
{
    static bool maximized=false;

    if(!maximized)
    {
        this->showMaximized();
        ui->buttonExpand->setText("Зменшити");
        ui->buttonExpand->setIcon(QIcon(":/icons/window_nofullscreen.ico"));
    }
    else
    {
        this->showNormal();
        ui->buttonExpand->setText("На весь екран");
        ui->buttonExpand->setIcon(QIcon(":/icons/window_fullscreen.ico"));
    }

    maximized=!maximized;
}
//===============================================================
void MainWindow::DisconnectedState()
{
    ui->alarmWidget->clear();
    ui->alarmWidget->addItem("  !!!НЕМА ПІДКЛЮЧЕННЯ ДО СЕРВЕРА АЛАРМІВ!!!   ");
    ui->alarmWidget->item(0)->setBackground(Qt::white);
}
//===============================================================
void MainWindow::ButtonReportClicked()
{
    QProcess::startDetached(reportCmd);
}
//================================================================
