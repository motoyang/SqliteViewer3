#ifndef SQLQUERYMODEL_H
#define SQLQUERYMODEL_H

#include <QStandardItemModel>

class SqlQueryModel : public QStandardItemModel
{
public:
    SqlQueryModel();

    void updateData(const QStringList &headerLabels, const QString &sql);
    void updateData(const QString& sql);

private:
//    QString m_sql;
};

#endif // SQLQUERYMODEL_H
