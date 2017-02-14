#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QIcon>
#include <memory>

//! [0]
class TreeItem
{
public:
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

private:
    QList<TreeItem*> m_childItems;
    QList<QVariant> m_itemData;
    TreeItem *m_parentItem;

    std::shared_ptr<QIcon> m_icon;
    QString m_sql;
};
//! [0]

#endif // TREEITEM_H
