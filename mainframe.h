#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QMainWindow>

namespace Ui {
class MainFrame;
}

class QStandardItemModel;

class MainFrame : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainFrame(QWidget *parent = 0);
    ~MainFrame();

    bool init();

private:
    Ui::MainFrame *ui;

    QStandardItemModel *m_treeModel = Q_NULLPTR;
};

#endif // MAINFRAME_H
