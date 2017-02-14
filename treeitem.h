#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QIcon>
#include <memory>

class TreeItem
{
public:
    enum class ITEM_TYPE {
        ITEM_ROOT = 0,
        ITEM_TABLES = 1,
        ITEM_TABLE = 2,
        ITEM_VIEWERS = 3,
        ITEM_VIEWER = 4
    };

    explicit TreeItem(const std::shared_ptr<QIcon>& icon, const QList<QVariant> &data, TreeItem *parentItem = 0);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItem *parentItem();
    std::shared_ptr<QIcon> icon() const;
    void setSql(const QString& sql);
    QString sql() const;
    void setType(ITEM_TYPE type);
    ITEM_TYPE type() const;

private:
    QList<TreeItem*> m_childItems;
    QList<QVariant> m_itemData;
    TreeItem *m_parentItem;

    std::shared_ptr<QIcon> m_icon;
    QString m_sql;
    ITEM_TYPE m_type;
};
//! [0]

#endif // TREEITEM_H
