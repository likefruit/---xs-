#include "mfgdwcopy.h"
#include "mfsqltablemodel.h"

MFGDWCopy::MFGDWCopy(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QGroupBox* box = new QGroupBox("设置拷贝条件");
    QHBoxLayout* hLayout = new QHBoxLayout();

    QPushButton* ok, *cancel;

    layout->addWidget(box);
    layout->addLayout(hLayout);
    hLayout->addStretch(1);
    hLayout->addWidget(ok = new QPushButton("执行拷贝"));
    hLayout->addWidget(cancel = new QPushButton("退出"));

    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ok, SIGNAL(clicked()), this, SLOT(copy()));

    hLayout = new QHBoxLayout();
    box->setLayout(hLayout);

    hLayout->addWidget(new QLabel("从"));
    hLayout->addWidget(_from = new MFAutoLineEdit()); _from->setSort();
    hLayout->addWidget(new QLabel("复制到"));
    hLayout->addWidget(_to = new MFAutoLineEdit()); _to->setSort();

    _to->addItems(MFConf()[DCONF_WF]);


    connect(_from, SIGNAL(currentIndexChanged(QString)), this, SLOT(setToList(QString)));
    connect(_from->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(setToList(QString)));
    connect(_from, SIGNAL(editTextChanged(QString)), this, SLOT(setToList(QString)));

    _from->addItems(MFConf()[DCONF_WF]);
}

void MFGDWCopy::setToList(QString from)
{
    _to->clear();
    QStringList toList = MFConf()[DCONF_WF];
    foreach(QString to, toList)
    {
        if(to != from)
        {
            _to->addItem(to);
        }
    }
}

void MFGDWCopy::showEvent(QShowEvent *)
{
    _from->clear();

    MFSqlTableModel* model = MFGetModel("GDW", MFSqlTableModel::QUERY);
    model->setFilterAndSelect();
    QList<QVariant> vars = model->getFieldData(GDW_WEEK);
    foreach(QVariant var, vars)
    {
        _from->addItem(var.toString());
    }
}

void MFGDWCopy::copy()
{
    if(_from->text() == _to->text())
    {
        QMessageBox::warning(this, "错误", "不能自己拷贝自己");
        return;
    }

    QList< QList<QVariant> > lists;
    MFSqlTableModel* model = MFGetModel("GDW", MFSqlTableModel::QUERY);
    model->setFilterAndSelect(FIELDEQUAL(GDW_WEEK, _from->currentText()));
    for(int i=0; i<model->rowCount(); i++)
    {
        QString day = model->getData(i, GDW_DAY).toString();
        QString time = model->getData(i, GDW_TIME).toString();
        QString food = model->getData(i, GDW_FOOD).toString();

       lists << (QList<QVariant>()
                                             << QVariant()
                                             << _to->currentText()
                                             << day
                                             << time
                                             << food);
    }

    while(lists.count())
    {
        model->insertRecordByValuesAndCommit(lists.at(0));
        lists.removeFirst();
    }
    QMessageBox::warning(this, "结束", "周食谱拷贝结束");
}
