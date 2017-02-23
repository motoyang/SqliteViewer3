#ifndef JSONREADER_H
#define JSONREADER_H

#include <QObject>

class QStandardItemModel;
class QStandardItem;
class QJsonObject;
class QJsonArray;
class QJsonValue;

class JsonReader : public QObject
{
    Q_OBJECT
public:
    explicit JsonReader(QObject *parent = 0);

    void parseToModel(const QString &name, QStandardItemModel* model);

private:
    void parseJsonObject(QStandardItem *parent, const QJsonObject &jo);
    void parseJsonArray(QStandardItem *parent, const QJsonArray &ja);
    void buildRow(QStandardItem *parent, const QJsonValue &jv, QList<QStandardItem*> &row);

signals:

public slots:
};

#endif // JSONREADER_H
