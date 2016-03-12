#include "mainwindow.h"
#include <QApplication>
#include <QProxyStyle>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QStyle *style = new QProxyStyle(QStyleFactory::create("fusion"));
    a.setStyle(style);

    w.show();

    return a.exec();
}
