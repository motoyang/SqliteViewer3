#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QSqlDatabase>
#include <QVariant>
#include <memory>

class TreeItem;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(QObject *parent = nullptr);
    ~TreeModel();
    bool initRoot();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    TreeItem* itemFromIndex(const QModelIndex& index) const;
    TreeItem* getItem(int i) const;
    void selectItem(const QModelIndex& index);

    void setupModelData(const QSqlDatabase &db);

private:
    TreeItem *m_rootItem, *m_currentItem;
    std::shared_ptr<QIcon> m_iconTables, m_iconViewers, m_iconTable, m_iconViewer;
};

#endif // TREEMODEL_H
