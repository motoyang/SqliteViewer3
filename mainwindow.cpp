#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFileDialog>
#include <QDebug>
#include <QSqlError>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_tmContext = nullptr;
    m_currentTV = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::init()
{
    // 连接Sqlite数据库
    db = QSqlDatabase::addDatabase("QSQLITE");

    // 准备TreeView中的图标
    m_iconTables = QIcon(QStringLiteral(":/Resources/tables_128x128.png"));
    m_iconTable = QIcon(QStringLiteral(":/Resources/table_128x128.png"));
    m_iconViews = QIcon(QStringLiteral(":/Resources/viewers_128x128.png"));
    m_iconView = QIcon(QStringLiteral(":/Resources/viewer_128x128.png"));

    // 设置每个view的model
    ui->tvTablesAndViews->setModel(&m_simTablesAndViews);
    ui->tvColumns->setModel(&m_sqmColumns);
    ui->tvColumns->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tvForeignKeys->setModel(&m_sqmForeignKeys);
    ui->tvForeignKeys->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tvIndexes->setModel(&m_sqmIndexes);
    ui->tvIndexes->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tvTriggers->setModel(&m_sqmTriggers);
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
    m_tmContext->setEditStrategy(QSqlTableModel::OnFieldChange);
    if (!m_tmContext->select()) {
        QSqlError e = m_tmContext->lastError();
        QString erro = e.text();
        qDebug() << erro;
        Q_ASSERT(false);
    }
    ui->tvContext->resizeColumnsToContents();
}

void MainWindow::updateTriggers()
{
    QString sql = QStringLiteral("select name, sql from sqlite_master where type='trigger' and tbl_name='%1'")
            .arg(m_currentTV->data(Qt::DisplayRole).toString());
    m_sqmTriggers.setQuery(sql);
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
    m_sqmIndexes.setQuery(sql);

    // 根据item的内容，resize列的宽带
    ui->tvIndexes->resizeColumnsToContents();
}

void MainWindow::updateForeignKeys()
{
    QString sql = QStringLiteral("pragma foreign_key_list('%1')").arg(m_currentTV->data(Qt::DisplayRole).toString());
    m_sqmForeignKeys.setQuery(sql);
    ui->tvForeignKeys->resizeColumnsToContents();
}

void MainWindow::updateColumns()
{
    // 设置表头
    m_sqmColumns.setHeaderData(0, Qt::Horizontal, tr("cid"));
    m_sqmColumns.setHeaderData(1, Qt::Horizontal, tr("Name"));
    m_sqmColumns.setHeaderData(2, Qt::Horizontal, tr("Type"));
    m_sqmColumns.setHeaderData(3, Qt::Horizontal, tr("Not null"));
    m_sqmColumns.setHeaderData(4, Qt::Horizontal, tr("Default value"));
    m_sqmColumns.setHeaderData(5, Qt::Horizontal, tr("PK"));

    QString sql = QStringLiteral("pragma table_info ('%1')").arg(m_currentTV->data(Qt::DisplayRole).toString());
    m_sqmColumns.setQuery(sql);
    ui->tvColumns->resizeColumnsToContents();
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

    ui->tvTablesAndViews->resizeColumnToContents(0);
}

bool MainWindow::openDb(const QString &name)
{
    bool ret = true;
    if (db.isOpen()) {
        db.close();

        m_sqmColumns.clear();
        m_sqmForeignKeys.clear();
        m_sqmIndexes.clear();
        m_sqmTriggers.clear();

        m_tmContext->clear();
        m_sqmQuery.clear();
        ui->pteResult->clear();
        ui->teQuery->clear();
        ui->teStructure->clear();

        ui->twQueryAndResult->setCurrentIndex(0);
    }

    db.setDatabaseName(name);
    if (!db.open()) {
        Q_ASSERT(false);
        ret = false;
    }

    updateTablesAndViewersModel();

    return ret;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), QDir::homePath(),
        tr("Database (*.db);;Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"));

    openDb(fileName);
//    openDb("/Users/moto2yang/cpp/qt/db/Sakila.db");
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
    // 根据用户的输入，查询数据库
    QString sql = ui->teQuery->toPlainText();
    m_sqmQuery.setQuery(sql);
//    while (m_sqmQuery.canFetchMore()) {
//          m_sqmQuery.fetchMore();
//    }

    // 清空result页面的内容
    ui->pteResult->clear();

    // 显示affected rows number.
    int rows = m_sqmQuery.rowCount();
    sql = QStringLiteral("%1 row(s) affected.").arg(rows);
    ui->pteResult->appendPlainText(sql);

    // 判断执行结果，如果失败，在result页面输出失败信息，并切换到result页面
    QSqlError err = m_sqmQuery.lastError();
    if (err.isValid()) {
        QString e = err.text();
        qDebug() << e;
        ui->pteResult->appendPlainText(e);
        ui->twQueryAndResult->setCurrentIndex(1);
    } else {
        ui->twQueryAndResult->setCurrentIndex(0);
        ui->tvQuery->resizeColumnsToContents();
    }
}
