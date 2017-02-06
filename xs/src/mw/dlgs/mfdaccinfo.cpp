#include "mfdaccinfo.h"

MFDAccInfo::MFDAccInfo(QWidget *parent ) :
    QDialog(parent)
{
    QGridLayout* grid;

    QHBoxLayout* hLayout;
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(_infoBox = new QGroupBox("账户信息"));
    layout->addWidget(_commandBox = new QGroupBox());

    _infoBox->setLayout(grid = new QGridLayout());
    _commandBox->setLayout(hLayout = new QHBoxLayout());
    _hLayout = hLayout;

    hLayout->addWidget(_transType = new QLabel("充值金额："));
    hLayout->addWidget(_trans = new QDoubleSpinBox());
    hLayout->addStretch(1);
    hLayout->addWidget(_ok = new QPushButton("确定"));
    hLayout->addWidget(_cancel = new QPushButton("取消"));

    connect(_ok, SIGNAL(clicked()), this, SLOT(handle()));
    connect(_cancel, SIGNAL(clicked()), this, SLOT(reject()));

    grid->addWidget(new QLabel("姓名："), 0, 0);
    grid->addWidget(_01 =  new QLineEdit(), 0, 1);
    grid->addWidget(new QLabel("电话："), 0, 2);
    grid->addWidget(_02 = new QLineEdit(), 0, 3);
    grid->addWidget(new QLabel("ID号："), 0, 4);
    grid->addWidget(_03 =  new QLineEdit(), 0, 5);

    grid->addWidget(new QLabel("病区："), 1, 0);
    grid->addWidget(_04 =   new MFAutoLineEdit(), 1, 1);   _04->setAutoList(MFConf()[DCONF_ZONE]); _04->setFit();
    grid->addWidget(new QLabel("科室："), 1, 2);
    grid->addWidget(_05 = new MFAutoLineEdit(), 1, 3);   _05->setAutoList(MFConf()[DCONF_DEPT]); _05->setFit();
    grid->addWidget(new QLabel("床位："), 1, 4);
    grid->addWidget(_06 = new QLineEdit(), 1, 5);

    grid->addWidget(new QLabel("卡号："),      2, 0);
    grid->addWidget(_07 =  new QLineEdit(),    2, 1);
    grid->addWidget(new QLabel("状态："),      2, 2);
    grid->addWidget(_08 =  new MFAutoLineEdit(),    2, 3);  _08->addItems(MFConf()[DCONF_CS]); _08->setFit();
    grid->addWidget(new QLabel("余额："),      2, 4);
    grid->addWidget(_09 =  new QLineEdit(),    2, 5);

    grid->addWidget(new QLabel("押金："), 3, 0);
    grid->addWidget(_10 =  new QLineEdit(MFConf()[DCONF_DEPOSIT].at(0)), 3, 1);
    grid->addWidget(new QLabel("支付方式："), 3, 2);
    grid->addWidget(_11 =  new MFAutoLineEdit(), 3, 3);   _11->addItems(MFConf()[DCONF_CT]); _11->setFit();
    grid->addWidget(new QLabel("支付金额："), 3, 4);
    grid->addWidget(_12 =  new QDoubleSpinBox(), 3, 5);

    grid->addWidget(new QLabel("拼音码："), 4, 0);
    grid->addWidget(_13 =  new QLineEdit(), 4, 1);
    grid->addWidget(new QLabel("密码："), 4, 2);
    grid->addWidget(_14 =  new QLineEdit(), 4, 3);
    grid->addWidget(new QLabel("账户类型："), 4, 4);
    grid->addWidget(_15 =  new MFAutoLineEdit(), 4, 5);   _15->addItems(MFConf()[DCONF_DACC_TYPE]); _15->setFit();

    grid->addWidget(new QLabel("月补助金额："), 5, 0);
    grid->addWidget(_16 =  new QDoubleSpinBox(), 5, 1);  _16->setRange(0, 20000);
    grid->addWidget(new QLabel("领补助日期："), 5, 2);
    grid->addWidget(_17 =  new QLineEdit(), 5, 3);

    grid->setColumnStretch(6, 1);

    _07->setDisabled(true);
    _09->setDisabled(true);
    _14->setDisabled(true);

    _trans->setRange(-1000, 1000);
}
int MFDAccInfo::change_balance(QString op, QString orderid, double trans_value, bool use_arg)
{
    /* 设置好充值的钱，直接调用充值的函数了 */
    //_trans->setValue(_16->value());
    /* write card */
    QString balance;
    QString nouse;

    if(!use_arg)
    {
        trans_value = _trans->value();
    }

    if(orderid.length() == 0)
    {
        orderid = MFGenOrder(_accid, op);
    }

    /* add flow */
    MFSqlTableModel* flow = MFGetModel("DFLOW", MFSqlTableModel::QUERY);
    flow->setFilterAndSelect(QString());
    if(!flow->insertRecordByValuesAndCommit(QList<QVariant>()
                                            << QVariant()
                                            << _accid
                                            << qulonglong(_cardno)
                                            << trans_value
                                        //    << DCONF_OP_ACC_SUPPLY
                                            << op
                                            << trans_value + _09->text().toDouble()
                                            << MFGetOper()
                                            << QVariant(MFNowDate()) << QVariant(MFNowTime())
                                            << QVariant(orderid)))
    {
        mylog << "error" << flow->lastError().text();
        return -1;
    }

    /* prepare for print */
    flow->setFilter(QString("F1=%1 and F2=%2").arg(_accid).arg(_cardno));
    flow->sort(0, Qt::DescendingOrder);
    flow->select();
    _flowId = flow->getData(0, 0).toInt();

    /* 如果是挂失，则不需要写卡 */
    if(op != DCONF_OP_ACC_LOST)
    {
        MFParseCard(_buf, balance, nouse, nouse, nouse, nouse, nouse, nouse, nouse);
        if(op == DCONF_OP_ACC_LOST_RECOVERY)
        {
            /* 如果是挂失回复，需要把数据库的余额写入到卡里 */
            if(MFWriteCard(_cardno, _buf,
                           _09->text(),
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL, NULL))
            {
                flow->removeRow(0);
                flow->submitAll();
                mylog << "error";
                return -1;
            }
        }
        else
        {
            if(MFWriteCard(_cardno, _buf,
                           QString::number(trans_value + QString(balance).toDouble()).toAscii().data(),
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL, NULL))
            {
                flow->removeRow(0);
                flow->submitAll();
                mylog << "error";
                return -1;
            }
        }
    }

    /* update dacc */
    /* update DACC table */
    MFSqlTableModel* model = MFGetModel("DACC", MFSqlTableModel::QUERY);
    model->setFilterAndSelect(QString("F0=%1").arg(_accid));
    if(model->rowCount()!=1)
    {
        /* 如果是挂失则不写卡 */
        if(op != DCONF_OP_ACC_LOST)
        {
            MFWriteCard(_cardno, _buf,
                    balance,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL, NULL);
        }
        flow->removeRow(0);
        flow->submitAll();
        mylog << "error";
        return -1;
    }

    QMap<int, QVariant> updateValues;
    updateValues.insert(DACC_BALANCE, trans_value + _09->text().toDouble());
    if(op == DCONF_OP_ACC_SUPPLY)
        updateValues.insert(DACC_SUPPLY_DATE, MFNowMonth());

    /* 如果是挂失，则需要修改状态字段为挂失 */
    if(op == DCONF_OP_ACC_LOST)
        updateValues.insert(DACC_STATUS, DCONF_CS_LOST);

    /* 如果是挂失恢复，则需要将状态改回来 */
    if(op == DCONF_OP_ACC_LOST_RECOVERY)
        updateValues.insert(DACC_STATUS, DCONF_CS_GENERAL);

    if(!model->updateRecordByValuesAndCommit(updateValues))
    {
        flow->removeRow(0);
        flow->submitAll();
        MFWriteCard(_cardno, _buf,
                    balance,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL, NULL);
        mylog << "error";
        return -1;
    }

    return _flowId;
}
void MFDAccInfo::setControl(int accid)
{
    MFSqlTableModel* model = MFGetModel("DACC", MFSqlTableModel::QUERY);

    model->setFilter(QString("F0=%1").arg(accid));
    model->select();
    if(model->rowCount() != 1)
        return;


    int row = 0;
    _01->setText(model->getData(row, 1).toString());
    _02->setText(model->getData(row, 2).toString());
    _03->setText(model->getData(row, 3).toString());
//    _04->setCurrentIndex(_04->findText(model->getData(row, 4).toString()));
//    _05->setCurrentIndex(_05->findText(model->getData(row, 5).toString()));
    _04->setText(model->getData(row, 4).toString());
    _05->setText(model->getData(row, 5).toString());
    _06->setText(model->getData(row, 6).toString());
    _07->setText(model->getData(row, 7).toString());
    _cardno = _07->text().toLongLong();
    _08->setCurrentIndex(_08->findText(model->getData(row, 8).toString()));
    _09->setText(model->getData(row, 9).toString());
    _10->setText(model->getData(row, 10).toString());
    _11->setCurrentIndex(_11->findText(model->getData(row, 11).toString()));
    _12->setValue(model->getData(row, 12).toDouble());
    _13->setText(model->getData(row, 13).toString());
    _14->setText(model->getData(row, 14).toString());
    _15->setCurrentIndex(_15->findText(model->getData(row, 15).toString()));
    _16->setValue(model->getData(row, 16).toDouble());
    _17->setText(model->getData(row, 17).toString());
}

MFDAccOpen::MFDAccOpen(unsigned char* buf, unsigned long cardno, QWidget *parent) :
    MFDAccInfo(parent)
{
    _buf = buf;
    _cardno = cardno;

    _09->setDisabled(true);
    _trans->setValue(100);
    _transType->setText("开卡金额：");

    _01->setFocus();
    _07->setText(QString::number(cardno));

    unsigned char buf1[16];
    MDString(_07->text().toUtf8().data(), buf1);
    char buf2[38];
    xs_hex2str(buf1, 16, buf2);
    _14->setText(buf2);

    _hLayout->insertWidget(2, new QLabel("卡类型："));
    _hLayout->insertWidget(3, _carType = new MFAutoLineEdit()); _carType->setFit();
    _carType->addItem("普通卡");
    _carType->addItem("军官卡");
    _carType->addItem("士兵卡");
    connect(_carType, SIGNAL(currentIndexChanged(QString)), this, SLOT(OnCardTypeChange(QString)));

    connect(_trans, SIGNAL(valueChanged(QString)), this, SLOT(OnBalanceChange()));
    connect(_10, SIGNAL(textChanged(QString)), this, SLOT(OnBalanceChange()));
    _17->setDisabled(true);

    _16->setValue(MFConf()[DCONF_SUPPLY_AMOUNT].at(0).toDouble());

    OnBalanceChange();
}
void MFDAccOpen::OnCardTypeChange(QString str)
{
    if(str == "普通卡")
    {
        _11->setCurrentIndex(0);
        _12->setValue(0);
        _10->setText(MFConf()[DCONF_DEPOSIT].at(0));
    }
    else if(str == "士兵卡")
    {
        _11->setCurrentIndex(1);
        _12->setValue(18);
        _10->setText("50");
    }
    else if(str == "军官卡")
    {
        _11->setCurrentIndex(1);
        _12->setValue(12);
        _10->setText("50");
    }
}
void MFDAccOpen::OnOK()
{
    if(_01->text().length() == 0)
    {
        QMessageBox::warning(this, "错误", "账户名不能为空");
        return;
    }
    if(_02->text().length() == 0)
    {
        QMessageBox::warning(this, "错误", "账户电话号码不能为空");
        return;
    }
    if(_03->text().length() == 0)
    {
        QMessageBox::warning(this, "错误", "账户ID号不能为空");
        return;
    }
    if(_04->currentText().length() == 0)
    {
        QMessageBox::warning(this, "错误", "账户病区不能为空");
        return;
    }
    if(_05->currentText().length() == 0)
    {
        QMessageBox::warning(this, "错误", "账户科室不能为空");
        return;
    }
    if(_06->text().length() == 0)
    {

        QMessageBox::warning(this, "错误", "账户床位不能为空");
        return;
    }
    if(_trans->value() < _10->text().toDouble())
    {
        QMessageBox::warning(this, "错误", "不花钱不给开卡");
        _trans->selectAll();
        _trans->setFocus();
        return;
    }
    QString daily;

    if(_11->currentText() == DCONF_CT_NOW)
    {

        daily = "0";
    }
    else if(_11->currentText() == DCONF_CT_DAILY)
    {

        if(_12->value() == 0)
        {

            QMessageBox::warning(this, "错误", "日结算卡，结算金额不能为0");
            return;
        }

        daily = _12->text();
        mylog << daily;
    }

    MFLog("开始。。。");

    MFSqlTableModel* acc = MFGetModel("DACC", MFSqlTableModel::QUERY);
    /*
    acc->setFilterAndSelect(QString("F%1='%2' and (F%3='%4' or F%5='%6'").
                            arg(DACC_ID).arg(_03->text()).
                            arg(DACC_STATUS).arg(DCONF_CS_GENERAL).
                            arg(DACC_STATUS).arg(DCONF_CS_LOST));
                            */
    if(!acc->setFilterAndSelect(QString("%1 and (%2 or %3)").arg(FIELDEQUAL(DACC_ID, _03->text()),
                                                                 FIELDEQUAL(DACC_STATUS, DCONF_CS_GENERAL),
                                                                 FIELDEQUAL(DACC_STATUS, DCONF_CS_LOST))))
    {
        mylog << "select error";
        return;
    }



    if(acc->rowCount() > 0)
    {
        QMessageBox::warning(this, "错误", "该身份证已经有卡存在，如果需要再开卡，请先退卡");
        return;
    }

    if(!acc->insertRecordByValues(QList<QVariant>()
                                  << QVariant()  /* id */
                                  << _01->text()
                                  << _02->text()
                                  << _03->text()
                                  << _04->currentText()
                                  << _05->currentText()
                                  << _06->text()
                                  << _07->text()
                                  << DCONF_CS_GENERAL
                                  << _09->text().toDouble() + _16->value()   /* 开卡余额 */
                                  << _10->text()
                                  << _11->currentText()
                                  << _12->text().toDouble()
                                  << _13->text()
                                  << _14->text()
                                  << _15->currentText()
                                  << _16->value()
                                  << MFNowMonth()))
    {
        mylog << "insert error";
        return;
    }

    if(!acc->submitAll())
    {

        mylog << "submit error " << acc->lastError().text();
        return;
    }


    QString status;
    int accid = MFGetAccID(_cardno, status, acc);
    if(accid <= 0)
    {

        QMessageBox::warning(this, "错误", "写数据库失败");
        mylog << "get acc id error";
        return;
    }

    MFLog(QString("accid=%1").arg(accid));



    /* write card */
    if(MFWriteCard(_cardno, _buf,
                   QString::number(_09->text().toDouble() +_16->text().toDouble()),
                   _01->text().toAscii().data(),
                   _04->currentText().toUtf8().data(),
                   _05->currentText().toUtf8().data(),
                   _06->text().toUtf8().data(),
                   QString::number(accid).toAscii().data(),
                   (char*)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
                   daily.toAscii().data()))
    {

        QMessageBox::warning(this, "错误", "写卡失败");
        mylog << "write_card_data id error";
        acc->removeRow(0);
        acc->submitAll();
        return;
    }



    MFSqlTableModel* flow = MFGetModel("DFLOW", MFSqlTableModel::QUERY);
    if(!flow->insertRecordByValuesAndCommit(QList<QVariant>()
                                            << QVariant()
                                            << accid
                                            << qulonglong(_cardno)
                                            << _trans->value()
                                            << DCONF_OP_ACC_OPEN
                                            << _09->text()
                                            << MFGetOper()
                                            << QVariant(MFNowDate()) << QVariant(MFNowTime())
                                            << QVariant(MFGenOrder(accid, DCONF_OP_ACC_OPEN))))
    {

        mylog << "error" << flow->lastError().text();
        acc->removeRow(0);
        acc->submitAll();
        QMessageBox::warning(this, "错误", "写流水表失败");
        return;
    }

    if(_16->value() > 0.001)
    {
        if(!flow->insertRecordByValuesAndCommit(QList<QVariant>()
                                                << QVariant()
                                                << accid
                                                << qulonglong(_cardno)
                                                << _16->value()
                                                << DCONF_OP_ACC_SUPPLY
                                                << _09->text().toDouble() + _16->value()
                                                << MFGetOper()
                                                << QVariant(MFNowDate()) << QVariant(MFNowTime())
                                                << QVariant(MFGenOrder(accid, DCONF_OP_ACC_SUPPLY))))
        {

            mylog << "error" << flow->lastError().text();
            acc->removeRow(0);
            acc->submitAll();
            QMessageBox::warning(this, "错误", "写补助流水表失败");
            flow->setFilterAndSelect(QString("F1=%1 and F2=%2").arg(accid).arg(_cardno));
            flow->removeRow(0);
            flow->submitAll();
            return;
        }
    }

    /* get flow id for print */
    flow->setFilterAndSelect(QString("F1=%1 and F2=%2").arg(accid).arg(_cardno));
    /* print */
    printTicket(flow->getData(0, 0).toInt());

    if(flow->rowCount() > 1)
        printTicket(flow->getData(0, 1).toInt());

    /* close this dialog */
    accept();
}

MFDAccClose::MFDAccClose(unsigned char* buf, unsigned long cardno, int accid, QWidget *parent) :
    MFDAccInfo(parent)
{
    _cardno = cardno;
    _buf = buf;
    _accid = accid;
    setControl(accid);

    /* total cost this month */
    double total_cost = calc_total_cost_in_month(_accid, MFNowMonth());

    /* 如果当月消费还没到补助金额，则需要把补助金额还回来 */
    if(total_cost < _16->value())
        _trans->setValue(_09->text().toDouble() + _10->text().toDouble() - (_16->value() - total_cost));
    else
        _trans->setValue(_09->text().toDouble() + _10->text().toDouble());
    _trans->setDisabled(true);
    _infoBox->setDisabled(true);
    _transType->setText("应退金额：");
}

bool MFDAccClose::canClose()
{
    QString balance;
    QString orderid;
    QString accid;
    QString nouse;
    MFParseCard(_buf, balance, nouse, nouse, nouse, nouse, accid, orderid, nouse);

    mylog << balance << accid << orderid;
    MFSqlTableModel* model;

    if(orderid.at(0) != 'F' && orderid.at(0) != 'f')
    {
        model = MFGetModel("DORDER", MFSqlTableModel::QUERY);
        model->setFilterAndSelect(QString("F1='%1'").arg(orderid));
        if(model->rowCount() == 0)
        {
            mylog << "order not sync";
            QMessageBox::critical(this, "错误", "订单未同步到电脑，请等待手持机同步数据后再退卡");

            return false;
        }
    }


    model = MFGetModel("DACC", MFSqlTableModel::QUERY);
    model->setFilterAndSelect(QString("F0=%1").arg(accid));
    if(model->rowCount() != 1)
    {
        mylog << "acc not exist";
        QMessageBox::critical(this, "错误", "账户信息不存在");

        return false;
    }

    if(model->getData(0, 9).toDouble() != QString(balance).toDouble())
    {
        mylog << "balance not equal" << model->getData(0, 9).toDouble() << QString(balance).toDouble();
        QMessageBox::critical(this, "错误", "余额不一致");

        return false;
    }

    /* 领了补助再来退卡，否则金额不对的 */
    QString toMonth = MFNowMonth();
    if(toMonth != model->getData(0, DACC_SUPPLY_DATE).toString() && model->getData(0, DACC_SUPPLY_AMOUNT).toDouble() > 0.001)
    {
        QMessageBox::critical(this, "错误", "本月补助未领，请先领补助再来退卡");

        return false;
    }

    return true;
}

void MFDAccClose::OnOK()
{
    if(!canClose())
    {
        reject();
        return;
    }

    double total_cost = calc_total_cost_in_month(_accid, MFNowMonth());

    MFSqlTableModel* flow = MFGetModel("DFLOW", MFSqlTableModel::QUERY);

    if(total_cost < _16->value())
    {
        if(!flow->insertRecordByValuesAndCommit(QList<QVariant>()
                                                << QVariant()
                                                << _accid
                                                << qulonglong(_cardno)
                                                << _16->value() - total_cost
                                                << DCONF_OP_ACC_SUPPLY_CALC
                                                << _trans->value()
                                                << MFGetOper()
                                                << QVariant(MFNowDate()) << QVariant(MFNowTime())
                                                << QVariant(MFGenOrder(_accid, DCONF_OP_ACC_SUPPLY_CALC))))
        {
            mylog << "error" << flow->lastError().text();
            return;
        }
    }
    /* add flow */
    if(!flow->insertRecordByValuesAndCommit(QList<QVariant>()
                                            << QVariant()
                                            << _accid
                                            << qulonglong(_cardno)
                                            << _trans->value()
                                            << DCONF_OP_ACC_CLOSE
                                            << 0
                                            << MFGetOper()
                                            << QVariant(MFNowDate()) << QVariant(MFNowTime())
                                            << QVariant(MFGenOrder(_accid, DCONF_OP_ACC_CLOSE))))
    {
        mylog << "error" << flow->lastError().text();
        return;
    }

    /* prepare for print */
    flow->setFilter(QString("F1=%1 and F2=%2").arg(_accid).arg(_cardno));
    flow->sort(0, Qt::DescendingOrder);
    flow->select();
    _flowId = flow->getData(0, 0).toInt();

    /* write card */
    if(MFWriteCard(_cardno, _buf,
                   (char*) "0",
                   (char*)"0",
                   _04->currentText().toUtf8().data(),
                   _05->currentText().toUtf8().data(),
                   _06->text().toUtf8().data(),
                   (char*)"0",
                   (char*)"0", (char*)"0"))
    {
        flow->removeRow(0);
        flow->submitAll();
        mylog << "error";
        return;
    }


    /* update DACC table */
    MFSqlTableModel* model = MFGetModel("DACC", MFSqlTableModel::QUERY);
    model->setFilterAndSelect(QString("F0=%1").arg(_accid));
    if(model->rowCount()!=1)
    {
        flow->removeRow(0);
        flow->submitAll();
        mylog << "error";
        return;
    }

    QMap<int, QVariant> updateValues;
    updateValues.insert(8, DCONF_CS_END);
    updateValues.insert(9, 0);
    if(!model->updateRecordByValuesAndCommit(updateValues))
    {
        flow->removeRow(0);
        flow->submitAll();
        mylog << "error";
        return;
    }

    accept();

    printTicket(_flowId);
}


MFDAccCharge::MFDAccCharge(unsigned char* buf, unsigned long cardno, int accid, QWidget *parent) :
    MFDAccInfo(parent)
{
    _cardno = cardno;
    _buf = buf;
    _accid = accid;
    setControl(accid);

    _trans->setValue(0);
    _trans->setFocus();
    _infoBox->setDisabled(true);

}
void MFDAccCharge::OnOK()
{
    change_balance(DCONF_OP_ACC_CHARGE, QString(), _trans->value(), true);
    accept();
    printTicket(_flowId);
}
#if 0
void MFDAccCharge::OnOK()
{
    /* add flow */
    MFSqlTableModel* flow = MFGetModel("DFLOW", MFSqlTableModel::QUERY);
    if(!flow->insertRecordByValuesAndCommit(QList<QVariant>()
                                            << QVariant()
                                            << _accid
                                            << qulonglong(_cardno)
                                            << _trans->value()
                                            << DCONF_OP_ACC_CHARGE
                                            << _trans->value() + _09->text().toDouble()
                                            << MFGetOper()
                                            << QVariant(MFNowDate()) << QVariant(MFNowTime())
                                            << QVariant(MFGenOrder(_accid))))
    {
        mylog << "error" << flow->lastError().text();
        return;
    }

    /* prepare for print */
    flow->setFilter(QString("F1=%1 and F2=%2").arg(_accid).arg(_cardno));
    flow->sort(0, Qt::DescendingOrder);
    flow->select();
    _flowId = flow->getData(0, 0).toInt();

    /* write card */
    QString balance;
    QString nouse;
    MFParseCard(_buf, balance, nouse, nouse, nouse, nouse, nouse, nouse, nouse);
    if(MFWriteCard(_cardno, _buf,
                   QString::number(_trans->value() + QString(balance).toDouble()).toAscii().data(),
                   NULL,
                   NULL,
                   NULL,
                   NULL,
                   NULL,
                   NULL, NULL))
    {
        flow->removeRow(0);
        flow->submitAll();
        mylog << "error";
        return;
    }

    /* update dacc */
    /* update DACC table */
    MFSqlTableModel* model = MFGetModel("DACC", MFSqlTableModel::QUERY);
    model->setFilterAndSelect(QString("F0=%1").arg(_accid));
    if(model->rowCount()!=1)
    {
        MFWriteCard(_cardno, _buf,
                    balance,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL, NULL);
        flow->removeRow(0);
        flow->submitAll();
        mylog << "error";
        return;
    }

    QMap<int, QVariant> updateValues;
    updateValues.insert(9, _trans->value() + _09->text().toDouble());
    if(!model->updateRecordByValuesAndCommit(updateValues))
    {
        flow->removeRow(0);
        flow->submitAll();
        MFWriteCard(_cardno, _buf,
                    balance,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL, NULL);
        mylog << "error";
        return;
    }

    accept();
    printTicket(_flowId);
}
#endif

MFDAccChange::MFDAccChange(unsigned char* buf, unsigned long cardno, QWidget *parent) :
    MFDAccInfo(parent)
{
    _transType->setText("补卡费用：");
    _cardno = cardno;
    _buf = buf;
    _accid = 0;

    _01->setDisabled(true);
    _02->setDisabled(true);
    //   _03->setDisabled(true);
    _04->setDisabled(true);
    _05->setDisabled(true);
    _06->setDisabled(true);
    _07->setDisabled(true);
    _08->setDisabled(true);
    _09->setDisabled(true);
    _10->setDisabled(true);
    _11->setDisabled(true);
    _12->setDisabled(true);
    _13->setDisabled(true);
    _14->setDisabled(true);
    _15->setDisabled(true);
    _16->setDisabled(true);
    _17->setDisabled(true);
    _03->setFocus();

    _trans->setValue(MFConf()[DCONF_CHANGE_CARD].at(0).toDouble());

    connect(_03, SIGNAL(textChanged(QString)), this, SLOT(OnIdChange(QString)));

}
void MFDAccChange::OnOK()
{
    if(_accid <= 0)
        return;

    if(_08->currentText() != DCONF_CS_LOST)
    {
        QMessageBox::information(this, "提示", "请先挂失后再补卡");
        return;
    }

    MFSqlTableModel* flow = MFGetModel("DFLOW", MFSqlTableModel::QUERY);
    /* add flow */
    if(!flow->insertRecordByValuesAndCommit(QList<QVariant>()
                                            << QVariant()
                                            << _accid
                                            << qulonglong(_cardno)
                                            << _trans->value()
                                            << DCONF_OP_ACC_CHANGE_LOST
                                            << _09->text().toDouble() - _trans->value()
                                            << MFGetOper()
                                            << QVariant(MFNowDate()) << QVariant(MFNowTime())
                                            << QVariant(MFGenOrder(_accid, DCONF_OP_ACC_CHANGE_LOST))))
    {

        QMessageBox::information(this, "提示", "账户流水表操作失败");
        return;
    }

    /* prepare for print */
    flow->setFilter(QString("F1=%1 and F2=%2").arg(_accid).arg(_cardno));
    flow->sort(0, Qt::DescendingOrder);
    flow->select();
    _flowId = flow->getData(0, 0).toInt();


    if(MFWriteCard(_cardno, _buf,
                   QString::number(_09->text().toDouble() - _trans->value()).toAscii().data(),  // 余额不对
                   _01->text().toAscii().data(),
                   _04->currentText().toUtf8().data(),
                   _05->currentText().toUtf8().data(),
                   _06->text().toUtf8().data(),
                   QString::number(_accid).toAscii().data(),
                   (char*)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", _12->text()))
    {
        flow->removeRow(0);
        flow->submitAll();
        QMessageBox::information(this, "提示", "写卡失败");
        return;
    }

    MFSqlTableModel* model = MFGetModel("DACC", MFSqlTableModel::QUERY);
    QMap<int, QVariant> updateValues;
    updateValues.insert(DACC_BALANCE,  _09->text().toDouble() - _trans->value());
    updateValues.insert(DACC_CARD,  qulonglong(_cardno));
    updateValues.insert(DACC_STATUS, DCONF_CS_GENERAL);
    if(!model->updateRecordByValuesAndCommit(updateValues))
    {
        flow->removeRow(0);
        flow->submitAll();
        QMessageBox::information(this, "提示", "账户表更新失败");
        return;
    }

    accept();
    printTicket(_flowId);

}

void MFDAccChange::OnIdChange(QString strId)
{
    strId = _03->text();
    MFSqlTableModel* acc = MFGetModel("DACC", MFSqlTableModel::QUERY);
    acc->setFilterAndSelect(QString("F3 like '%%1%' and (F8='%2' or F8='%3')").arg(strId, DCONF_CS_GENERAL, DCONF_CS_LOST));
    if(acc->rowCount() == 1)
    {
        _accid = acc->getData(0, 0).toInt();
        setControl(_accid);
    }
    else
    {
        _01->setText("");
        _02->setText("");
        _accid = 0;
    }
}

MFDAccFind::MFDAccFind(unsigned char* buf, unsigned long cardno, QWidget *parent) :
    MFDAccChange(buf, cardno, parent)
{
    _transType->hide();
    _trans->hide();
    QString status;
    _accid = MFGetAccID(cardno, status);
    setControl(_accid);
    this->setWindowTitle("挂失恢复");
    _03->setDisabled(true);
    this->_ok->setText("确认恢复");
    this->_cancel->setText("关闭窗口");
}
void MFDAccFind::OnOK()
{
    if(_accid <= 0)
    {
        return;
    }
#if 0
    MFSqlTableModel* acc = MFGetModel("DACC", MFSqlTableModel::QUERY);
    // acc->setFilterAndSelect(QString("F0=%1").arg(_accid));
    acc->setFilterAndSelect(FIELDEQUAL(DACC_SEQ, _accid));
    if(acc->rowCount() == 1)
    {
        QMap<int, QVariant> vars;
        vars.insert(DACC_STATUS, DCONF_CS_GENERAL);
        if(!acc->updateRecordByValuesAndCommit(vars))
        {
            QMessageBox::warning(this, "错误", "失败");
            return;
        }

        this->setControl(_accid);
    }
    else
    {
        QMessageBox::warning(this, "错误", QString("找到账户数量为 %1").arg(acc->rowCount()));
    }
#else

    MFSqlTableModel* model = MFGetModel("DFLOW", MFSqlTableModel::QUERY);
    model->setFilterAndSelect(QString("%1 and %2 and %3")
                              .arg( FIELDEQUAL(DFLOW_ACC_SEQ, _accid) , FIELDEQUAL(DFLOW_METHOD, DCONF_OP_ACC_LOST) , FIELDEQUAL(DFLOW_DATE, MFNowDate())));
    if(model->rowCount() > 0)
    {
        QMessageBox::warning(this, "错误", "这张卡今天才挂失的，至少明天才能解挂失");
        return;
    }
    change_balance(DCONF_OP_ACC_LOST_RECOVERY, QString(), 0, true);
#endif

}

MFDAccLost::MFDAccLost(QWidget *parent) :
    MFDAccChange(NULL, 0, parent)
{
    _transType->hide();
    _trans->hide();
    _16->setDisabled(true);
    _17->setDisabled(true);

}
void MFDAccLost::OnOK()
{

    mylog << _accid;

    if(_accid > 0)
    {
#if 0
        MFSqlTableModel* acc = MFGetModel("DACC", MFSqlTableModel::QUERY);
        acc->setFilterAndSelect(QString("F0=%1").arg(_accid));
        if(acc->rowCount() == 1)
        {
            QMap<int, QVariant> vars;
            vars.insert(DACC_STATUS, DCONF_CS_LOST);
            if(!acc->updateRecordByValuesAndCommit(vars))
            {
                QMessageBox::warning(this, "错误", "失败");
                return;
            }

            this->setControl(_accid);


        }
        else
        {
            QMessageBox::warning(this, "错误", QString("找到账户数量为 %1").arg(acc->rowCount()));
        }
#else
        MFSqlTableModel* acc = MFGetModel("DACC", MFSqlTableModel::QUERY);
        acc->setFilterAndSelect(FIELDEQUAL(DACC_SEQ, _accid));
        if(acc->getData(0, DACC_STATUS).toString() != DCONF_CS_LOST)
        {
            change_balance(DCONF_OP_ACC_LOST, QString(), 0, true);
            this->setControl(_accid);
        }
#endif
    }
    else
    {
        QMessageBox::warning(this, "错误", "未找到账户");
    }
}

MFDAccSupply::MFDAccSupply(unsigned char *buf, unsigned long cardno, int accid, QWidget *parent):
    MFDAccChange(buf, cardno, parent)
{
    _accid = accid;
    setControl(_accid);

    _transType->hide();
    _trans->hide();

    this->_ok->setText("领取补贴");
    this->_cancel->setText("关闭窗口");
    _03->setDisabled(true);
    _16->setDisabled(true);
    _17->setDisabled(true);
}

double MFDAccInfo::calc_total_cost_in_month(int accid, QString month)
{
    double money = 0;
    /* 从flow表里获取该账户在补助月的所有操作，将消费这块给计算出来 */
    MFSqlTableModel* model = MFGetModel("DFLOW", MFSqlTableModel::QUERY);
    model->setFilterAndSelect(QString("%1 and %2").arg(FIELDEQUAL(DFLOW_ACC_SEQ, accid), FIELDLIKE(DFLOW_DATE, month)));

    for(int i=0; i<model->rowCount(); i++)
    {
        QString method = model->getData(i, DFLOW_METHOD).toString();
        double amount = model->getData(i, DFLOW_AMOUNT).toDouble();
        if(method == DCONF_OP_ACC_BOOK
                || method ==  DCONF_OP_ACC_BUY
                || method == DCONF_OP_ACC_DAILY)
        {
            money += amount;
        }
        else if(method == DCONF_OP_ACC_CANCELBOOK)
        {
            money -= amount;
        }
    }
    return money;
}

void MFDAccSupply::OnOK()
{
    QString supply_retrive = MFConf()[DCONF_SUPPLY_RECICLE].at(0);
    // 上一次补助时间
    QString month = _17->text();
    double money = 0;

    if(supply_retrive == "是")
    {
        money = calc_total_cost_in_month(_accid, month);
        if(money > _16->value()) money = _16->value();
        charge_supply(money);
    }
    else
    {
        charge_supply(_16->value());
    }
}

void MFDAccInfo::charge_supply(double mony)
{
    /* 设置好充值的钱，直接调用充值的函数了 */
    change_balance(DCONF_OP_ACC_SUPPLY, QString(), mony, true);

    accept();
    printTicket(_flowId);
}

MFDAccCancel::MFDAccCancel(unsigned char *buf, unsigned long cardno, int accid, QWidget *parent):
    MFDAccCharge(buf, cardno, accid, parent)
{
    _table = new MFTableView("DORDER");
#if 0
#define DFLOWF10 "流水号"
#define DFLOWF11 "订单号"
#define DFLOWF12 "菜品号"
#define DFLOWF13 "数量"
#define DFLOWF14 "单价"
#define DFLOWF15 "订餐员"
#define DFLOWF16 "订单状态"
#define DFLOWF17 "错误状态"
#define DFLOWF18 "就餐日期"
#define DFLOWF19 "餐别"
#endif
    _table->_model->setHeaderData(0, Qt::Horizontal, DFLOWF10);
    _table->_model->setHeaderData(1, Qt::Horizontal, DFLOWF11);
    _table->_model->setHeaderData(2, Qt::Horizontal, DFLOWF12);
    _table->_model->setHeaderData(3, Qt::Horizontal, DFLOWF13);
    _table->_model->setHeaderData(4, Qt::Horizontal, DFLOWF14);
    _table->_model->setHeaderData(5, Qt::Horizontal, DFLOWF15);
    _table->_model->setHeaderData(6, Qt::Horizontal, DFLOWF16);
    _table->_model->setHeaderData(7, Qt::Horizontal, DFLOWF17);
    _table->_model->setHeaderData(8, Qt::Horizontal, DFLOWF18);
    _table->_model->setHeaderData(9, Qt::Horizontal, DFLOWF19);
    _table->_model->setHeaderData(10, Qt::Horizontal, DFLOWF20);

    _table->_model->setFilterAndSelect(QString("%1 and %2 and %3 and %4").arg(FIELDGREATEEQUAL(DORDER_DAY, MFNowDate()),
                                                                FIELDLIKE(DORDER_ORDERID, "o"),
                                                                FIELDLIKE(DORDER_ORDERID, QString(".%1").arg(accid)),
                                                                FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK)));

    _table->hideColumn(0);
    _table->hideColumn(6);
    _table->hideColumn(7);



    _table->setItemDelegate(readOnlyDelegate());

    QVBoxLayout* lay = (QVBoxLayout*)layout();
    lay->insertWidget(1, _table);

    //_table->resizeRowsToContents();
    _table->resizeColumnsToContents();

    _ok->setText("取消所选菜品");
    _cancel->setText("关闭窗口");

    QPushButton* cancelAll;
    _hLayout->insertWidget(_hLayout->count()-2, cancelAll = new QPushButton("取消所有菜品"));
    connect(cancelAll, SIGNAL(clicked()), this, SLOT(slotCancelAll()));

    _trans->hide();
    _transType->hide();

    connect(_table, SIGNAL(sigSelectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(slotSelChange(QItemSelection,QItemSelection)));
    _ok->setDisabled(true);
}

void MFDAccCancel::slotCancelAll()
{
    MFSqlTableModel* accModel = MFGetModel("DACC", MFSqlTableModel::QUERY);
    accModel->setFilterAndSelect(FIELDEQUAL(DACC_SEQ, _accid));
    double dailyCost = accModel->getData(0, DACC_AMOUNT).toDouble();
    QString daily = accModel->getData(0, DACC_SETTLE ).toString();

    QMap<QString, double> orders;
    for(int i=0; i<_table->_model->rowCount(); i++)
    {
        QString orderid = _table->_model->getData(i, DORDER_ORDERID).toString();
        int count = _table->_model->getData(i, DORDER_COUNT).toInt();
        double price = _table->_model->getData(i, DORDER_PRICE).toDouble();

        if(orders.find(orderid) == orders.end())
        {
            orders.insert(orderid, count* price);
        }
        else
        {
            orders.insert(orderid, count* price + orders[orderid]);
        }

        _table->_model->setData(_table->_model->index(i, DORDER_STATUS), MD_ORDER_STATUS_CANCEL);
    }

    foreach(QString orderid, orders.keys())
    {
        if(daily == DCONF_CT_NOW)
            change_balance(DCONF_OP_ACC_CANCELBOOK, orderid + ".all", orders.value(orderid), true);
        else
            change_balance(DCONF_OP_ACC_CANCELBOOK, orderid + ".all", dailyCost, true);

    }

    _table->_model->submitAll();
}

void MFDAccCancel::slotSelChange(QItemSelection sel, QItemSelection)
{
    MFSqlTableModel* accModel = MFGetModel("DACC", MFSqlTableModel::QUERY);
    accModel->setFilterAndSelect(FIELDEQUAL(DACC_SEQ, _accid));
 //   double dailyCost = accModel->getData(0, DACC_AMOUNT).toDouble();
    QString daily = accModel->getData(0, DACC_SETTLE ).toString();

    if(daily == DCONF_CT_DAILY)
    {
        _ok->setDisabled(true);
        return;
    }

    if(sel.count() == 0)
        _ok->setDisabled(true);
    else
        _ok->setDisabled(false);
}
#if 0
void MFDAccCancel::OnOK()
{
    QModelIndexList indexs = _table->selectionModel()->selectedRows();

    mylog << indexs.count();

    foreach(QModelIndex index, indexs)
    {
        QString orderid = _table->_model->getData(index.row(), DORDER_ORDERID).toString();
        QString dinner = _table->_model->getData(index.row(), DORDER_DINNER).toString();
        QString cb = _table->_model->getData(index.row(), DORDER_TIME).toString();
        int count = _table->_model->getData(index.row(), DORDER_COUNT).toInt();
        double price = _table->_model->getData(index.row(), DORDER_PRICE).toDouble();

        mylog << count * price;

        change_balance(DCONF_OP_ACC_CANCELBOOK, orderid + "." + cb + "."+dinner, count*price, true);
        this->setControl(_accid);
        _table->_model->setData(_table->_model->index(index.row(), DORDER_STATUS), MD_ORDER_STATUS_CANCEL);

    }
    _table->_model->submitAll();

}
#else
void MFDAccCancel::OnOK()
{
    QModelIndexList indexs = _table->selectionModel()->selectedIndexes();
    QMap<int, int> rows;
    foreach(QModelIndex index, indexs)
    {
        rows.insert(index.row(), index.row());
    }

    foreach(int row, rows.keys())
    {
        QString orderid = _table->_model->getData(row, DORDER_ORDERID).toString();
        QString dinner = _table->_model->getData(row, DORDER_DINNER).toString();
        QString cb = _table->_model->getData(row, DORDER_CB).toString();
        int count = _table->_model->getData(row, DORDER_COUNT).toInt();
        double price = _table->_model->getData(row, DORDER_PRICE).toDouble();

        mylog << count * price;

        change_balance(DCONF_OP_ACC_CANCELBOOK, orderid + "." + cb + "."+dinner, count*price, true);
        this->setControl(_accid);
        _table->_model->setData(_table->_model->index(row, DORDER_STATUS), MD_ORDER_STATUS_CANCEL);
    }
    _table->_model->submitAll();
}
#endif
