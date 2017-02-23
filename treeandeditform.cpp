#include <QStandardItemModel>
#include <QDebug>
#include <QFileInfo>
#include "treeandeditform.h"
#include "ui_treeandeditform.h"
#include "plistreader.h"
#include "jsonreader.h"

TreeAndEditForm::TreeAndEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TreeAndEditForm)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
}

TreeAndEditForm::~TreeAndEditForm()
{
    delete ui;
    qDebug() << "TreeAndEditForm destroied.";
}

bool TreeAndEditForm::init(const QString &name)
{
    m_treeModel = new QStandardItemModel(this);
    ui->treeView->setModel(m_treeModel);

    QStringList sl;
    sl << tr("Key") << tr("Class") << tr("Value");
    m_treeModel->setHorizontalHeaderLabels(sl);

    QString suffix = QFileInfo(name).suffix().toLower();
    if (suffix == QStringLiteral("json")) {
        JsonReader jr;
        jr.parseToModel(name, m_treeModel);
    } else if (suffix == QStringLiteral("plist")) {
        PlistReader pr;
        pr.parseToModel(name, m_treeModel);
    } else {
        Q_ASSERT(false);
        return false;
    }

    return true;
}
