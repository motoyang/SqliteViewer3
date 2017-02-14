#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlRecord>
#include <QIcon>
#include "treeitem.h"
#include "treemodel.h"

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent),
      rootItem(nullptr)
{

    m_iconTables = std::make_shared<QIcon>(QStringLiteral(":/Resources/tables_128x128.png"));
    m_iconTable = std::make_shared<QIcon>(QStringLiteral(":/Resources/table_128x128.png"));
    m_iconViewers = std::make_shared<QIcon>(QStringLiteral(":/Resources/viewers_128x128.png"));
    m_iconViewer = std::make_shared<QIcon>(QStringLiteral(":/Resources/viewer_128x128.png"));


}

TreeModel::~TreeModel()
{
    delete rootItem;
}

bool TreeModel::initRoot()
{
    if (rootItem) {
        delete rootItem;
        rootItem = nullptr;
    }

    QList<QVariant> rootData;
    rootData << tr("Title");
    rootItem = new TreeItem(nullptr, rootData);

    QList<QVariant> data;
    data.append(tr("Tables"));
    TreeItem *tablesItem = new TreeItem(m_iconTables, data, rootItem);
    rootItem->appendChild(tablesItem);
    data.pop_front();
    data.append(tr("Viewers"));
    TreeItem *viewersItem = new TreeItem(m_iconViewers, data, rootItem);
    rootItem->appendChild(viewersItem);

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
        TreeItem *parent = nullptr;
        QString type = query.value(0).toString();
        if (type == QStringLiteral("table")) {
            parent = rootItem->child(0);
            icon = m_iconTable;
        } else if (type == QStringLiteral("view")) {
            parent = rootItem->child(1);
            icon = m_iconViewer;
        } else {
            Q_ASSERT(false);
        }

        // 将table或view的名称放入TreeItem的data中
        QList<QVariant> lv;
        lv.append(query.value(2));

        TreeItem *item = new TreeItem(icon, lv, parent);
        item->setSql(query.value(4).toString());
        parent->appendChild(item);
    }
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    QVariant qRet;

    if (!index.isValid()) {
        return qRet;
    }

    TreeItem *i = static_cast<TreeItem*>(index.internalPointer());

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
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

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

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

