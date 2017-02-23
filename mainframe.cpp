#include <QStandardItemModel>
#include <QStandardItem>
#include <QDir>

#include "mainframe.h"
#include "ui_mainframe.h"
#include "plistreader.h"
#include "jsonreader.h"

MainFrame::MainFrame(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainFrame)
{
    ui->setupUi(this);
}

MainFrame::~MainFrame()
{
    delete ui;
}

bool MainFrame::init()
{

    m_treeModel = new QStandardItemModel(this);
    ui->treeView->setModel(m_treeModel);

    QStringList sl;
    sl << tr("Key") << tr("Class") << tr("Value");
    m_treeModel->setHorizontalHeaderLabels(sl);

    PlistReader pr;
    pr.parseToModel(QDir::homePath() + QStringLiteral("/db/xml/config.plist"), m_treeModel);

//    JsonReader jr;
//    jr.parseToModel(QDir::homePath() + QStringLiteral("/db/json/2.json"), m_treeModel);

    return true;
}
