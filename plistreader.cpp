#include <QStandardItemModel>
#include <QStandardItem>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QXmlStreamReader>
#include "plistreader.h"

PlistReader::PlistReader()
{

}

void PlistReader::push(const QString &e)
{
    m_stack.push(e);
    if (e == PLIST_ARRAY) {
        m_indexes.push(0);
    }
}

void PlistReader::pop(const QString &e)
{
    QString p = m_stack.pop();
    Q_ASSERT(p == e);

    if (e == PLIST_ARRAY) {
        m_indexes.pop();
    }
}

void PlistReader::appendValue(QStandardItem *parent, QList<QStandardItem *> &row, const QString &t, const QString &v)
{
    if (!m_stack.isEmpty() && m_stack.top() == PLIST_ARRAY) {
        QString idx = QStringLiteral("%1").arg(m_indexes.top());
        m_indexes.top()++;
        row.append(new QStandardItem(idx));
    }
    row.append(new QStandardItem(t));
    row.append(new QStandardItem(v));
    parent->appendRow(row);
    row.clear();
}

QStandardItem* PlistReader::appendGroup(QStandardItem *parent, QList<QStandardItem *> &row, const QString &g)
{
    if (!m_stack.isEmpty() && m_stack.top() == PLIST_ARRAY) {
        QString idx = QStringLiteral("%1").arg(m_indexes.top());
        m_indexes.top()++;
        row.append(new QStandardItem(idx));
    }
    row.append(new QStandardItem(g));
    parent->appendRow(row);
    parent = row.first();
    row.clear();
    push(g);

    return parent;
}

PlistReader::PLIST_ELEMENT_TYPE PlistReader::parseElement(QXmlStreamReader &reader, QString &str)
{
    PlistReader::PLIST_ELEMENT_TYPE r = PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_NULL;
    QString name = reader.name().toString();

    auto i = ms_pType.find(name);
    if (ms_pType.cend() != i) {
        r = i.value();

        if (r == PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_KEY
                || r == PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_STRING
                || r == PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_NUMBER
                || r == PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_DATE
                || r == PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_DATA
                ) {
            str = reader.readElementText();
        } else {
            str = name;
        }
    }

    return r;
}

void PlistReader::parseToModel(const QString &plist, QStandardItemModel *model)
{
    QFile f(plist);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Q_ASSERT(false);
        return;
    }

    QStandardItem *parent = model->invisibleRootItem();
    QList<QStandardItem*> row;

    QXmlStreamReader reader;
    // 设置文件，这时会将流设置为初始状态
    reader.setDevice(&f);
    // 如果没有读到文档结尾，而且没有出现错误
    while (!reader.atEnd()) {
        // 读取下一个记号，它返回记号的类型
        QXmlStreamReader::TokenType type = reader.readNext();

        switch (type) {
        case QXmlStreamReader::StartDocument:
            qDebug() << reader.documentEncoding()
                     << reader.documentVersion();
            break;

        case QXmlStreamReader::StartElement: {
            QString n;
            PlistReader::PLIST_ELEMENT_TYPE r = parseElement(reader, n);

            switch (r) {
            case PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_ROOT:
                row.append(new QStandardItem(n));
                break;

            case PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_ARRAY:
                parent = appendGroup(parent, row, n);
                break;

            case PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_DICT:
                 parent = appendGroup(parent, row, n);
                break;

            case PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_KEY: {
                Q_ASSERT(m_stack.isEmpty() || m_stack.top() == PLIST_DICT);
                row.append(new QStandardItem(n));
                break;
            }

            case PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_BOOLEAN:
                appendValue(parent, row, PLIST_BOOLEAN, n);
                break;

            case PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_STRING:
                appendValue(parent, row, PLIST_STRING, n);
                break;

            case PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_NUMBER:
                appendValue(parent, row, PLIST_NUMBER, n);
                break;

            case PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_DATE:
                appendValue(parent, row, PLIST_DATE, n);
                break;

            case PlistReader::PLIST_ELEMENT_TYPE::PLIST_ELEMENT_DATA: {
                QByteArray ba = QByteArray::fromBase64(n.trimmed().toUtf8());
                n.clear();
                for (auto i = 0; i < ba.size(); ++i) {
                    n += QStringLiteral("%1").arg(uint8_t(ba.at(i)), 2, 16, QLatin1Char('0'));
                    if (((i + 1) % 4) == 0) {
                        n += " ";
                    }
                }
                appendValue(parent, row, PLIST_DATA, n);
                break;
            }

            default:
                Q_ASSERT(false);
                break;
            }
            break;
        }

        case QXmlStreamReader::EndElement: {
            QString n = reader.name().toString();
            if (n == PLIST_ARRAY || n == PLIST_DICT) {
                pop(n);
                parent = parent->parent();
                Q_ASSERT(parent || m_stack.isEmpty());
            }
            break;
        }
        case QXmlStreamReader::Characters:
            qDebug() << reader.text();
            break;

        case QXmlStreamReader::EndDocument:
            break;
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::EntityReference:
            qDebug() << reader.text();
            break;
        case QXmlStreamReader::NoToken:
            qDebug() << "NoToken";
            break;
        case QXmlStreamReader::Invalid:
            qDebug() << "Invalid" << "errorString is" << reader.errorString();
            break;
        default:
            Q_ASSERT(false);
        }
        // 如果读取过程中出现错误，那么输出错误信息
        if (reader.hasError()) {
            qDebug() << "error: " << reader.errorString();
        }
    }

    f.close();
}
