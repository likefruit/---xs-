#ifndef MFWIDGET_H
#define MFWIDGET_H

#include <QWidget>
#include "mw.h"
#include "mfsqltablemodel.h"
#include <QLineEdit>
#include <QComboBox>
#if 0
#define SUPPORT_AUTO_POP 1

class MFAutoLineEdit : public QLineEdit
{
    Q_OBJECT
public:
#if SUPPORT_AUTO_POP
    MFAutoLineEdit(bool autoPop = false, QWidget *parent = 0) : QLineEdit(parent)
    {
        _autoPop = autoPop;
        _listView = new QListView(this);
        _listModel = new QStringListModel(this);
        _listView->setModel(_listModel);

        _listView->setWindowFlags(Qt::ToolTip);

        connect(_listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(slotListViewClicked(const QModelIndex &)));
        this->installEventFilter(this);
    }

    QString currentText()
    {
        return text();
    }

    void setAutoPop(bool autoPop = true)
    {
        _autoPop = autoPop;
    }
    bool _autoPop;
#else
    MFAutoLineEdit(/*bool autoPop = false, */QWidget *parent = 0) : QLineEdit(parent)
    {
        _listView = new QListView(this);
        _listModel = new QStringListModel(this);
        _listView->setModel(_listModel);

        _listView->setWindowFlags(Qt::ToolTip);

        connect(_listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(slotListViewClicked(const QModelIndex &)));
        this->installEventFilter(this);
    }
#endif
    void setAutoList(QStringList autoList)
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
        this->setSelection(0,-1);

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
        if(obj == this && ev->type() == QEvent::FocusOut && _autoPop)
        {
            if(!this->_listView->isHidden())
            {
                qApp->postEvent(this, new QEvent((QEvent::Type)(QEvent::User+100)));
                return true;
            }
        }

        return QLineEdit::eventFilter(obj, ev);
    }

    void focusOutEvent(QFocusEvent *ev)
    {
        mylog << "focus out now .............................";
        qApp->postEvent(this, new QEvent((QEvent::Type)(QEvent::User+100)));
        QLineEdit::focusOutEvent(ev);

        qApp->removeEventFilter(this);
    }

    bool event(QEvent *ev)
    {
        if(ev->type() == (QEvent::Type)(QEvent::User+100) && !_listView->hasFocus())
        {
            _listView->hide();
            mylog << "list view hide";
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
            mylog << "list view hide";
            return true;
        }
        else
        {
            return QLineEdit::event(ev);
        }

    }

    void keyPressEvent(QKeyEvent *ev)
    {
        int count = _listView->model()->rowCount();
        QModelIndex index = _listView->currentIndex();
        int key = ev->key();

        int row;

        if(key == Qt::Key_Down)
        {
            if(_listView->isHidden())
            {
                int cur = _autoList.indexOf(text());
                cur++;
                if(cur != _autoList.count())
                {
                    setText(_autoList.at(cur));
                }
            }
            else
            {
                row = index.row() + 1;
                row %= count;
                index = _listView->model()->index(row, 0);
                _listView->setCurrentIndex(index);
            }
        }
        else if(key == Qt::Key_Up)
        {
            if(_listView->isHidden())
            {
                int cur = _autoList.indexOf(text());
                cur--;
                if(cur >= 0)
                {
                    setText(_autoList.at(cur));
                }
            }
            else
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
                mylog << "list view hide";
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
protected:
    void setCompleter(QCompleter*)
    {}

signals:

public slots:
    void slotTextChange(const QString& str)
    {
#if SUPPORT_AUTO_POP
        if(str.length() == 0 && !_autoPop)
#else
        if(str.length() == 0)
#endif
        {
            mylog << "list view hide";
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
        if(len == 0 /*|| (len == 1 && avalidList.at(0) == str)*/)
        {
            mylog << "list view hide";
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
        if(index.row() >= 0)
        {
            QString text = index.data().toString();
            setText(text);
        }
        qApp->postEvent(this, new QEvent((QEvent::Type)(QEvent::User+102)));
    }

};
#else
#define SUPPORT_AUTO_POP 0
class MFCompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit MFCompleter(const QStringList& completions, QObject *parent = 0) : QCompleter(completions, parent)
    {}
};

class MFAutoLineEdit : public QComboBox
{
    Q_OBJECT
public:
    explicit MFAutoLineEdit(QWidget* parent = 0) : QComboBox(parent)
    {
        this->setEditable(true);
        _sort = false;
        _must_fit = true;
    }
#if 0
    explicit MFAutoLineEdit(bool sort, bool must_fit, QWidget* parent = 0) : QComboBox(parent)
    {
        this->setEditable(true);
        _sort = sort;
        _must_fit = must_fit;
    }
#endif

    void setFit(bool f = true)
    {
        _must_fit = f;
    }
    void setSort(bool s = true)
    {
        _sort = s;
    }

    void setReadOnly(bool ro)
    {
        if(ro) this->setDisabled(true);
        else this->setDisabled(false);
    }

    bool _sort;
    QStringList _strList;
    bool _must_fit;

    void focusOutEvent(QFocusEvent *e)
    {
        if(_must_fit)
        {
            QString t = text();
            if(_strList.indexOf(t) == -1)
            {
                if(_strList.count() > 0)
                    this->setText(_strList.at(0));
                else
                    this->setText("");
            }
        }
        QComboBox::focusOutEvent(e);
    }

    void setAutoList()
    {
        if(_sort)
            _strList.sort();
        QComboBox::clear();
        MFCompleter* comp = new MFCompleter(_strList);
        comp->setCompletionMode(QCompleter::PopupCompletion);
        setCompleter(comp);
        QComboBox::addItems(_strList);
    }

    void setAutoList(QStringList strList)
    {
        _strList = strList;
        setAutoList();
    }

    void setText(QString str)
    {
        this->lineEdit()->setText(str);
    }

    QString text()
    {
        return this->lineEdit()->text();
    }

    void addItems(QStringList texts)
    {
        _strList << texts;
        setAutoList();
    }

    void clear()
    {
        _strList.clear();
        QComboBox::clear();
    }

    void addItem(QString text)
    {
        _strList << text;
        setAutoList();
    }
};
#endif

#if 0
class MFAutoListCombo : public QComboBox
{
    Q_OBJECT
public:

    MFAutoListCombo(QWidget *parent = 0) : QComboBox(parent)
    {
        _lineEdit = new MFAutoLineEdit(this);
        setLineEdit(_lineEdit);
    }

    MFAutoLineEdit* _lineEdit;
    void setAutoList(QStringList strList)
    {
        _lineEdit->setAutoList(strList);
    }

    void showPopup()
    {
    //    _lineEdit->_listView->hide();
        QComboBox::showPopup();
    }

    QString text()
    {
        return _lineEdit->text();
    }

protected:
    void setCompleter(QCompleter*)
    {}
};

#endif
class MFWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MFWidget(QWidget *parent = 0):
        QWidget(parent)
    {
         qApp->postEvent(this, new QEvent(EV_CREATE));
    }

    virtual void OnCreate()
    {}

    bool event(QEvent *e)
    {
        if(e->type() == EV_CREATE)
        {
            OnCreate();

            return true;
        }
        return QWidget::event(e);
    }

signals:
    
public slots:
    
};

class MFFilterWidget : public MFWidget
{
    Q_OBJECT
public:
    explicit MFFilterWidget(QString filter, QWidget* parent = 0) : MFWidget(parent)
    {
        _filter_string = filter;
    }

    virtual QString printLabel() = 0;
    virtual QString filter() = 0;
    virtual void reloadList() = 0;

    MFAutoLineEdit* _box;
    MFAutoLineEdit* _boxHigh;
    QString _filter_string;
};

class MFSingleFilter : public MFFilterWidget
{
    Q_OBJECT
public:
    explicit MFSingleFilter(QString tableName, QString field, QString title, QString filter_string, QWidget *parent = 0):
        MFFilterWidget(filter_string, parent)
    {
        _layout = new QHBoxLayout(this);
        _layout->addWidget(_label = new QLabel(title));
        _layout->addWidget(_box = new MFAutoLineEdit()); _box->setFit();
        _layout->setMargin(0);
        _table = tableName;
        _field = field;


        connect(_box, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(selectChange(QString)));
        connect(_box, SIGNAL(editTextChanged(QString)), this, SIGNAL(selectChange(QString)));

        initComboBox(_box);
    }

    void reloadList()
    {
        initComboBox(_box);
    }

    void initComboBox(MFAutoLineEdit* box)
    {
        box->clear();
        box->setEditable(true);
        MFSqlTableModel* model = MFGetModel(_table, MFSqlTableModel::QUERY);
        model->setFilterAndSelect(_filter_string);

        QList<QVariant> vars = model->getFieldData(_field);
        QStringList strList;


        foreach(QVariant var, vars)
        {
            strList << var.toString();
        }

        strList.sort();
        strList.insert(0, MW_ALL);

#if 0
        box->addItems(strList);
        QCompleter* comp = new QCompleter(strList);
        comp->setCompletionMode(QCompleter::PopupCompletion);
        box->setCompleter(comp);
#endif
        box->setAutoList(strList);
    }

    QHBoxLayout* _layout;
    QLabel* _label;


    QString _table;
    QString _field;

    QString printLabel()
    {
        /*if(_box->currentText() == 0)
            return _label->text() + "全部";*/
        return _label->text() + _box->currentText();
    }

    QString filter()
    {
        if(_box->currentText() == MW_ALL)
            return QString();
        return QString("(%1.%2='%3')").arg(_table, _field, _box->currentText());
    }

signals:
    void selectChange(QString );

public slots:

};

class MFRangeFilter : public MFSingleFilter
{
    Q_OBJECT
public:

    explicit MFRangeFilter(QString tableName, QString field, QString title, QString filter_string, QWidget *parent = 0):
        MFSingleFilter(tableName, field, title, filter_string, parent)
    {
        _layout->addWidget(new QLabel("到"));
        _layout->addWidget(_boxHigh = new MFAutoLineEdit()); _boxHigh->setFit();
        initComboBox(_boxHigh);

        connect(_boxHigh, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(selectChange(QString)));
        connect(_boxHigh, SIGNAL(editTextChanged(QString)), this, SIGNAL(selectChange(QString)));
    }

    void reloadList()
    {
        initComboBox(_box);
        initComboBox(_boxHigh);
    }

    QString printLabel()
    {
        if(_box->currentText() == MW_ALL || _boxHigh->currentText() == MW_ALL)
            return _label->text() + "全部";
        return _label->text() + "从 "+_box->currentText() + " 到 " + _boxHigh->currentText();
    }
    QString filter()
    {
        if(_box->currentText() == MW_ALL || _boxHigh->currentText() == MW_ALL)
            return QString();
        return QString("(%1.%2>='%3' and %4.%5 <= '%6' )").arg(_table, _field, _box->currentText(),_table, _field, _boxHigh->currentText());
    }
};

#if 0
class MFTimeFilter : public MFFilterWidget
{
    Q_OBJECT
public:
    explicit MFTimeFilter(QString title, QWidget *parent = 0):
        MFFilterWidget(parent)
    {
        _layout = new QHBoxLayout(this);
        _layout->addWidget(_label = new QLabel(title));
    }

    QHBoxLayout* _layout;
    QLabel* _label;
};
#endif


#endif // MFWIDGET_H
