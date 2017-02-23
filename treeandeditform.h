#ifndef TREEANDEDITFORM_H
#define TREEANDEDITFORM_H

#include <QWidget>

namespace Ui {
class TreeAndEditForm;
}

class QStandardItemModel;

class TreeAndEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit TreeAndEditForm(QWidget *parent = 0);
    ~TreeAndEditForm();

    bool init(const QString &name);

private:
    Ui::TreeAndEditForm *ui;

    QStandardItemModel *m_treeModel = Q_NULLPTR;
};

#endif // TREEANDEDITFORM_H
