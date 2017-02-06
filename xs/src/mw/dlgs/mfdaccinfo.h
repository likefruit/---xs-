#ifndef MFDACCINFO_H
#define MFDACCINFO_H

#include <QDialog>
#include "mw.h"
#include "mfsqltablemodel.h"
#include "mftableview.h"
class MFDAccInfo : public QDialog
{
    Q_OBJECT
public:
    QLineEdit* _01;
    QLineEdit* _02;
    QLineEdit* _03;
    MFAutoLineEdit* _04;
    MFAutoLineEdit* _05;
    QLineEdit* _06;
    QLineEdit* _07;
    MFAutoLineEdit* _08;
    QLineEdit* _09;
    QLineEdit* _10;
    MFAutoLineEdit* _11;
    QDoubleSpinBox* _12;
    QLineEdit* _13;
    QLineEdit* _14;
    MFAutoLineEdit* _15;
    QDoubleSpinBox* _16;
    QLineEdit* _17;

    void charge_supply(double money);
    double calc_total_cost_in_month(int accid, QString month);

    unsigned char* _buf;
    unsigned long _cardno;
    int _accid;

    QGroupBox* _infoBox;

    QGroupBox* _commandBox;
    int _flowId;

    /*  */
    QLabel* _transType;
    QDoubleSpinBox* _trans;

    QPushButton* _ok;
    QPushButton* _cancel;
    QHBoxLayout* _hLayout;

    explicit MFDAccInfo(QWidget *parent = 0);
    virtual void OnOK() = 0;

    void setControl(int accid);

    void printTicket(int flowid)
    {
        _flowId = flowid;
        QPrintDialog dlg;
        connect(&dlg, SIGNAL(accepted(QPrinter*)), this, SLOT(print(QPrinter*)));
        dlg.exec();
    }

    int change_balance(QString op, QString orderid, double trans_value, bool use_arg);

public slots:
    void handle()
    {
        OnOK();
    }

    void print(QPrinter* print)
    {
        MFPrintFlowByID(print, _flowId);
    }
};

class MFDAccOpen : public MFDAccInfo
{
    Q_OBJECT
public:
    explicit MFDAccOpen(unsigned char* buf, unsigned long cardno, QWidget *parent = 0);

    MFAutoLineEdit* _carType;

    void OnOK();
public slots:
    void OnBalanceChange()
    {
        _09->setText(QString::number(_trans->value() - _10->text().toDouble()));
    }
    void OnCardTypeChange(QString str);
};

class MFDAccClose : public MFDAccInfo
{
    Q_OBJECT
public:
    explicit MFDAccClose(unsigned char* buf, unsigned long cardno, int accid, QWidget *parent = 0);


    bool canClose();
    void OnOK();
};

class MFDAccCharge : public MFDAccInfo
{
    Q_OBJECT
public:
    explicit MFDAccCharge(unsigned char* buf, unsigned long cardno, int accid, QWidget *parent = 0);

    void OnOK();
};

class MFDAccChange : public MFDAccInfo
{
    Q_OBJECT
public:
    explicit MFDAccChange(unsigned char* buf, unsigned long cardno, QWidget *parent = 0) ;

    void OnOK();

public slots:
    void OnIdChange(QString strId);
};

class MFDAccFind : public MFDAccChange
{
    Q_OBJECT
public:
    explicit MFDAccFind(unsigned char* buf, unsigned long cardno, QWidget *parent = 0);
    void OnOK();
};

class MFDAccLost : public MFDAccChange
{
    Q_OBJECT
public:
    explicit MFDAccLost(QWidget *parent = 0);
    void OnOK();
};

class MFDAccSupply : public MFDAccChange
{
    Q_OBJECT
public:
    explicit MFDAccSupply(unsigned char* buf, unsigned long cardno, int accid, QWidget *parent = 0);
    void OnOK();


    void retrive_supply();
};

class MFDAccCancel : public MFDAccCharge
{
    Q_OBJECT
public:
    MFTableView* _table;
    explicit MFDAccCancel(unsigned char* buf, unsigned long cardno, int accid, QWidget* parent = 0);
    void OnOK();
public slots:
    void slotCancelAll();
    void slotSelChange(QItemSelection, QItemSelection);
};

#endif // MFDACCINFO_H
