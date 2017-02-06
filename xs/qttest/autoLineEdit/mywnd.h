#ifndef MYWND_H
#define MYWND_H

#include <QWidget>
#include <QLineEdit>
#include <QListView>
#include <QStringList>
#include <QStringListModel>
#include <QKeyEvent>
#include <QModelIndex>
#include <QComboBox>
#include <QApplication>
#include <QDebug>
#include <QList>
#include <QVariant>

#define mylog QList<QVariant>() << __FILE__ << __LINE__
#define mylog1 qDebug() << __FILE__ << __LINE__

class MyWnd : public QWidget
{
    Q_OBJECT
public:
    explicit MyWnd(QWidget *parent = 0);

    void moveEvent(QMoveEvent *)
    {
        mylog1 << "aaaaaaa";
    }
signals:
    
public slots:
    
};

class MFAutoLineEdit : public QLineEdit
{
    Q_OBJECT
public:

    MFAutoLineEdit(QStringList autoList, QWidget *parent = 0) : QLineEdit(parent)
    {
        setStringList(autoList);
        _listView = new QListView(this);
        _listModel = new QStringListModel(this);
        _listView->setModel(_listModel);

        _listView->setWindowFlags(Qt::ToolTip);

        connect(_listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(slotListViewClicked(const QModelIndex &)));
    }

    void setStringList(QStringList autoList)
    {
        _autoList = autoList;
    }

    QListView* _listView;
    QStringListModel* _listModel;
    QStringList _autoList;

    void focusInEvent(QFocusEvent *ev)
    {
        slotTextChange(text());
        QLineEdit::focusInEvent(ev);
        qApp->installEventFilter(this);
    }

    bool eventFilter(QObject *obj, QEvent *ev)
    {
        if(ev->type() == QEvent::Move)
        {
            QPoint pt(0, height());
            pt = mapToGlobal(pt);
            pt.setY(pt.y()+1);

            if(!_listView->isHidden())
            _listView->move(pt);

        }
        return QLineEdit::eventFilter(obj, ev);
    }

    void focusOutEvent(QFocusEvent *ev)
    {
        QLineEdit::focusOutEvent(ev);
        qApp->postEvent(this, new QEvent((QEvent::Type)(QEvent::User+100)));
        qApp->removeEventFilter(this);
    }

    bool event(QEvent *ev)
    {
        if(ev->type() == (QEvent::Type)(QEvent::User+100) && !_listView->hasFocus())
        {
            mylog << "hide";
            _listView->hide();
            return true;
        }
        if(ev->type() == (QEvent::Type)(QEvent::User+101))
        {
            slotTextChange(text());
            return true;
        }
        if(ev->type() == (QEvent::Type)(QEvent::User+102))
        {
            _listView->hide();
            return true;
        }
        return QLineEdit::event(ev);
    }

    void keyPressEvent(QKeyEvent *ev)
    {
        int count = _listView->model()->rowCount();
        QModelIndex index = _listView->currentIndex();
        int key = ev->key();

        int row;

        if(key == Qt::Key_Down)
        {
            if(!_listView->isHidden())
            {
                row = index.row() + 1;
                row %= count;
                index = _listView->model()->index(row, 0);
                _listView->setCurrentIndex(index);
            }
        }
        else if(key == Qt::Key_Up)
        {
            if(!_listView->isHidden())
            {
                row = index.row() - 1;
                if(row < 0) row = count - 1;

                index = _listView->model()->index(row, 0);
                _listView->setCurrentIndex(index);
            }
        }
        else if(key == Qt::Key_Escape || key == Qt::Key_Tab)
        {
            if(!_listView->isHidden())
            {
                mylog << "hide";
                _listView->hide();
            }
        }
        else if(key == Qt::Key_Enter || key == Qt::Key_Return)
        {
            if(!_listView->isHidden())
                slotListViewClicked(index);
        }
        else
        {
            qApp->postEvent(this, new QEvent((QEvent::Type)(QEvent::User+101)));
        }

        QLineEdit::keyPressEvent(ev);
    }

signals:

public slots:
    void slotTextChange(const QString& str)
    {
        if(str.length() == 0)
        {
            mylog << "hide";
            _listView->hide();
            return;
        }

        QStringList avalidList;
        foreach(QString value, _autoList)
        {
            if(value.indexOf(str) != -1)
            {
                mylog << str << value;
                avalidList << value;
            }
            else
            {
                mylog << str << value;
            }
        }

        int len = avalidList.length();
        if(len == 0 || (len == 1 && avalidList.at(0) == str))
        {
            mylog << "hide";
            _listView->hide();
            return;
        }

        mylog << "aaa";

        _listModel->setStringList(avalidList);
        _listView->setModel(_listModel);
        _listView->setMinimumWidth(this->width());
        _listView->setMaximumWidth(this->width());

        QPoint pt(0, height());
        pt = mapToGlobal(pt);
        pt.setY(pt.y()+1);

        _listView->move(pt);

        if(_listView->isHidden())
        {
            _listView->show();
        }
    }

    void slotSelectionChanged()
    {
        slotTextChange(text());
    }

    void slotListViewClicked(const QModelIndex & index)
    {
        QString text = index.data().toString();

        setText(text);
        qApp->postEvent(this, new QEvent((QEvent::Type)(QEvent::User+102)));
    }

};

#endif // MYWND_H
