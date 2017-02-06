#ifndef MFTOOLBAR_H
#define MFTOOLBAR_H

#include <QWidget>
#include <QTreeWidget>
#include <QMap>
#include <QApplication>
#include <QDesktopWidget>
#include <QAction>
#include "mw.h"

class MFToolbarTitle: public QWidget
{
    Q_OBJECT
public:
    explicit MFToolbarTitle(QWidget *parent = 0):
        QWidget(parent)
    {

    }

    void paintEvent(QPaintEvent *)
    {
        QPainter p(this);

    //    p.drawRect(this->rect());
    //    p.fillRect(this->rect(), Qt::white);

        p.end();
    }

public slots:

};

class MFToolBar : public QWidget
{
    Q_OBJECT
public:
    explicit MFToolBar(QWidget *parent = 0):
        QWidget(parent)
    {
        _view = new QTreeWidget(this);
        _view->setHeaderHidden(true);
        _view->show();
        QFont font = _view->font();
     //   font.setWeight(1000);
    //    font.setBold(true);
     //   font.setPointSize(15);
        _view->setFont(font);
     //   connect(_view, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(itemChange(QTreeWidgetItem*,int)));
        connect(_view, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelChanged()));
    //    this->setMinimumWidth(qApp->desktop()->width()/5);

    }
    QTreeWidget* _view;
    QSize sizeHint() const
    {
        return QSize(this->fontMetrics().width("中")*20, 0);
    }

    void resizeEvent(QResizeEvent *)
    {
        QSize s = size();
        int m = 5;

        _view->setGeometry(m, 0, s.width()-m, s.height());
    }

    QTreeWidgetItem* addItem(QString str, QTreeWidgetItem* parent = NULL, QAction* act = NULL)
    {
        QStringList strList;
        strList.append(str);
        QTreeWidgetItem* item = new QTreeWidgetItem(strList);
        if(parent)
        {
            parent->addChild(item);
            if(act)
                _actions.insert(str, act);
        }
        else
        {
            _view->addTopLevelItem(item);
        }
        return item;
    }

    QMap<QString, QAction*> _actions;
    void triggleMenu(QString name)
    {
        _actions[name]->activate(QAction::Trigger);
    }
    
signals:
    
public slots:
    void itemSelChanged()
    {
        QTreeWidgetItem* item = _view->selectedItems().first();

        /* 如果有父节点，说明那个啥，这个节点是命令节点 */
        if(item->parent())
        {
            _actions[item->text(0)]->activate(QAction::Trigger);
        }
    }

    
};

#endif // MFTOOLBAR_H
