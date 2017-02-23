#include <QStandardItemModel>
#include <QStandardItem>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QDebug>
#include "jsonreader.h"

JsonReader::JsonReader(QObject *parent) : QObject(parent)
{

}

void JsonReader::parseToModel(const QString& name, QStandardItemModel* model)
{
    QFile f(name);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Q_ASSERT(false);
    }
    QByteArray ba = f.readAll();

    QJsonParseError jpErr;
    QJsonDocument jd = QJsonDocument::fromJson(ba, &jpErr);

    QStandardItem *root = model->invisibleRootItem();
    if (jd.isObject()) {
        parseJsonObject(root, jd.object());
    } else if (jd.isArray()) {
        parseJsonArray(root, jd.array());
    } else if (jd.isEmpty()) {
        qDebug() << "Json document is empty!";
    } else if (jd.isNull()) {
        qDebug() << "Json document is null!";
    } else {
        qDebug() << "Json document error!";
        Q_ASSERT(false);
    }
}

void JsonReader::parseJsonObject(QStandardItem *parent, const QJsonObject &jo)
{
    for (QJsonObject::const_iterator i = jo.constBegin(); i != jo.constEnd(); ++i) {

        QList<QStandardItem*> row;
        QStandardItem *item = new QStandardItem(i.key());
        row.append(item);
        buildRow(item, i.value(), row);
        parent->appendRow(row);
    }
}

void JsonReader::parseJsonArray(QStandardItem *parent, const QJsonArray &ja)
{
    int no = 0;
    for (QJsonArray::const_iterator i = ja.constBegin(); i < ja.constEnd(); ++i) {
        QList<QStandardItem*> row;
        QStandardItem *item = new QStandardItem(QStringLiteral("%1").arg(no++));
        row.append(item);
        buildRow(item, *i, row);
        parent->appendRow(row);
    }
}

void JsonReader::buildRow(QStandardItem *parent, const QJsonValue &jv, QList<QStandardItem*>& row)
{
    if (jv.isObject()) {
        row.append(new QStandardItem(tr("Object")));
        row.append(new QStandardItem(tr("")));
        parseJsonObject(parent, jv.toObject());
    } else if (jv.isArray()) {
        row.append(new QStandardItem(tr("Array")));
        row.append(new QStandardItem(tr("")));
        parseJsonArray(parent, jv.toArray());
    } else if (jv.isBool()) {
        row.append(new QStandardItem(tr("Boolean")));
        row.append(new QStandardItem(jv.toBool()? tr("true") : tr("false")));
    } else if (jv.isDouble()) {
        row.append(new QStandardItem(tr("Number")));
        row.append(new QStandardItem(QStringLiteral("%1").arg(jv.toDouble())));
    } else if (jv.isString()) {
        row.append(new QStandardItem(tr("String")));
        row.append(new QStandardItem(jv.toString()));
    } else if (jv.isNull()) {
        qDebug() << "isNull";
        Q_ASSERT(false);
    } else if (jv.isUndefined()) {
        qDebug() << "isUndefined";
        Q_ASSERT(false);
    } else {
        Q_ASSERT(false);
    }
}
