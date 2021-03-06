#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QStandardItemModel>

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

private slots:
    void on_actionOpen_triggered();
    void on_tvTablesAndViews_clicked(const QModelIndex &index);
    void on_actionExecute_triggered();
    void on_tabArea_currentChanged(int index);
    void on_actionColumnsView_triggered();
    void on_actionTablesViews_triggered();
    void on_actionIndexesView_triggered();
    void on_actionForigenKeysView_triggered();
    void on_actionTriggersView_triggered();
    void on_dockTablesAndViews_visibilityChanged(bool visible);
    void on_dockColumns_visibilityChanged(bool visible);
    void on_dockForeignKeys_visibilityChanged(bool visible);
    void on_dockIndexes_visibilityChanged(bool visible);
    void on_dockTriggers_visibilityChanged(bool visible);

private:
    Ui::MainWindow *ui;

    void updateContextTab();
    void updateStructureTab();
    void updateTriggers();
    void updateIndexes();
    void updateForeignKeys();
    void updateColumns();
    void updateTablesAndViewersModel();
    void hideOrShow(QWidget *w);
    bool openDb(const QString &name);
    bool openJson(const QString &name);
    bool openPlist(const QString &name);

    QSqlDatabase db;

    QSqlTableModel *m_tmContext;
    QSqlQueryModel m_sqmColumns, m_sqmForeignKeys, m_sqmIndexes, m_sqmTriggers, m_sqmQuery;
    QStandardItemModel m_simTablesAndViews;

    QStandardItem *m_currentTV;

    QIcon m_iconTables, m_iconViews, m_iconTable, m_iconView;

    // m_simTablesAndViews中的用户自定义角色
    enum class ITEM_ROLE_TYPE {
        ROLE_TABLESANDVIEWS = Qt::UserRole + 1
    };

    // m_simTablesAndViews中的用户自定义角色的数值
    enum class ITEM_DATA_TYPE {
        DATA_TABLES = 1,
        DATA_VIEWS = 2,
        DATA_TABLE = 3,
        DATA_VIEW = 4
    };
};

#endif // MAINWINDOW_H
