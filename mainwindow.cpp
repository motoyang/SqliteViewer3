#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QSqlError>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_currentTV(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::init()
{
    db = QSqlDatabase::addDatabase("QSQLITE");

    m_iconTables = QIcon(QStringLiteral(":/Resources/tables_128x128.png"));
    m_iconTable = QIcon(QStringLiteral(":/Resources/table_128x128.png"));
    m_iconViews = QIcon(QStringLiteral(":/Resources/viewers_128x128.png"));
    m_iconView = QIcon(QStringLiteral(":/Resources/viewer_128x128.png"));

    ui->tvTablesAndViews->setModel(&m_simTablesAndViews);
    ui->tvColumns->setModel(&m_simColumns);
    ui->tvColumns->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tvForeignKeys->setModel(&m_simForeignKeys);
    ui->tvForeignKeys->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tvIndexes->setModel(&m_simIndexes);
    ui->tvIndexes->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tvTriggers->setModel(&m_simTriggers);
    ui->tvTriggers->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    m_tmContext = new QSqlTableModel(this, db);
    ui->tvContext->setModel(m_tmContext);
    ui->tvContext->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    ui->tvQuery->setModel(&m_sqmQuery);
    ui->tvQuery->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    return true;
}

void MainWindow::updateStructureTab()
{
    QString sql = QStringLiteral("select sql from sqlite_master where tbl_name='%1'")
            .arg(m_currentTV->data(Qt::DisplayRole).toString());
    QSqlQuery query(sql);
    query.exec();

    sql.clear();
    while (query.next()) {
        sql.append(query.value(0).toString() + QStringLiteral("\n\n"));
    }

    ui->teStructure->setText(sql);
}

void MainWindow::updateContextTab()
{
    QString name = m_currentTV->data(Qt::DisplayRole).toString();
    m_tmContext->setTable(name);
    m_tmContext->setEditStrategy(QSqlTableModel::OnRowChange);
    if (!m_tmContext->select()) {
        QSqlError e = m_tmContext->lastError();
        QString erro = e.text();
        qDebug() << erro;
        Q_ASSERT(false);
    }
}

void MainWindow::updateTriggers()
{
    QString sql = QStringLiteral("select name, sql from sqlite_master where type='trigger' and tbl_name='%1'")
            .arg(m_currentTV->data(Qt::DisplayRole).toString());
    m_simTriggers.setQuery(sql);
    ui->tvTriggers->resizeColumnsToContents();
}

void MainWindow::updateIndexes()
{
    // 查询所有的indexes
    QString sql = QStringLiteral("PRAGMA index_list ('%1')").arg(m_currentTV->data(Qt::DisplayRole).toString());
    QSqlQuery query(sql);
    query.exec();

    // 构造in子句
    QString indexes = QStringLiteral("('");
    while (query.next()) {
        indexes.append(query.value(1).toString() + QStringLiteral("', '"));
    }
    // 更换最后的三个字符", '"为")"
    int sz = indexes.size();
    if (sz > 2) {
        indexes.replace(sz - 3, 3, QStringLiteral(")"));
    }

    // 根据构造的select语句，在sqlite_master中查询对应的indexes
    sql = QStringLiteral("select name, sql from sqlite_master where name in %1").arg(indexes);
    m_simIndexes.setQuery(sql);

    // 根据item的内容，resize列的宽带
    ui->tvIndexes->resizeColumnsToContents();
}

void MainWindow::updateForeignKeys()
{
    QString sql = QStringLiteral("pragma foreign_key_list('%1')").arg(m_currentTV->data(Qt::DisplayRole).toString());
    m_simForeignKeys.setQuery(sql);
}

void MainWindow::updateColumns()
{
    QStringList sl;
    sl << tr("cid") << tr("name") << tr("type") << tr("notnull") << tr("default value")
       << tr("pk");
//    m_simColumns.setHeaderData(0, Qt::Horizontal, tr("Name"));
//    model->setHeaderData(1, Qt::Horizontal, tr("Salary"));

    QString sql = QStringLiteral("pragma table_info ('%1')").arg(m_currentTV->data(Qt::DisplayRole).toString());
    m_simColumns.setQuery(sql);
}

void MainWindow::updateTablesAndViewersModel()
{
    // 先清除原先的数据
    m_simTablesAndViews.clear();

    // 初始化Tables和Views items。
    QStandardItem *itemTables = new QStandardItem(m_iconTables, tr("Tables"));
    itemTables->setData(int(ITEM_DATA_TYPE::DATA_TABLES), int(ITEM_ROLE_TYPE::ROLE_TABLESANDVIEWS));
    m_simTablesAndViews.appendRow(itemTables);
    QStandardItem *itemViews = new QStandardItem(m_iconViews, tr("View"));
    itemViews->setData(int(ITEM_DATA_TYPE::DATA_VIEWS), int(ITEM_ROLE_TYPE::ROLE_TABLESANDVIEWS));
    m_simTablesAndViews.appendRow(itemViews);

    QString sql = QStringLiteral("select * from sqlite_master where type='table' or type = 'view' order by type");
    QSqlQuery query(sql);
    query.exec();

    while (query.next()) {
        // 判断这行记录是table还是view
        QString type = query.value(0).toString();
        if (type == QStringLiteral("table")) {
            QStandardItem *item = new QStandardItem(m_iconTable, query.value(2).toString());
            item->setData(int(ITEM_DATA_TYPE::DATA_TABLE), int(ITEM_ROLE_TYPE::ROLE_TABLESANDVIEWS));
            itemTables->appendRow(item);
        } else if (type == QStringLiteral("view")) {
            QStandardItem *item = new QStandardItem(m_iconView, query.value(2).toString());
            item->setData(int(ITEM_DATA_TYPE::DATA_VIEW), int(ITEM_ROLE_TYPE::ROLE_TABLESANDVIEWS));
            itemViews->appendRow(item);
        } else {
            Q_ASSERT(false);
        }

    }
}

bool MainWindow::openDb(const QString &name)
{
    bool ret = true;
    if (db.isOpen()) {
        db.close();
    }
//    tmTablesAndViewers.initRoot();

    db.setDatabaseName(name);
    if (!db.open()) {
        Q_ASSERT(false);
        ret = false;
    }

    updateTablesAndViewersModel();
//    tmTablesAndViewers.setupModelData(db);

    return ret;
}

void MainWindow::on_actionOpen_triggered()
{
    openDb("/Users/moto2yang/cpp/qt/test1/db/Sakila.db");
//    ui->tvTablesAndViewers->setModel(&tmTablesAndViewers);
}

void MainWindow::on_tvTablesAndViews_clicked(const QModelIndex &index)
{
    QStandardItem* item = m_simTablesAndViews.itemFromIndex(index);
    int itemType = item->data(int(ITEM_ROLE_TYPE::ROLE_TABLESANDVIEWS)).toInt();
    if (item == m_currentTV || itemType == int(ITEM_DATA_TYPE::DATA_TABLES)
            || itemType == int(ITEM_DATA_TYPE::DATA_VIEWS)) {
        return;
    }
    m_currentTV = item;

    updateColumns();
    updateForeignKeys();
    updateIndexes();
    updateTriggers();

    updateContextTab();
    updateStructureTab();
}

void MainWindow::on_actionExecute_triggered()
{
    QString sql = ui->teQuery->toPlainText();
    m_sqmQuery.setQuery(sql);
    while (m_sqmQuery.canFetchMore()) {
          m_sqmQuery.fetchMore();
    }

    QSqlError err = m_sqmQuery.lastError();
    if (err.isValid()) {
        QString e = err.text();
        qDebug() << e;
        ui->pteResult->setWindowTitle(e);
//        emit ui->twQueryAndResult->tabBarClicked(1);
        ui->twQueryAndResult->setCurrentWidget(ui->pteResult);
    }

    ui->tvQuery->resizeColumnsToContents();
}
