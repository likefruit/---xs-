#ifndef MFTABLEITEMDELEGATE_H
#define MFTABLEITEMDELEGATE_H

#include <QItemDelegate>
#include "mw.h"
#include "mfsqltablemodel.h"
#include "mfwidget.h"

class MFItemDelegateReadOnly : public QItemDelegate
{
    Q_OBJECT
public:
    explicit MFItemDelegateReadOnly(QObject *parent = 0):
        QItemDelegate(parent)
    {}

    QWidget *createEditor(QWidget*parent, const QStyleOptionViewItem &,
            const QModelIndex &) const
    {
        QLineEdit* edit = new QLineEdit(parent);
        edit->setReadOnly(true);
        return edit;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QVariant var = index.model()->data(index);
        QLineEdit *combo = static_cast<QLineEdit*>(editor);
        combo->setText(var.toString());
    }
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QVariant var;
        QLineEdit *combo = static_cast<QLineEdit*>(editor);

        var = combo->text();
        model->setData(index, var, Qt::EditRole);
    }

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
    {
        editor->setGeometry(option.rect);
    }
};

MFItemDelegateReadOnly* readOnlyDelegate();

class MFItemDelegateCannotModify : public QItemDelegate
{
    Q_OBJECT
public:
    MFItemDelegateCannotModify(int col = 0, int value = 0, QObject* parent = 0) : QItemDelegate(parent)
    {
        _col = col;
        _value = value;
    }

    int _col;
    int _value;

    QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem &,
            const QModelIndex &index) const
    {
        QLineEdit* w = new QLineEdit(parent);
        const QAbstractItemModel* model = index.model();
        if(model->data(model->index(index.row(), _col)).toInt() != _value)
        {
            w->setReadOnly(true);
        }
        return w;
    }
};

class MFItemDelegateAutoEdit : public QItemDelegate
{
    Q_OBJECT
public:
    MFItemDelegateAutoEdit(QStringList list, bool modify_enable = true, QObject *parent = 0):
        QItemDelegate(parent)
    {
        _list = list;
        _modify_enable = modify_enable;
    }

    MFItemDelegateAutoEdit(QString dbName, int field, QObject* parent = 0) : QItemDelegate(parent)
    {
        _dbName = dbName;
        _field = field;
        _modify_enable = true;
    }

    QStringList _list;
    QString _dbName;
    int _field;
    bool _modify_enable;

    QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem &,
            const QModelIndex &index) const
    {

        QStringList strList = _list;
#if SUPPORT_AUTO_POP
        MFAutoLineEdit* combo = new MFAutoLineEdit(true, parent);
#else
        MFAutoLineEdit* combo = new MFAutoLineEdit(parent);
        combo->setFit(false);
#endif
        if(strList.count() == 0)
        {
            MFSqlTableModel* model = MFGetModel(_dbName, MFSqlTableModel::SHOW);
            QList<QVariant> vars = model->getFieldData(_field);
            foreach(QVariant var, vars)
            {
                strList << var.toString();
            }
        }

        combo->setAutoList(strList);

        if(!_modify_enable)
        {
            const QAbstractItemModel* model = index.model();
            if(model->data(model->index(index.row(), 0)).toInt() != 0)
                combo->setReadOnly(true);
        }

        return combo;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QVariant var = index.model()->data(index);
        MFAutoLineEdit *combo = static_cast<MFAutoLineEdit*>(editor);
        combo->setText(var.toString());
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QVariant var;
        MFAutoLineEdit *combo = static_cast<MFAutoLineEdit*>(editor);

        var = combo->text();
        model->setData(index, var, Qt::EditRole);
    }

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
    {
        editor->setGeometry(option.rect);
    }

};


#endif // MFTABLEITEMDELEGATE_H
