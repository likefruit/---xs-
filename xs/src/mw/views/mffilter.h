#ifndef MFFILTER_H
#define MFFILTER_H

#include <QWidget>
#include "mw.h"
#include "mfwidget.h"
#include "mfsqltablemodel.h"

class MFFilter : public QWidget
{
    Q_OBJECT
public:
    explicit MFFilter(QString dbName, QWidget *parent) :
        QWidget(parent)
    {
        _grid = new QGridLayout(this);
        _grid->setMargin(0);
        _grid->setColumnStretch(4, 1);

        _model = MFGetModel(dbName, MFSqlTableModel::SHOW);
        _modelEdit = MFGetModel(dbName, MFSqlTableModel::EDIT);
    }

    MFSqlTableModel* _model;
    MFSqlTableModel* _modelEdit;

    struct filter
    {
        MFAutoLineEdit* _1; /* and or */
        MFAutoLineEdit* _2; /* field */
        MFAutoLineEdit* _3; /* > = < like */
        MFAutoLineEdit* _4; /* filed selection, edit able */

        void initCombo1()
        {
            if(_1 == NULL)
                return;
            _1->addItem("并且");
            _1->addItem("或者");
        }
        void initCombo2(QMap<QString, QString> map)
        {
            _2->clear();
            _2->addItem("所有字段");
            _2->addItems(map.keys());
#if 0
            QCompleter* comp = new QCompleter(map.keys());
            comp->setCompletionMode(QCompleter::PopupCompletion);
            _2->setCompleter(comp);
#else
      //      _2->setAutoList(map.keys());
#endif
        }
        void initCombo3()
        {
            _3->addItem("模糊匹配");
            _3->addItem("=");
            _3->addItem(">");
            _3->addItem("<");
            _3->addItem(">=");
            _3->addItem("<=");
        }
        void initCombo4()
        {
        //    _4->setEditable(true);
        }

        filter(bool first, QMap<QString, QString> map)
        {
            if(!first)
            {
                _1 = new MFAutoLineEdit();
            }
            else
                _1 = NULL;
            _2 = new MFAutoLineEdit();
            _2->setMinimumSize(_2->sizeHint() + QSize(50, 0));
            _3 = new MFAutoLineEdit();
            _4 = new MFAutoLineEdit();
            _4->setMinimumSize(_4->sizeHint() + QSize(50, 0));
            _4->setFit(false);
            _4->setSort(true);

            initCombo1();
            initCombo2(map);
            initCombo3();
            initCombo4();
        }
    };

    QList<filter*> _filters;
    QGridLayout* _grid;
    QMap<QString, QString> _fieldMap;

    void addField(QString strHuman, QString strMachine)
    {
        if(_fieldMap.find(strHuman) != _fieldMap.end())
            return;

        _fieldMap.insert(strHuman, strMachine);
        foreach(filter* f, _filters)
        {
            f->_2->addItem(strHuman);
        }
    }

protected:
    QString getOneFilterString(filter* f)
    {
        QString fil;
        QString andor;
        QString field = f->_2->currentText();
        QString like = f->_3->currentText();
        QString val = f->_4->text();
        if(f->_1)
        {
            if(f->_1->currentText() == "并且")
                andor = " and ";
            else
                andor = " or ";
        }

        if(field == "所有字段")
        {
            like = " like ";
            val = "'%"+val+"%'";
            QMap<QString, QString>::iterator it = _fieldMap.begin();
            for(;it!=_fieldMap.end(); it++)
            {
                if(it.value().length() > 0)
                {
                    if(fil.length() > 0)
                    {
                        fil += " or ";
                    }

                    fil += (it.value() + like + val);
                }
            }
         }
        else
        {
            if(like == "模糊匹配")
            {
                like = " like ";
                val = "'%"+val+"%'";
            }
            else
            {
                like = " " + like + " ";
                val = "'"+val+"'";
            }
            field = _fieldMap[field];
            fil = field + like + val;
        }

        if(andor.length())
            return andor + " ( " + fil + " ) ";


        return " ( " + fil + " ) ";

    }

public:
    QString getFilterString()
    {
        QString fil;
        foreach(filter* f, _filters)
        {
            fil += getOneFilterString(f);
        }

        return fil;
    }

    void initF4ComboAllField(filter* f)
    {
        QMap<QString, void*> strList;
        foreach(QString fieldHuman, _fieldMap.keys())
        {
            QString fieldName = _fieldMap[fieldHuman];
            mylog << fieldName;
            QList<QVariant> vars = _model->getFieldData(fieldName);

            foreach(QVariant var, vars)
            {
                if(var.type() != QVariant::DateTime)
                {
                    strList.insert(var.toString(), NULL);
                }
                else
                {
                    strList.insert( var.toDateTime().toString("yyyy-MM-dd hh:mm:ss"), NULL);
                //    f->_3->setDisabled(true);
                }
            }
        }
#if 0
        QCompleter* comp = new QCompleter(strList);
        comp->setCompletionMode(QCompleter::PopupCompletion);
        f->_4->setCompleter(comp);
#else
        f->_4->setAutoList(strList.keys());
#endif
    }

    void initF4Combo(filter* f, QString str)
    {
        f->_4->clear();
        f->_3->setDisabled(false);
        if(str == "所有字段")
        {
            mylog << "all field";
            return initF4ComboAllField(f);
        }


        QString fieldName = _fieldMap[str];
        QList<QVariant> vars = _model->getFieldData(fieldName);

        /* all fields */
        if(fieldName.length() == 0)
        {
            return;
        }

        QStringList strList;
        foreach(QVariant var, vars)
        {
            if(var.type() != QVariant::DateTime)
            {
                strList << var.toString();
            }
            else
            {
                strList << var.toDateTime().toString("yyyy-MM-dd hh:mm:ss");
            //    f->_3->setDisabled(true);
            }
        }
        f->_4->setAutoList(strList);
    //    f->_4->addItems(strList);
    }

signals:
    void sigDataChange();
    
public slots:
    void slotFilterChange()
    {
        _modelEdit->setFilter(getFilterString());
        if(!_modelEdit->select())
        {
            mylog << "error*************" << getFilterString();
        }
        else
        {
            mylog << "ok******************" << getFilterString();
        }
        emit sigDataChange();
    }

    /* when filter field change, need init combobox */
    void filterFieldChange(QString str)
    {
        foreach(filter* f, _filters)
        {
            if(f->_2 == sender())
            {
                initF4Combo(f, str);
                break;
            }
        }
    }

    void deleteFilter()
    {
        if(_filters.count() == 0)
            return;
        filter* f = _filters.at(_filters.count()-1);
        _filters.removeLast();
        delete f->_1;
        delete f->_2;
        delete f->_3;
        delete f->_4;
        delete f;
        if(_filters.size() > 0)
        {
            f = _filters.at(_filters.size() -1);
            f->_4->setFocus();
        }

        slotFilterChange();
    }

    MFFilter::filter* addFilter()
    {
        int count = _filters.count();
        filter* f = new filter(count == 0, this->_fieldMap);
        if(count != 0)
            _grid->addWidget(f->_1, count, 0);
        _grid->addWidget(f->_2, count, 1);
        _grid->addWidget(f->_3, count, 2);
        _grid->addWidget(f->_4, count, 3);
        _filters.append(f);
        if(f->_1)
            connect(f->_1, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotFilterChange()));
        connect(f->_2, SIGNAL(currentIndexChanged(QString)), this, SLOT(filterFieldChange(QString)));
        connect(f->_3, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotFilterChange()));
        //connect(f->_4, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotFilterChange()));
        connect(f->_4, SIGNAL(textChanged(QString)), this, SLOT(slotFilterChange()));
        //f->_4->setFocus();
        initF4Combo(f, f->_2->currentText());

        slotFilterChange();

        return f;
    }
};

#endif // MFFILTER_H
