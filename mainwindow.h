#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QStandardItemModel>
//#include "sqlquerymodel.h"

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

    void updateContextTab();
    void updateStructureTab();
    void updateTriggers();
    void updateIndexes();
    void updateForeignKeys();
    void updateColumns();
    void updateTablesAndViewersModel();
    bool openDb(const QString& name);
    QSqlDatabase db;

//    TreeModel tmTablesAndViewers;
    QSqlTableModel *m_tmContext;
    QSqlQueryModel m_simColumns, m_simForeignKeys, m_simIndexes, m_simTriggers, m_sqmQuery;
    QStandardItemModel m_simTablesAndViews;
    QIcon m_iconTables, m_iconViews, m_iconTable, m_iconView;

private slots:
    void on_actionOpen_triggered();
    void on_tvTablesAndViews_clicked(const QModelIndex &index);

    void on_actionExecute_triggered();

private:
    Ui::MainWindow *ui;

    QStandardItem* m_currentTV;

    enum class ITEM_ROLE_TYPE {
        ROLE_TABLESANDVIEWS = Qt::UserRole + 1
    };
    enum class ITEM_DATA_TYPE {
        DATA_TABLES = 1,
        DATA_VIEWS = 2,
        DATA_TABLE = 3,
        DATA_VIEW = 4
    };
};

#endif // MAINWINDOW_H
