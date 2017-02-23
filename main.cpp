#include <QApplication>
#include <QStyleFactory>
#include <QIcon>
#include "mainwindow.h"
#include "mainframe.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QIcon i(":/sqliteDB.icns");
    a.setWindowIcon(i);

    MainWindow w;
//    MainFrame w;
    w.setWindowIcon(i);
    w.init();
    w.show();

    return a.exec();
}
