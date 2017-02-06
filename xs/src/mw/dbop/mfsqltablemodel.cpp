
#include "mfsqltablemodel.h"

MFSqlTableModel::MFSqlTableModel(QString name, int role, QObject *parent) :
    QSqlTableModel(parent)
{
    _role = role;
    _models[role].insert(name, this);
    setTable(name);
    _name = name;
    setEditStrategy(QSqlTableModel::OnManualSubmit);

    if(role == SHOW)
    {
        /* if this is show model, connect with edit model when data change */
        MFSqlTableModel* edit = MFGetModel(name, (int)EDIT);
        MFSqlTableModel* query = MFGetModel(name, (int)QUERY);
        connect(edit, SIGNAL(sigDataChange()), this, SLOT(slotSelect()));
        connect(edit, SIGNAL(sigDataChange()), query, SLOT(slotSelect()));
        connect(query, SIGNAL(sigDataChange()), this, SLOT(slotSelect()));
        connect(query, SIGNAL(sigDataChange()), edit, SLOT(slotSelect()));

        //
        connect(this, SIGNAL(sigDataChange()), edit, SLOT(slotSelect()));
        connect(this, SIGNAL(sigDataChange()), query, SLOT(slotSelect()));
    }

    /* if _role is query, always need query in other */
  //  if(_role != QUERY)
    if(_role == SHOW)
        select();
}


bool MFSqlTableModel::setFilterAndSelect(QString filter)
{
    setFilter(filter);
    return select();
}

void MFSqlTableModel::setFilter(const QString &filter)
{
    if(_role == SHOW)
    {
        mylog << "show model can not set filter";
        assert(0);
    }
    QSqlTableModel::setFilter(filter);
}

bool MFSqlTableModel::updateRecordByValuesAndCommit(QMap<int, QVariant> values)
{
    if(!updateRecordByValues(values))
        return false;
    return submitAll();
}

bool MFSqlTableModel::updateRecordByValues(QMap<int, QVariant> values)
{
    for(int i=0; i<rowCount(); i++)
    {
        foreach(int col, values.keys())
        {
            if(!setData(index(i, col), values[col]))
            {
                return false;
            }
        }
    }
    return true;
}

bool MFSqlTableModel::insertRecordByValuesAndCommit(QStringList strs)
{
    QList<QVariant> vars;
    foreach(QString str, strs)
    {
        vars << str;
    }
    return insertRecordByValuesAndCommit(vars);
}

bool MFSqlTableModel::insertRecordByValuesAndCommit(QList<QVariant> vars)
{
    if(!insertRecordByValues(vars))
        return false;

    if(submitAll())
        return true;

    this->revert();
    return false;
}

bool MFSqlTableModel::insertRecordByValues(QList<QVariant> vars)
{
    int cols = vars.count();
    QSqlRecord record;
    for(int i=0; i<cols; i++)
    {
        record.append(QSqlField(QString("F%1").arg(i)));
        record.setValue(i, vars.at(i));
    }
    if(!insertRecord(-1, record))
        return false;

    return true;
}

QVariant MFSqlTableModel::getData(int row, int col)
{
    return data(index(row, col));
}
/* 取某一列的指，要distinct */
QList<QVariant> MFSqlTableModel::getFieldData(QString col, bool distinct)
{
    return getFieldData(fieldIndex(col), distinct);
}

int MFSqlTableModel::getRow(QMap<int, QVariant> filter)
{
    int row, col;
    bool ok;
    for(row=0; row<rowCount(); row++)
    {
        ok = true;
        foreach(col, filter.keys())
        {
            if(data(index(row, col)) != filter[col])
            {
                ok = false;
                break;
            }
        }
        if(ok)
            break;
    }
    if(ok) return row;
    return -1;
}

QList<QVariant> MFSqlTableModel::getFieldData(int col, bool distinct)
{
    QList<QVariant> vars;
    for(int i=0; i<rowCount(); i++)
    {
        QVariant var = data(index(i, col));
        if(distinct && vars.indexOf(var) != -1)
            continue;
        vars.append(data(index(i, col)));
    }
    return vars;
}

MFSqlTableModel* MFSqlTableModel::getModel(QString name, int role, QObject* parent)
{
    if(name == "DORDER")
    {
        name = name;
    }
    if(_models[role].find(name) == _models[role].end())
    {
        MFCreateModel(name, role, parent);
    }
    return _models[role][name];
}

bool MFSqlTableModel::deleteRowFromTable ( int row )
{
    if(_role == SHOW)
    {
        assert(0);
    }
    bool ret = QSqlTableModel::deleteRowFromTable(row);
    if(ret)
        emit sigDataChange();
    return ret;
}

bool MFSqlTableModel::insertRowIntoTable ( const QSqlRecord & values )
{
    if(_role == SHOW)
    {
        assert(0);
    }
    bool ret = QSqlTableModel::insertRowIntoTable(values);
    if(ret)
        emit sigDataChange();
    return ret;
}

bool MFSqlTableModel::updateRowInTable ( int row, const QSqlRecord & values )
{
    if(_role == SHOW)
    {
        assert(0);
    }
    bool ret= QSqlTableModel::updateRowInTable(row, values);
    if(ret)
        emit sigDataChange();
    return ret;
}
