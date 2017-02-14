#include <QSqlDatabase>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::init()
{
//    ui->tvTablesAndViewers->setModel(&tmTablesAndViewers);
    db = QSqlDatabase::addDatabase("QSQLITE");

    return true;
}

bool MainWindow::openDb(const QString &name)
{
    bool ret = true;
    if (db.isOpen()) {
        db.close();
    }
    tmTablesAndViewers.initRoot();

    db.setDatabaseName(name);
    if (!db.open()) {
        Q_ASSERT(false);
        ret = false;
    }

    tmTablesAndViewers.setupModelData(db);

    return ret;
}

void MainWindow::on_actionOpen_triggered()
{
    openDb("/Users/moto2yang/cpp/qt/test1/db/Sakila.db");
    ui->tvTablesAndViewers->setModel(&tmTablesAndViewers);
}

void MainWindow::on_tvTablesAndViewers_clicked(const QModelIndex &index)
{
    tmTablesAndViewers.selectItem(index);
}
