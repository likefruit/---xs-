#include "mfviews.h"
#include "mfdaccinfo.h"

MFDeviceDP::MFDeviceDP(QWidget *parent) :
    MFDeviceHandle(_setup = new QGroupBox("设置下载参数"), parent)
{
    setTitle(MENU_DEVICE_DP );

    setupGroupBox();
}

void MFGDW::newFilterAdded(MFFilter::filter *f)
{
    if(f->_1) f->_1->setDisabled(true);
    f->_2->removeItem(0);
    f->_3->clear();f->_3->addItem("=");
}

void MFGDW::gdlSelectChange(QItemSelection selected, QItemSelection)
{
    MFFilter::filter * fw = NULL;
    MFFilter::filter * fd = NULL;
    MFFilter::filter * ft = NULL;
    MFSqlTableModel* gdw = MFGetModel("GDW", MFSqlTableModel::QUERY);

    if(selected.count() == 0)
    {
        return;
    }

    if(_workFilter->_filter->_filters.count() == 0)
    {
        if(QMessageBox::question(this, "增加周食谱", "是否将该菜谱加入到所有周别、所有日期、所有餐别中？", QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
        mylog << "yes press";
    }
    else
    {
        MFFilter::filter *f;
        for(int i=0; i<_workFilter->_filter->_filters.count(); i++)
        {
            f = _workFilter->_filter->_filters.at(i);
            if(f->_2->currentText() == GDWF1)
            {
                if(fw)
                {
                    QMessageBox::warning(this, "错误", GDWF1"条件重复");
                    return;
                }
                fw = f;
            }
            if(f->_2->currentText() == GDWF2)
            {
                if(fd)
                {
                    QMessageBox::warning(this, "错误", GDWF2"条件重复");
                    return;
                }
                fd = f;
            }
            if(f->_2->currentText() == GDWF3)
            {
                if(ft)
                {
                    QMessageBox::warning(this, "错误", GDWF3"条件重复");
                    return;
                }
                ft = f;
            }
        }
    }

    QModelIndex index = selected.at(0).topLeft();
    const QAbstractItemModel *gdlModel = index.model();
    QModelIndex idIndex = gdlModel->index(index.row(), GDL_NAME);
    QVariant varId = gdlModel->data(idIndex);

    QStringList w, d, t;
    QString str1, str2, str3;
    bool bNeedNotify = false;
    if(fw == NULL)
    {
        w = MFConf()[DCONF_WF];
        str1 = "所有周";
        bNeedNotify = true;
    }
    else
    {
        if(MFConf()[DCONF_WF].indexOf(fw->_4->text()) == -1)
        {
            QMessageBox::warning(this, "错误", "周食谱选择错误");
            return;
        }
        str1 = "周编号="+fw->_4->text();
        w << fw->_4->text();
    }

    if(fd == NULL)
    {
        str2 = "所有日期";
        bNeedNotify = true;
        d = MFConf() [DCONF_WD];
    }
    else
    {
        if(MFConf()[DCONF_WD].indexOf(fd->_4->text()) == -1)
        {
            QMessageBox::warning(this, "错误", "日期选择错误");
            return;
        }
        str2 = "日期="+fd->_4->text();
        d << fd->_4->text();
    }

    if(ft == NULL)
    {
        t = MFConf() [DCONF_DC];
        bNeedNotify = true;
        str3 = "所有餐别";
    }
    else
    {
        if(MFConf()[DCONF_DC].indexOf(ft->_4->text()) == -1)
        {
            QMessageBox::warning(this, "错误", "餐别选择错误");
            return;
        }
        str3 = "餐别="+ft->_4->text();
        t << ft->_4->text();
    }

    if(bNeedNotify)
    {
        if(QMessageBox::Yes != QMessageBox::question(this, "批量加周食谱提示", QString("是否将菜品[%1]加入到：%2，%3，%4").
                                                     arg(varId.toString(), str1, str2, str3),
                                                    QMessageBox::Yes, QMessageBox::No))
            return;
    }

    foreach(QString week, w)
    {
        foreach(QString day, d)
        {
            foreach(QString time, t)
            {
                mylog << week << day << time << varId;
                if(!gdw->insertRecordByValuesAndCommit(QList<QVariant>() << QVariant() << week << day << time << varId))
                {
                    mylog << "submit error " << gdw->lastError().text();
                    gdw->revertAll();
                }
            }
        }
    }
}

void MFSetupParam::showEvent(QShowEvent *)
{
    if(MFGetOper() != "系统管理员")
    {
        this->_table->setItemDelegate(readOnlyDelegate());
        this->_submit->setDisabled(true);
        this->_insert->setDisabled(true);
        this->_delete->setDisabled(true);
        this->_revert->setDisabled(true);
    }
}

void MFDeviceDP::setupGroupBox()
{
    QHBoxLayout* layout = new QHBoxLayout();
    _setup->setLayout(layout);

    layout->addWidget(_downloadGDW = new QCheckBox("下载周食谱"));
    connect(_downloadGDW, SIGNAL(stateChanged(int)), this, SLOT(slotDownloadGDWCheckChange(int)));
    layout->addWidget(_gdwBox = new MFAutoLineEdit()); _gdwBox->setSort();
    layout->addSpacing(20);
    layout->addWidget(_downloadConf = new QCheckBox("下载系统设置"));
    layout->addSpacing(20);
    layout->addWidget(_downloadBlack = new QCheckBox("下载黑名单"));
    layout->addSpacing(20);
    layout->addWidget(_downloadOper = new QCheckBox("下载操作员"));


    layout->addStretch(1);

    _downloadGDW->setChecked(false);
    _downloadConf->setChecked(false);
    _downloadBlack->setChecked(true);
    _downloadOper->setChecked(false);
    _gdwBox->setDisabled(true);

    /* init box selection */
    _gdwModel = MFGetModel("GDW", MFSqlTableModel::QUERY);
    _gdwModel->setFilterAndSelect(QString());
    QList<QVariant> fieldData = _gdwModel->getFieldData(1);
    foreach(QVariant var, fieldData)
    {
        _gdwBox->addItem(var.toString());
    }

    MFGDWSetup* gdwView =(MFGDWSetup*) MFGetWindow(MENU_FOOD_WEEK);

    int curid = _gdwBox->findData(gdwView->_gdw->_weekSelect);
    if(curid != -1)
        _gdwBox->setCurrentIndex(curid);
}

#ifdef WIN32
bool MFDeviceDP::downloadPOSOP(HANDLE hSerial)
{
    if(_downloadOper->checkState() == Qt::Unchecked)
        return true;

    MFSqlTableModel* posop = MFGetModel("DPOSOP", MFSqlTableModel::SHOW);
    int rowCount = posop->rowCount();
    int ret;
    xs_model_t* m = NULL;
    ret = win_model_call(hSerial, &m, QStringList() << __xs_pos<< __xs_oper<< __xs_start);
    if(!isSuccess(ret, m))
        return false;

    for(int i=0; i<rowCount; i++)
    {
        ret = win_model_call(hSerial, &m, QStringList() << __xs_pos<< __xs_oper<< __xs_data<<
                             posop->getData(i, 0).toString()<<
                             posop->getData(i, 1).toString()<<
                             posop->getData(i, 2).toString());
        if(!isSuccess(ret, m))
            return false;
    }

    ret = win_model_call(hSerial, &m, QStringList() << __xs_pos << __xs_oper << __xs_end);
    return isSuccess(ret, m);
}

bool MFDeviceDP::sendFood(HANDLE hSerial, int row)
{
    xs_model_t* m = NULL;
    int ret;
    QString id;
    QString name;
    QString type;
    QString cls;
    QString price;
    QString day;
    QString time;

    MFSqlTableModel* gdl = MFGetModel("GDL", MFSqlTableModel::QUERY);
    /*
            gdw 1-week 2-day, 3--time  4 foodid
     */
    day = _gdwModel->getData(row, GDW_DAY).toString();
    time = _gdwModel->getData(row, GDW_TIME).toString();
    name = _gdwModel->getData(row, GDW_FOOD).toString();
    gdl->setFilterAndSelect(QString("F1='%1'").arg(name));

    if(gdl->rowCount() != 1)
    {
        appendLog(QString("gdl error id=%1").arg(id));

        return false;
    }
    id = gdl->getData(0, 0).toString();
    price = gdl->getData(0, 2).toString();
    cls = gdl->getData(0, 3).toString();
    type = gdl->getData(0, 4).toString();

    day = getSeq(day);

    time = getSeq(time);

    mylog << "day and time is " << day << time;

    ret = win_model_call(hSerial, &m, QStringList()<< __xs_pos<< __xs_food<< __xs_data<<
                         id<<
                         name<<
                         type<<
                         cls<<
                         price<<
                         day<<
                         time);
    return isSuccess(ret, m);
}

bool MFDeviceDP::downloadFood(HANDLE hSerial)
{
    /* dont need download, return true mean success */
    if(_downloadGDW->checkState() == Qt::Unchecked)
        return true;

    if(_gdwBox->currentText().length() == 0)
        return false;

    xs_model_t* m;
    int rowCount;
    int ret = win_model_call(hSerial, &m, QStringList() << __xs_pos << __xs_food << __xs_start);
    if(!isSuccess(ret, m))
        return false;

    _gdwModel->setFilterAndSelect(QString("%1").arg(FIELDEQUAL(GDW_WEEK, _gdwBox->currentText())));
    rowCount = _gdwModel->rowCount();
    for(int i=0; i<rowCount; i++)
    {
        if(!sendFood(hSerial, i))
            return false;
    }

    ret = win_model_call(hSerial, &m, QStringList() << __xs_pos << __xs_food << __xs_end);
    if(!isSuccess(ret, m))
        return false;
    return true;
}
bool MFDeviceDP::deviceSendConf(QString key, QString value, HANDLE hSerial)
{
    xs_model_t* m = NULL;
    int ret;
    ret = win_model_call(hSerial, &m, QStringList() << __xs_pos<< __xs_conf<< __xs_data<< key<< value );
    return isSuccess(ret, m);
}
bool MFDeviceDP::sendConf(HANDLE hSerial)
{
    if(_downloadConf->checkState() == Qt::Unchecked)
        return true;

    xs_model_t* m;
    int ret = win_model_call(hSerial, &m, QStringList() << __xs_pos<< __xs_conf<< __xs_start);
    if(!isSuccess(ret, m))
        return false;

    QMap<QString, QStringList> conf;
    conf = MFConf();
    QString data;
    QStringList list;
    foreach(QString key, conf.keys())
    {
        if(key == DCONF_BOOKDATE)
        {
            if(!deviceSendConf("delay", conf[key].at(0), hSerial))
                return false;
        }

        else if(key == DCONF_COMPANY)
        {
            if(!deviceSendConf("company", conf[key].at(0), hSerial))
                return false;
        }
        else if(key == DCONF_ZONE || key == DCONF_DEPT)
        {
            list = conf[key];
            data = list.join("|");
            if(key == DCONF_ZONE)
            {
                if(!deviceSendConf("zone", data, hSerial))
                    return false;
            }
            else
            {
                if(!deviceSendConf("dept", data, hSerial))
                    return false;
            }
        }
    }

    ret = win_model_call(hSerial, &m, QStringList() << __xs_pos<< __xs_conf<< __xs_end);
    return isSuccess(ret, m);
}
bool MFDeviceDP::downloadBlack(HANDLE hSerial)
{
    if(_downloadBlack->checkState() == Qt::Unchecked)
        return true;

    xs_model_t* m = NULL;

    MFSqlTableModel* model = MFGetModel("DACC", MFSqlTableModel::QUERY);
    model->setFilterAndSelect(QString("%1").arg(FIELDEQUAL(DACC_STATUS, DCONF_CS_GENERAL)));

    int ret = win_model_call(hSerial, &m, QStringList() << __xs_pos<< __xs_black<< __xs_start);
    if(!isSuccess(ret, m))
        return false;

    mylog << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << model->rowCount();
    for(int i=0; i<model->rowCount(); i++)
    {
        ret = win_model_call(hSerial, &m, QStringList() << __xs_pos << __xs_black << __xs_data <<
                             model->getData(i, DACC_CARD).toString());
        if(!isSuccess(ret, m))
            return false;
    }

    ret = win_model_call(hSerial, &m, QStringList() << __xs_pos<< __xs_black<< __xs_end);
    return isSuccess(ret, m);
}

bool MFDeviceDP::handle_dev(HANDLE hSerial)
{
    if(!downloadFood(hSerial))
        return false;

    if(!downloadPOSOP(hSerial))
        return false;

    if(!downloadBlack(hSerial))
        return false;

    return sendConf(hSerial);
}

bool MFDeviceUpgrade::handle_dev(HANDLE hSerial)
{
    char tmp[1024];
    HANDLE hFile;
    int header;
    DWORD readlen;
    xs_model_t* model;
    QString str = _fileName->text();
    QStringList strList = str.split("/");
    str = strList.last();


    model = xs_model_create(4, __xs_pos, __xs_trans, str.toAscii().data(),
                            xs_itoa_r(xs_file_length(_fileName->text().toAscii().data()), tmp));

    if(!serial_send_model(hSerial, model))
    {
        _log->append("send model failure");
        return false;
    }

    model = serial_recv_model(hSerial);
    if(model == NULL)
    {
        _log->append("recv model failure");
        return false;
    }

    if(!xs_success(model->argv(0)))
    {
        xs_model_delete(model);
        _log->append("pos retuan failure");
        return false;
    }

    xs_model_delete(model);

    _log->append("start send file");

    /* hFile = CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,
        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL); */
    hFile = CreateFileA(_fileName->text().toAscii().data(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        mylog << "open file error:" << _fileName->text();
        return false;
    }

    while(1)
    {
        if(!ReadFile(hFile, tmp, sizeof(tmp), &readlen, NULL))
        {
            CloseHandle(hFile);
            _log->append("read file error");
            return false;
        }
        if(readlen  == 0)
        {
            CloseHandle(hFile);
            _log->append("read file length is zero");
            return false;
        }

        header = htonl(readlen);

        if(!serial_send_block(hSerial, (char*)&header, 4))
        {
            CloseHandle(hFile);
            _log->append("send header error");
            return false;
        }

        if(!serial_send_block(hSerial, (char*)tmp, readlen))
        {
            CloseHandle(hFile);
            _log->append("send data error");
            return false;
        }

        if((model = serial_recv_model(hSerial)))
        {
            if(!xs_success(model->argv(0)))
            {
                xs_model_delete(model);
                CloseHandle(hFile);
                _log->append("read response error");
                return false;
            }

            if(atoi(model->argv(1)) == 0)
            {
                _log->append("file trans ok");
                xs_model_delete(model);
                break;
            }
            else
            {
                mylog << "remain filelen = " << model->argv(1);
            }

        }
        else
        {
            CloseHandle(hFile);
            _log->append("read response error");
            return false;
        }

        xs_model_delete(model);
    }

    CloseHandle(hFile);

    return true;
}
#else
#endif
