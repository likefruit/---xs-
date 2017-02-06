#ifndef MFWORKAREA_H
#define MFWORKAREA_H

#include <QWidget>
#include "mfwidget.h"
#include "mw.h"
#include "mffilter.h"
#include "mftableview.h"

class MFWorkFilter : public QGroupBox
{
    Q_OBJECT
public:
    explicit MFWorkFilter(QString tableName) : QGroupBox(MW_FILTER_TITLE)
    {
        setLayout(_filterBoxLayout = new QVBoxLayout());
        _filterBoxLayout->addLayout(_filterCmdLayout = new QHBoxLayout());
        _filterBoxLayout->addWidget(_filter = new MFFilter(tableName, this));

        _filterCmdLayout->addWidget(_addFilter = new QPushButton("增加过滤条件"));
        _filterCmdLayout->addWidget(_delFilter = new QPushButton("删除过滤条件"));
        _filterCmdLayout->addStretch(1);
        connect(_addFilter, SIGNAL(clicked()), this, SIGNAL(addFilter()));
        connect(_delFilter, SIGNAL(clicked()), _filter, SLOT(deleteFilter()));
    }

    QVBoxLayout* _filterBoxLayout;
    /* and than is button 增加过滤条件，减少过滤条件 */
    QHBoxLayout* _filterCmdLayout; QPushButton* _addFilter; QPushButton* _delFilter;
    /* and than is filter */
    MFFilter* _filter;

signals:
    void addFilter();
};

class MFWorkArea : public MFWidget
{
    Q_OBJECT
public:
    explicit MFWorkArea(QWidget* setup, QWidget* workArea, QWidget *parent = 0) :
        MFWidget(parent)
    {
        _title1 = new QLabel(this);
        QVBoxLayout* layout = new QVBoxLayout(this);
        _mainLayout = layout;
     //   _mainLayout->setMargin(0);

    #ifdef WIN32
        _mainLayout->setSpacing(_mainLayout->spacing()*2);
    #endif

        layout->addWidget(_title1);


        if(setup)
        {
            mylog << "setup is not null";
            layout->addWidget(setup);
            setup->show();
        }
        else
        {
            mylog << "setup is null";
        }

       // layout->addWidget(_table = new MFTableView(tableName, this), 1);
        layout->addWidget(workArea, 1);

        layout->addWidget(_commandBox = new QGroupBox("操作命令"));
        _commandBox->setLayout(_bottomLayout = new QHBoxLayout());

        _bottomLayout->addLayout(_bottomLeftLayout = new QHBoxLayout());
        _bottomLayout->addStretch(1);
        _bottomLayout->addLayout(_bottomMiddleLayout = new QHBoxLayout());
        _bottomLayout->addLayout(_bottomRightLayout = new QHBoxLayout());
    }

    QLabel* _title1;
    void setTitle(QString str)
    {
        _title1->setText(QString("<H2><u>%1</u></H2>").arg(str));
    }

    QVBoxLayout* _mainLayout;
    /* first is _title*/

 //   QWidget* _setup;
    /* and than is table view */
 //   MFTableView* _table;
    /* and than is QHBoxLayout for button*/
    QGroupBox* _commandBox;

    QHBoxLayout* _bottomLayout;
    QHBoxLayout* _bottomLeftLayout;
    QHBoxLayout* _bottomMiddleLayout;  /* for subclass */
    QHBoxLayout* _bottomRightLayout;  /* for subclass */

signals:
    
public slots:
};

#endif // MFWORKAREA_H
