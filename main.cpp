#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QIcon i(":/sqliteDB.icns");
    a.setWindowIcon(i);

    MainWindow w;
    w.setWindowIcon(i);
    w.show();
    w.init();

    return a.exec();
}

//pragma table_info (actor)
//PRAGMA foreign_key_list(film_actor)
//PRAGMA index_list ('film_actor')
//PRAGMA index_info ('idx_fk_film_actor_actor')
