#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlRecord>
#include <QIcon>
#include <QStringList>
#include "treeitem.h"
#include "treemodel.h"

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent),
      m_rootItem(nullptr),
      m_currentItem(nullptr)
{

    m_iconTables = std::make_shared<QIcon>(QStringLiteral(":/Resources/tables_128x128.png"));
    m_iconTable = std::make_shared<QIcon>(QStringLiteral(":/Resources/table_128x128.png"));
    m_iconViewers = std::make_shared<QIcon>(QStringLiteral(":/Resources/viewers_128x128.png"));
    m_iconViewer = std::make_shared<QIcon>(QStringLiteral(":/Resources/viewer_128x128.png"));


}

TreeModel::~TreeModel()
{
    delete m_rootItem;
}

void TreeModel::selectItem(const QModelIndex &index)
{
    TreeItem *item = itemFromIndex(index);
    if (item == m_currentItem) {
        return;
    }

    if (item->type() == TreeItem::ITEM_TYPE::ITEM_TABLES || item->type() == TreeItem::ITEM_TYPE::ITEM_TABLES) {
        return;
    }

    QStringList sl;
    sl << tr("cid") << tr("name") << tr("type") << tr("notnull") << tr("default value")
       << tr("pk");

    QString sql = QStringLiteral("pragma table_info ('%1')").arg(item->data(0).toString());
    QSqlQuery query;
    query.exec(sql);

    while (query.next()) {

    }
}

TreeItem* TreeModel::itemFromIndex(const QModelIndex &index) const
{
    return static_cast<TreeItem*>(index.internalPointer());
}

TreeItem* TreeModel::getItem(int i) const
{
    TreeItem* iRet = nullptr;
    if (0 <= i && 2 >= i) {
       Q_ASSERT(false);
        return iRet;
    }

    if (i == 0) {
        iRet = m_rootItem;
    } else {
        iRet = m_rootItem->child(i - 1);
    }

    return iRet;
}

bool TreeModel::initRoot()
{
    if (m_rootItem) {
        delete m_rootItem;
        m_rootItem = nullptr;
    }

    QList<QVariant> rootData;
    rootData << tr("Title");
    m_rootItem = new TreeItem(nullptr, rootData);
    m_rootItem->setType(TreeItem::ITEM_TYPE::ITEM_ROOT);

    QList<QVariant> data;
    data.append(tr("Tables"));
    TreeItem *tablesItem = new TreeItem(m_iconTables, data, m_rootItem);
    tablesItem->setType(TreeItem::ITEM_TYPE::ITEM_TABLES);
    m_rootItem->appendChild(tablesItem);
    data.pop_front();
    data.append(tr("Viewers"));
    TreeItem *viewersItem = new TreeItem(m_iconViewers, data, m_rootItem);
    viewersItem->setType(TreeItem::ITEM_TYPE::ITEM_VIEWERS);
    m_rootItem->appendChild(viewersItem);

    return true;
}

void TreeModel::setupModelData(const QSqlDatabase &db)
{
    Q_UNUSED(db);

    QString sql = QStringLiteral("select * from sqlite_master where type='table' or type = 'view' order by type");
    QSqlQuery query(sql);
    query.exec();

    std::shared_ptr<QIcon> icon = nullptr;
    while (query.next()) {
        // 先判断这行记录是table还是view
        TreeItem::ITEM_TYPE it;
        TreeItem *parent = nullptr;
        QString type = query.value(0).toString();
        if (type == QStringLiteral("table")) {
            parent = m_rootItem->child(0);
            icon = m_iconTable;
            it = TreeItem::ITEM_TYPE::ITEM_TABLE;
        } else if (type == QStringLiteral("view")) {
            parent = m_rootItem->child(1);
            icon = m_iconViewer;
            it = TreeItem::ITEM_TYPE::ITEM_VIEWER;
        } else {
            Q_ASSERT(false);
        }

        // 将table或view的名称放入TreeItem的data中
        QList<QVariant> lv;
        lv.append(query.value(2));

        TreeItem *item = new TreeItem(icon, lv, parent);
        item->setSql(query.value(4).toString());
        item->setType(it);
        parent->appendChild(item);
    }
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
//        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
        return itemFromIndex(parent)->columnCount();
    } else {
        return m_rootItem->columnCount();
    }
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    QVariant qRet;

    if (!index.isValid()) {
        return qRet;
    }

//    TreeItem *i = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *i = itemFromIndex(index);

    if (role == Qt::ToolTipRole) {
        qRet = i->sql();
    }

    if (role == Qt::DecorationRole) {
        qRet = *(i->icon());
    }

    if (role == Qt::DisplayRole) {
        qRet = i->data(index.column());
    }

    return qRet;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else {
//        parentItem = static_cast<TreeItem*>(parent.internalPointer());
        parentItem = itemFromIndex(parent);
    }

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

//    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *childItem = itemFromIndex(index);
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else {
//        parentItem = static_cast<TreeItem*>(parent.internalPointer());
        parentItem = itemFromIndex(parent);
    }
    return parentItem->childCount();
}

