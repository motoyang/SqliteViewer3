#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QStandardItemModel>
#include "treemodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool init();

    bool openDb(const QString& name);
    QSqlDatabase db;

    TreeModel tmTablesAndViewers;
    QStandardItemModel simColumns, simForeignKeys, simIndexes, simTriggers;


private slots:
    void on_actionOpen_triggered();

    void on_tvTablesAndViewers_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
