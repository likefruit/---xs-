#ifndef MFSQLTABLEMODEL_H
#define MFSQLTABLEMODEL_H

#include <QSqlTableModel>
#include <QDebug>
#include "mw.h"


class MFSqlTableModel;

void MFCreateModel(QString name, int role, QObject* parent);
MFSqlTableModel* MFGetModel(QString name, int role, QObject* parent = qApp);

class MFSqlTableModel : public QSqlTableModel
{
    Q_OBJECT
public:

    int _role;
    QString _name;

    enum
    {
        EDIT = 0,
        SHOW,
        QUERY,
        ALL
    };

    explicit MFSqlTableModel(QString name, int role, QObject *parent = 0);

    bool setFilterAndSelect(QString filter = QString());

    void setFilter(const QString &filter);

    bool updateRecordByValuesAndCommit(QMap<int, QVariant> values);

    bool updateRecordByValues(QMap<int, QVariant> values);

    bool insertRecordByValuesAndCommit(QStringList strs);

    bool insertRecordByValuesAndCommit(QList<QVariant> vars);

    bool insertRecordByValues(QList<QVariant> vars);

    QVariant getData(int row, int col);

    static QMap<QString, MFSqlTableModel*> _models[ALL];

    /* 取某一列的指，要distinct */
    QList<QVariant> getFieldData(QString col, bool distinct = true);

    int getRow(QMap<int, QVariant> filter);

    QList<QVariant> getFieldData(int col, bool distinct = true);

    static MFSqlTableModel* getModel(QString name, int role, QObject* parent);

    virtual bool deleteRowFromTable ( int row );

    virtual bool insertRowIntoTable ( const QSqlRecord & values );

    virtual bool updateRowInTable ( int row, const QSqlRecord & values );

signals:
    void sigDataChange();

public slots:

    void slotSelect()
    {
        select();
    }
};


class MFSqlTableModelDACC : public MFSqlTableModel
{
    Q_OBJECT
public:
    explicit MFSqlTableModelDACC(QString name, int role, QObject *parent = 0):
        MFSqlTableModel(name, role, parent)
    {

    }

signals:

public slots:
};

class MFSqlTableModelDBOM : public MFSqlTableModel
{
    Q_OBJECT
public:
    explicit MFSqlTableModelDBOM(QString name, int role, QObject *parent = 0) :
        MFSqlTableModel(name, role, parent)
    {
    }

protected:

    QSqlRecord convert(const QSqlRecord record)
    {
        QSqlRecord r(record);

        QString str = r.value(2).toString();
        if(str.length() == 0)
            return r;

        MFSqlTableModel* model = MFGetModel("DM", MFSqlTableModel::QUERY);
        model->setFilterAndSelect(QString("F1='%1'").arg(str));
        if(model->rowCount() == 0)
        {
            if(!model->insertRecordByValuesAndCommit(QList<QVariant>() << QVariant() << str))
            {
                mylog << "insert error";
            }
            model->setFilterAndSelect(QString("F1='%1'").arg(str));
        }
        r.setValue(DBOM_METERIAL, model->getData(0, 1));

        return r;
    }

    virtual bool insertRowIntoTable ( const QSqlRecord & values )
    {
        QSqlRecord r = convert(values);
        mylog << r;
        return MFSqlTableModel::insertRowIntoTable(r);
    }

    virtual bool updateRowInTable ( int row, const QSqlRecord & values )
    {
        return MFSqlTableModel::updateRowInTable(row, convert(values));
    }



signals:

public slots:

};

class MFSqlTableModelDCONF : public MFSqlTableModel
{
    Q_OBJECT
public:
    explicit MFSqlTableModelDCONF(QString name, int role, QObject *parent = 0) :
        MFSqlTableModel(name, role, parent)
    {

    }

signals:

public slots:
};

class MFSqlTableModelDD : public MFSqlTableModel
{
    Q_OBJECT
public:
    explicit MFSqlTableModelDD(QString name, int role, QObject *parent = 0) :
        MFSqlTableModel(name, role, parent)
    {

    }
signals:

public slots:
};

class MFSqlTableModelDFLOW : public MFSqlTableModel
{
    Q_OBJECT
public:
    explicit MFSqlTableModelDFLOW(QString name, int role, QObject *parent = 0) :
        MFSqlTableModel(name, role, parent)
    {
    }


signals:

public slots:
};

class MFSqlTableModelDLOG : public MFSqlTableModel
{
    Q_OBJECT
public:
    explicit MFSqlTableModelDLOG(QString name, int role, QObject *parent = 0) :
        MFSqlTableModel(name, role, parent)
    {

    }

signals:

public slots:
};


class MFSqlTableModelDM : public MFSqlTableModel
{
    Q_OBJECT
public:
    explicit MFSqlTableModelDM(QString name, int role, QObject *parent = 0) :
        MFSqlTableModel(name, role, parent)
    {

    }

signals:

public slots:
};

class MFSqlTableModelDOP : public MFSqlTableModel
{
    Q_OBJECT
public:
    explicit MFSqlTableModelDOP(QString name, int role, QObject *parent = 0) :
        MFSqlTableModel(name, role, parent)
    {

    }

signals:

public slots:
};

class MFSqlTableModelDPOSOP : public MFSqlTableModel
{
    Q_OBJECT
public:
    explicit MFSqlTableModelDPOSOP(QString name, int role, QObject *parent = 0) :
        MFSqlTableModel(name, role, parent)
    {

    }

signals:

public slots:
};

class MFSqlTableModelGDL : public MFSqlTableModel
{
    Q_OBJECT
public:
    explicit MFSqlTableModelGDL(QString name, int role, QObject *parent = 0) :
        MFSqlTableModel(name, role, parent)
    {

    }



signals:

public slots:

};

// MFSqlTableModelDORDER
class MFSqlTableModelDORDER : public MFSqlTableModel
{
    Q_OBJECT
public:
    explicit MFSqlTableModelDORDER(QString name, int role, QObject *parent = 0) :
        MFSqlTableModel(name, role, parent)
    {

    }

signals:

public slots:

};

class MFSqlTableModelGDW : public MFSqlTableModel
{
    Q_OBJECT
public:
    explicit MFSqlTableModelGDW(QString name, int role, QObject *parent = 0) :
        MFSqlTableModel(name, role, parent)
    {
    }

    Qt::ItemFlags flags(const QModelIndex &index) const
    {
        return MFSqlTableModel::flags(index) & ~Qt::ItemIsEditable;
    }


signals:

public slots:
};

int MFGetAccID(unsigned long cardno, QString& status, MFSqlTableModel* model = NULL);
void MFPrintFlowByID(QPrinter* print, int flowid);

#endif // MFSQLTABLEMODEL_H
