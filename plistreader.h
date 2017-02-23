#ifndef PLISTREADER_H
#define PLISTREADER_H

#include <QMap>
#include <QStack>

class QStandardItemModel;
class QStandardItem;
class QXmlStreamReader;

class PlistReader
{
public:
    PlistReader();
    void parseToModel(const QString &plist, QStandardItemModel *model);

private:
    QStack<int> m_indexes;
    QStack<QString> m_stack;
    void push(const QString &e);
    void pop(const QString &e);
    void appendValue(QStandardItem *parent, QList<QStandardItem*> &row, const QString &t, const QString &v);
    QStandardItem* appendGroup(QStandardItem *parent, QList<QStandardItem*> &row, const QString &g);

    const QString PLIST_PLIST       = QStringLiteral("plist");
    const QString PLIST_DICT        = QStringLiteral("dict");
    const QString PLIST_ARRAY       = QStringLiteral("array");
    const QString PLIST_KEY         = QStringLiteral("key");
    const QString PLIST_BOOLEAN     = QStringLiteral("boolean");
    const QString PLIST_FALSE       = QStringLiteral("false");
    const QString PLIST_TRUE        = QStringLiteral("true");
    const QString PLIST_INTEGER     = QStringLiteral("integer");
    const QString PLIST_NUMBER      = QStringLiteral("number");
    const QString PLIST_STRING      = QStringLiteral("string");
    const QString PLIST_DATE        = QStringLiteral("date");
    const QString PLIST_DATA        = QStringLiteral("data");

    enum class PLIST_ELEMENT_TYPE {
        PLIST_ELEMENT_NULL = 0,
        PLIST_ELEMENT_ROOT,
        PLIST_ELEMENT_DICT,
        PLIST_ELEMENT_ARRAY,
        PLIST_ELEMENT_KEY,
        PLIST_ELEMENT_BOOLEAN,
        PLIST_ELEMENT_NUMBER,
        PLIST_ELEMENT_STRING,
        PLIST_ELEMENT_DATE,
        PLIST_ELEMENT_DATA
    };

    QMap<QString, PLIST_ELEMENT_TYPE> ms_pType = {
        {PLIST_PLIST,   PLIST_ELEMENT_TYPE::PLIST_ELEMENT_ROOT},
        {PLIST_DICT,    PLIST_ELEMENT_TYPE::PLIST_ELEMENT_DICT},
        {PLIST_ARRAY,   PLIST_ELEMENT_TYPE::PLIST_ELEMENT_ARRAY},
        {PLIST_KEY,     PLIST_ELEMENT_TYPE::PLIST_ELEMENT_KEY},
        {PLIST_FALSE,   PLIST_ELEMENT_TYPE::PLIST_ELEMENT_BOOLEAN},
        {PLIST_TRUE,    PLIST_ELEMENT_TYPE::PLIST_ELEMENT_BOOLEAN},
        {PLIST_INTEGER, PLIST_ELEMENT_TYPE::PLIST_ELEMENT_NUMBER},
        {PLIST_NUMBER,  PLIST_ELEMENT_TYPE::PLIST_ELEMENT_NUMBER},
        {PLIST_STRING,  PLIST_ELEMENT_TYPE::PLIST_ELEMENT_STRING},
        {PLIST_DATE,    PLIST_ELEMENT_TYPE::PLIST_ELEMENT_DATE},
        {PLIST_DATA,    PLIST_ELEMENT_TYPE::PLIST_ELEMENT_DATA}
    };

    PLIST_ELEMENT_TYPE parseElement(QXmlStreamReader &reader, QString &str);
};

#endif // PLISTREADER_H
