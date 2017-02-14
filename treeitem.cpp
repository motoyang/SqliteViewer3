#include <QStringList>
#include "treeitem.h"

TreeItem::TreeItem(const std::shared_ptr<QIcon>& icon, const QList<QVariant> &data, TreeItem *parent)
{
    m_icon = icon;
    m_parentItem = parent;
    m_itemData = data;
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::setType(ITEM_TYPE type)
{
    m_type = type;
}

TreeItem::ITEM_TYPE TreeItem::type() const
{
    return m_type;
}

std::shared_ptr<QIcon> TreeItem::icon() const
{
    return m_icon;
}

void TreeItem::setSql(const QString &sql)
{
    m_sql =  sql;
}

QString TreeItem::sql() const
{
    return m_sql;
}

void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
    return m_childItems.value(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TreeItem::data(int column) const
{
    return m_itemData.value(column);
}

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

