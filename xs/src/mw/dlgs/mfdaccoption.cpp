#include "mfdaccoption.h"


MFDAccOption::MFDAccOption(bool already_open, QWidget *parent ) :
        QDialog(parent)
    {
        _already_open = already_open;
        if(_already_open)
            setWindowTitle("已开户卡");
        else
            setWindowTitle("新卡");
        _group = new QGroupBox("卡操作选择项：");
        initGroup();

    //    this->setWindowTitle("请选择");

        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(_group);

        QHBoxLayout* hLayout = new QHBoxLayout();
        layout->addLayout(hLayout);

        initCommands(hLayout);
        this->setLayout(layout);
    }

void MFDAccOption::initGroup()
{
    QVBoxLayout* layout = new QVBoxLayout();
    _group->setLayout(layout);

    _check1 = new QRadioButton("开卡");
    _check2 = new QRadioButton("挂失补卡");
    _check3 = new QRadioButton("充值");
    _check4 = new QRadioButton("退卡");
    _check5 = new QRadioButton("领补助");
    _check6 = new QRadioButton("退餐");

    _check5->setDisabled(true);
    _check6->setDisabled(true);

    if(_already_open)
    {
        _check1->setDisabled(true);
        _check2->setDisabled(true);
        _check3->setChecked(true);
    }
    else
    {
        _check3->setDisabled(true);
        _check4->setDisabled(true);
        _check1->setChecked(true);
    }

    layout->addWidget(_check1);
    layout->addWidget(_check2);
    layout->addWidget(_check3);
    layout->addWidget(_check4);
    layout->addWidget(_check5);
    layout->addWidget(_check6);

}
void MFDAccOption::initCommands(QHBoxLayout* layout)
{
    QPushButton* button = new QPushButton("确认");
    QPushButton* cancel = new QPushButton("取消");
    layout->addStretch(1);
    layout->addWidget(button);
    layout->addWidget(cancel);
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(button, SIGNAL(clicked()), this, SLOT(accept()));
}
