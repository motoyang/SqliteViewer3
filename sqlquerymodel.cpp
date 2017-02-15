#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include "sqlquerymodel.h"

SqlQueryModel::SqlQueryModel()
{

}

void SqlQueryModel::updateData(const QString &sql)
{
    clear();

    QSqlQuery query(sql);
    query.exec();
//    int i = query.
//    QSqlRecord record = query.record();
//    QSqlField f = record.count();
}

void SqlQueryModel::updateData(const QStringList &headerLabels, const QString &sql)
{
    clear();

    setHorizontalHeaderLabels(headerLabels);

    QSqlQuery query(sql);
    query.exec();

    while (query.next()) {
        QList<QStandardItem*> lsi;
        for (int i = 0; i < headerLabels.count(); ++i) {
            lsi.append(new QStandardItem(query.value(i).toString()));
        }
        appendRow(lsi);
    }
}
