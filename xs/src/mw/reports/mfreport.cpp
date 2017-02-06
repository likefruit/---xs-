
#include "mfreport.h"


bool MFReportDPArea::setBody()
{
    if(_report->_comboStyle->currentText() == MW_REPORT_STYLE_NORMAL)
    {
        return setBodyNormal();
    }
    else if(_report->_comboStyle->currentText() == MW_REPORT_STYLE_COMPACT)
    {
        return setBodyCompact();
    }
    else
    {
        pText("正在建设中...");
    }
    return false;
}

bool MFReportDPArea::setBodyCompact()
{
    QSqlQuery query;
    QString str = QString("select GDL.%9, GDW.%1,GDW.%2,GDW.%3, GDL.%4, GDL.%5, GDL.%6 from GDL inner join GDW on GDL.%7 = GDW.%3 where %8").
            arg(FIELD(GDW_DAY),
                FIELD(GDW_TIME),
                FIELD(GDW_FOOD),
                FIELD(GDL_PRICE),
                FIELD(GDL_TYPE),
                FIELD(GDL_CLASS),
                FIELD(GDL_NAME),
                _filterString,
                FIELD(GDL_SEQ)
                );
    bool bRet = query.exec(str);
    if(!bRet)
    {
        mylog << str;
        return bRet;
    }

    mylog << str;

    QMap<QString, QMap<QString, QList<MFReportRecord*>* >* > values;
    QMap<QString, QList<MFReportRecord*>* >* time;
    QList<MFReportRecord*>* day;
    MFReportRecord* r;

    while(query.next())
    {
        bRet = true;
        int seq = query.value(0).toInt();
        QString d = query.value(1).toString();
        QString t = query.value(2).toString();
        QString name = query.value(3).toString();
        double p = query.value(4).toDouble(); mylog << p;
        QString type = query.value(5).toString();
        QString cls = query.value(6).toString();


        QString book_id;

        QString str = query.value(2).toString(); /* cb */
        str = QString::number(str.split("[").at(1).split("]").at(0).toInt());
        book_id  = str + "." + QString::number(seq);

        time = values.value(t);
        mylog << d;
        if(time == NULL)
        {
            time = new QMap<QString, QList<MFReportRecord*>* >();
            values.insert(t, time);
        }

        day = time->value(d);
        mylog << t;
        if(day == NULL)
        {
            day = new QList<MFReportRecord*>();
            time->insert(d, day);
        }

        r = new MFReportRecord(10);
        r->setValue(0, book_id);
        r->setValue(1, name);
        r->setValue(2, p);
        r->setValue(3, cls);
        r->setValue(4, type);

        day->append(r);
    }

    //    QStringList ws = values.begin().value()->keys();

    beginTable(QString(), 0);

    foreach(QString t, values.keys())
    {
        time = values.value(t);
        beginRow();
        foreach(QString d, time->keys())
        {
            th(d+":"+t);
        }
        endRow();


        beginRow();
        foreach(QString d, time->keys())
        {
            day = time->value(d);
            beginTd();
            beginTable();
            beginRow();
            th("编号");
            th("菜名");
            th("单价");

            endRow();
            //foreach(r, *day)
            for(int i=0; i<day->count(); i++)
            {
                r = day->at(i);
                beginRow();
                td(r->value(0).toString());
                td(r->value(1).toString());
                td(r->value(2).toString());
                delete r;
                endRow();
            }
            endTable();

            endTd();
            delete day;
        }
        endRow();
        delete time;
    }

    endTable();
    return bRet;
}

bool MFReportDPArea::setBodyNormal()
{
    QSqlQuery query;
    QString str = QString("select GDL.%9, GDW.%1,GDW.%2,GDW.%3, GDL.%4, GDL.%5, GDL.%6 from GDL inner join GDW on GDL.%7 = GDW.%3 where %8").
            arg(FIELD(GDW_DAY),
                FIELD(GDW_TIME),
                FIELD(GDW_FOOD),
                FIELD(GDL_PRICE),
                FIELD(GDL_TYPE),
                FIELD(GDL_CLASS),
                FIELD(GDL_NAME),
                _filterString,
                FIELD(GDL_SEQ)
                );
    bool bRet = query.exec(str);
    if(!bRet)
    {
        mylog << str;
        return bRet;
    }

    mylog << str;

    QSqlRecord rec = query.record();
    QString cb;
    QString day;
    while(query.next())
    {
        bRet = true;
        QString cb1 = query.value(2).toString();
        QString day1 = query.value(1).toString();

        if(cb1 != cb || day1 != day)
        {
            if(cb.length())
            {
                endTable();
                br();
            }
            cb = cb1;
            day = day1;
            beginTable(QString("日期：%1    餐别：%2").arg(day1, cb1));
            beginRow();

            th("订餐编号");
            th("日期");
            th("餐别");
            th("菜品名称");
            th("单价(元)");
            th("类型");
            th("灶类");

            endRow();
        }

        beginRow();

        QString str = query.value(2).toString(); /* cb */
        str = QString::number(str.split("[").at(1).split("]").at(0).toInt());

        for(int i=0; i<rec.count(); i++)
        {
            if(i == 0)
                td(str + "." +query.value(i).toString());
            else
                td(query.value(i).toString());
        }

        endRow();
    }


    endTable();

    return bRet;
}

MFReportDP::MFReportDP(QWidget *parent) : MFReport(new MFReportDPArea(), parent)
{
    setTitle(MENU_REPORT_GDW);

    _filter->addFilter("GDW", FIELD(GDW_WEEK), "周食谱：", MFREPORT_FILTER_SINGLE, QString());
    _filter->addFilter("GDW", FIELD(GDW_DAY), "日期：", MFREPORT_FILTER_SINGLE, QString());
    _filter->addFilter("GDL", FIELD(GDL_CLASS), "灶类：", MFREPORT_FILTER_SINGLE, QString());
    _filter->addFilter("GDW", FIELD(GDW_TIME), "餐别：", MFREPORT_FILTER_RANGE, QString());

}


bool MFReportAreaZuofan::setBody()
{
    bool bRet = false;
    QString where = _filterString;
    where += QString("and (DORDER.%1)").arg(FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK));

    QString strSql;
    strSql = QString("select DORDER.%1, DORDER.%2, DORDER.%3, GDL.%4, GDL.%7, GDL.%8 "
                     "from DORDER inner join GDL on DORDER.%2=GDL.%5 where %6")
            .arg(FIELD(DORDER_COUNT), FIELD(DORDER_DINNER),
                 FIELD(DORDER_CB), FIELD(GDL_CLASS),
                 FIELD(GDL_NAME), where, FIELD(GDL_TYPE), FIELD(GDL_PRICE));
    QSqlQuery query;
    if(!query.exec(strSql))
    {
        return bRet;
    }

    QMap<QString, MFReportRecord*> values;
    while(query.next())
    {
        bRet = true;
        QString cb = query.value(2).toString();
        QString dinner = query.value(1).toString();
        int c = query.value(0).toInt();
        QString key =  cb+dinner;
        QString cls = query.value(3).toString();
        QString type = query.value(4).toString();
        double price = query.value(5).toDouble();

        if(values.find(key) == values.end())
        {
            MFReportRecord* d = new MFReportRecord(20);
            d->setValue(0, c);
            d->setValue(1, cb);
            d->setValue(2, dinner);
            d->setValue(3, cls);
            d->setValue(4, type);
            d->setValue(5, price*c);
            values[key] = d;
        }
        else
        {
            MFReportRecord* d = values[key];
            d->setValue(0, c + d->value(0).toInt());
        }
    }

    double total = 0;

    beginTable();
    beginRow();

    th("餐别");
    th("菜品名称");
    th("数量");
    th("灶类");
    th("类别");
    th("总价");


    endRow();
    foreach(QString key, values.keys())
    {
        MFReportRecord* r = values[key];
        beginRow();
        td(r->value(1).toString());
        td(r->value(2).toString());
        td(r->value(0).toString());
        td(r->value(3).toString());
        td(r->value(4).toString());
        td(r->value(5).toString());
        total += r->value(5).toDouble();
        delete r;
        endRow();
    }

    beginRow();
    td("");
    td("");
    td("");
    td("");
    td("合计：");
    td(QString::number(total));
    endRow();
    endTable();

    return bRet;
}

MFReportZuofan::MFReportZuofan(QWidget *parent) : MFReport(new MFReportAreaZuofan(), parent)
{
    setTitle(MENU_REPORT_ZUOFAN);

    _filter->addFilter("DORDER", FIELD(DORDER_DAY), "日期：", MFREPORT_FILTER_SINGLE, QString("%1 and %2").arg(FIELDLIKE(DORDER_ORDERID, "o"), FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK)));
    _filter->addFilter("DORDER", FIELD(DORDER_CB), "餐别：", MFREPORT_FILTER_SINGLE, QString("%1 and %2").arg(FIELDLIKE(DORDER_ORDERID, "o"), FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK)));
    _filter->addFilter("GDL", FIELD(GDL_CLASS), "灶类：", MFREPORT_FILTER_SINGLE, QString());
    _filter->addFilter("GDL", FIELD(GDL_TYPE), "类型：", MFREPORT_FILTER_SINGLE, QString());
}

bool MFReportAreaLinfan::setBody()
{
    bool bRet = false;
    QString where = _filterString;
    where += QString("and (DORDER.%1)").arg(FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK));

    QString strSql;
    strSql = QString("select DORDER.%1, DORDER.%2, DORDER.%3, DACC.%4, DORDER.%10 "
                     "from (DFLOW inner join DACC on DFLOW.%5=DACC.%6) inner join DORDER on DFLOW.%7=DORDER.%8 where %9")
            .arg(FIELD(DORDER_COUNT), FIELD(DORDER_DINNER),
                 FIELD(DORDER_CB), FIELD(DACC_AREA),
                 FIELD(DFLOW_ACC_SEQ), FIELD(DACC_SEQ), FIELD(DFLOW_ORDERID), FIELD(DORDER_ORDERID), where).arg(FIELD(DORDER_POSOP));

    mylog << strSql;
    QSqlQuery query;
    if(!query.exec(strSql))
    {
        return bRet;
    }

    MFReportRecord* r;
    QMap<QString, MFReportRecord*>* p;

    QMap<QString, QMap<QString, MFReportRecord*>* > values;
    while(query.next())
    {
        bRet = true;
        int c = query.value(0).toInt();
        QString d = query.value(1).toString();
        QString cb = query.value(2).toString();
        QString area = query.value(3).toString();
        QString oper = query.value(4).toString();

        QString key = /*"订餐员："+oper+"  " +*/ "病区："+area;

        if(values.find(key) == values.end())
        {
            p = new QMap<QString, MFReportRecord*>();
            values[key] = p;
        }
        else
        {
            p = values[key];
        }

        if(p->find(d) == p->end())
        {
            r = new MFReportRecord(5);
            p->insert(d, r);
            r->setValue(0, 0);
            r->setValue(1, cb);
            r->setValue(2, area);
            r->setValue(3, oper);
        }
        else
        {
            r = p->value(d);
        }
        r->setValue(0, r->value(0).toInt() + c);
    }

    foreach(QString key, values.keys())
    {
        p = values.value(key);

        pText(key);
        beginTable();
        beginRow();

        th("菜品名称");
        th("数量");

        endRow();
        foreach(QString d, p->keys())
        {
            beginRow();
            r = p->value(d);
            td(d);
            td(r->value(0).toString());
            delete r;
            endRow();

        }
        delete p;
        endTable();
    }

    return bRet;
}

MFReportLinfan::MFReportLinfan(QWidget *parent): MFReport(new MFReportAreaLinfan(), parent)
{
    setTitle(MENU_REPORT_LINFAN);

    _filter->addFilter("DORDER", FIELD(DORDER_DAY), "日期：", MFREPORT_FILTER_SINGLE, QString("%1 and %2").arg(FIELDLIKE(DORDER_ORDERID, "o"), FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK)));
    _filter->addFilter("DORDER", FIELD(DORDER_CB), "餐别：", MFREPORT_FILTER_SINGLE, QString("%1 and %2").arg(FIELDLIKE(DORDER_ORDERID, "o"), FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK)));
    _filter->addFilter("DACC", FIELD(DACC_AREA), "病区：", MFREPORT_FILTER_SINGLE, QString());
    _filter->addFilter("DACC", FIELD(DACC_DEPT), "科室：", MFREPORT_FILTER_SINGLE, QString());
}

bool MFReportAreaFafan::setBody()
{
    bool bRet = false;
    QString where = _filterString;
    where += QString("and (DORDER.%1)").arg(FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK));

    QString strSql;
    strSql = QString("select DORDER.%1, DORDER.%2, DORDER.%3, DACC.%4, DORDER.%10, DACC.%11, DACC.%12, DACC.%13 "
                     "from (DFLOW inner join DACC on DFLOW.%5=DACC.%6) inner join DORDER on DFLOW.%7=DORDER.%8 where %9")
            .arg(FIELD(DORDER_COUNT), FIELD(DORDER_DINNER),
                 FIELD(DORDER_CB), FIELD(DACC_AREA),
                 FIELD(DFLOW_ACC_SEQ), FIELD(DACC_SEQ), FIELD(DFLOW_ORDERID), FIELD(DORDER_ORDERID), where)
            .arg(FIELD(DORDER_POSOP), FIELD(DACC_DEPT), FIELD(DACC_BED), FIELD(DACC_NAME));
    QSqlQuery query;
    if(!query.exec(strSql))
    {
        return bRet;
    }

    MFReportRecord* r;
    QMap<QString, MFReportRecord*>* p;
    QMap<QString, QMap<QString, MFReportRecord*>* > values;
    QMap<QString, int> statics;

    while(query.next())
    {
        bRet = true;
        int c = query.value(0).toInt();
        QString d = query.value(1).toString();
        QString cb = query.value(2).toString();
        QString area = query.value(3).toString();
        //   QString oper = query.value(4).toString();
        QString dept = query.value(5).toString();
        QString bed = query.value(6).toString();
        QString name = query.value(7).toString();
        QString key = area +" "+ cb;

        if(values.find(key) == values.end())
        {
            p = new QMap<QString, MFReportRecord*>();
            values.insert(key, p);
        }
        else
        {
            p = values.value(key);
        }

        QString str = d+":"+QString::number(c);
        if(p->find(name) == p->end())
        {
            r = new MFReportRecord(10);
            r->setValue(0, area+"/"+dept+"/"+bed);
            r->setValue(1, str);
            r->setValue(2, cb);
            r->setValue(3, name);
            p->insert(name, r);
        }
        else
        {
            r = p->value(name);
            r->setValue(1, r->value(1).toString()+"  " + str);
        }

        if(statics.find(d) == statics.end())
        {
            statics.insert(d, c);
        }
        else
        {
            statics.insert(d, statics.value(d)+c);
        }
    }


    foreach(QString key, values.keys())
    {
        pText(key);
        p = values.value(key);
        beginTable();
        beginRow();

        th("餐别",10);
        th("姓名",15);
        th("地址",20);

        th("菜品明细",55);


        endRow();
        mylog << p->keys();
        foreach(QString name, p->keys())
        {
            beginRow();
            r = p->value(name);

            td(r->value(2).toString(), "center", 1, 10);
            td(r->value(3).toString(), "center", 1, 15);
            td(r->value(0).toString(), "center", 1, 20);
            td(r->value(1).toString(), "center", 1, 55);
            delete r;
            endRow();
        }
        delete p;
        endTable();
    }

    pText("统计");
    beginTable();
    QStringList keys = statics.keys();
    beginRow();
    foreach(QString key, keys)
    {
        th(key);
    }
    endRow();
    beginRow();
    foreach(QString key, keys)
    {
        td(QString::number(statics.value(key)));
    }
    endRow();

    endTable();
    return bRet;
}

MFReportFafan::MFReportFafan(QWidget *parent)
    : MFReport(new MFReportAreaFafan(), parent)
{
    setTitle(MENU_REPORT_FAFAN);

    _filter->addFilter("DORDER", FIELD(DORDER_DAY), "日期：", MFREPORT_FILTER_SINGLE, QString("%1 and %2").arg(FIELDLIKE(DORDER_ORDERID, "o"), FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK)));
    _filter->addFilter("DORDER", FIELD(DORDER_CB), "餐别：", MFREPORT_FILTER_SINGLE, QString("%1 and %2").arg(FIELDLIKE(DORDER_ORDERID, "o"), FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK)));
    _filter->addFilter("DACC", FIELD(DACC_AREA), "病区：", MFREPORT_FILTER_SINGLE, QString());
    _filter->addFilter("DACC", FIELD(DACC_DEPT), "科室：", MFREPORT_FILTER_SINGLE, QString());


    this->_bottomMiddleLayout->addWidget(_batchPrintByDept = new QPushButton("按科室批量打印"));
    this->_bottomMiddleLayout->addWidget(_batchPrintByZone = new QPushButton("按病区批量打印"));

    connect(_batchPrintByDept, SIGNAL(clicked()), this, SLOT(batchPrint()));
    connect(_batchPrintByZone, SIGNAL(clicked()), this, SLOT(batchPrint()));
}

void MFReportFafan::batchPrint()
{
    MFFilterWidget* filter = this->_filter->_filters.at(0);
    if(filter->_box->currentIndex() == 0)
    {
        QMessageBox::warning(this, "错误", "批量打印，首先要在界面左上角过滤条件中选择日期");
        return;
    }

    QObject* obj = sender();
    if(obj == _batchPrintByDept)
    {
        this->_filter->_filters.at(2)->_box->setCurrentIndex(0);
        filter =  this->_filter->_filters.at(3);
    }
    else
    {
        this->_filter->_filters.at(3)->_box->setCurrentIndex(0);
        filter =  this->_filter->_filters.at(2);
    }

    filter->_box->count();
    QPrinter printer;
    QPrintDialog dlg(&printer);
    if(dlg.exec() == QDialog::Accepted)
    {
        for(int i=1; i<filter->_box->count(); i++)
        {
            filter->_box->setCurrentIndex(i);
            if(this->_reportArea->updateContent())
            {
                QSizeF size = _reportArea->document()->pageSize();
                _reportArea->document()->setPageSize(printer.pageRect().size());
                this->_reportArea->print(&printer);
                _reportArea->document()->setPageSize(size);
            }
        }
    }
}


bool MFReportAreaCaigou::setBody()
{
    bool bRet = false;
    /* query first */
    QString where = _filterString;
    where += QString("and (DORDER.%1)").arg(FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK));

    QString strSql;
    strSql = QString("select DORDER.%1, DORDER.%2, DBOM.%3, DBOM.%4 "
                     "from (DORDER left join DBOM on DORDER.%2=DBOM.%5) left join GDL on DORDER.%2=GDL.%7 where %6 ")
            .arg(FIELD(DORDER_COUNT), FIELD(DORDER_DINNER),
                 FIELD(DBOM_METERIAL), FIELD(DBOM_WEIGHT),
                 FIELD(DBOM_DINNER), where, FIELD(GDL_NAME))
            ;
    QSqlQuery query;
    if(!query.exec(strSql))
    {
        mylog << query.lastError().text();
        mylog << strSql;
        return bRet;
    }
    QMap<QString, int> values;
    while(query.next())
    {
        bRet = true;
        mylog << query.value(0) << query.value(1) << query.value(2) << query.value(3);
        int c = query.value(0).toInt();
        QString d = query.value(1).toString();
        QString m = query.value(2).toString();
        int w = query.value(3).toInt();
        if(m.length() == 0)
        {
            QMessageBox::warning(this, "错误", QString("菜品(%1)没有填写材料配比，不能生成领料表").arg(d));
        }

        if(values.find(m) == values.end())
        {
            values.insert(m, w*c);
        }
        else
        {
            values.insert(m, values.value(m) + w*c);
        }

    }
    beginTable();
    beginRow();

    th("材料");
    th("重量(克)");
    th("重量(斤)");


    endRow();
    foreach(QString m, values.keys())
    {
        beginRow();
        td(m);
        td(QString::number(values.value(m)));
        td(QString::number(values.value(m)*1.0/500));
        endRow();
    }
    endTable();
    return bRet;
}

MFReportCaigou::MFReportCaigou(QWidget *parent) : MFReport(new MFReportAreaCaigou(), parent)
{
    setTitle(MENU_REPORT_CAIGOU);

    _filter->addFilter("DORDER", FIELD(DORDER_DAY), "日期：", MFREPORT_FILTER_SINGLE,
                       QString("%1 and %2").arg(FIELDLIKE(DORDER_ORDERID, "o"), FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK)));
    _filter->addFilter("GDL", FIELD(GDL_TYPE), "类型：", MFREPORT_FILTER_SINGLE, QString());
    _filter->addFilter("DORDER", FIELD(DORDER_CB), "餐别：", MFREPORT_FILTER_SINGLE,
                       QString("%1 and %2").arg(FIELDLIKE(DORDER_ORDERID, "o"), FIELDEQUAL(DORDER_STATUS, MD_ORDER_STATUS_OK)));
}

bool MFReportAreaCaiwu::setBody()
{
    bool bRet = false;
    QString strSql;
    strSql = QString("select DFLOW.%1,DFLOW.%2,DFLOW.%3, DFLOW.%4, DFLOW.%5, DACC.%6 from DFLOW inner join DACC on DFLOW.%7=DACC.%8 where %9").
            arg(FIELD(DFLOW_AMOUNT), FIELD(DFLOW_METHOD), FIELD(DFLOW_OPER), FIELD(DFLOW_DATE), FIELD(DFLOW_TIME), FIELD(DACC_NAME)).
            arg(FIELD(DFLOW_ACC_SEQ), FIELD(DACC_SEQ)).
            arg(_filterString);
    QSqlQuery query;
    if(!query.exec(strSql))
    {
        mylog << query.lastError().text();
        mylog << strSql;
        return bRet;
    }
    beginTable();
    beginRow();

    th("操作员");
    th("业务");
    th("操作金额");
    th("日期");
    th("时间");
    th("账户");

    endRow();

    QStringList condition;
    condition << DCONF_OP_ACC_CHARGE<<DCONF_OP_ACC_OPEN<<DCONF_OP_ACC_CLOSE;
    double total = 0;
    while(query.next())
    {
        bRet = true;
        QString method = query.value(1).toString();
        double money;
        if(method == DCONF_OP_ACC_CLOSE)
            money = -query.value(0).toDouble();
        else
            money = query.value(0).toDouble();

        QString oper = query.value(2).toString();
        QString date = query.value(3).toString();
        QString time = query.value(4).toString();
        QString acc = query.value(5).toString();

        if(condition.indexOf(method) != -1)
        {
            beginRow();
            td(oper);
            td(method);
            td(QString::number(money));
            td(date);
            td(time);
            td(acc);
            endRow();
            total += money;
        }
    }

    endTable();

    br();
    pText(QString("余额统计：%1").arg(total));
    return bRet;
}

MFReportCaiwu::MFReportCaiwu(QWidget *parent) : MFReport(new MFReportAreaCaiwu(), parent)
{
    setTitle(MENU_REPORT_CAIWU);

    _filter->addFilter("DFLOW", FIELD(DFLOW_OPER), "操作员：", MFREPORT_FILTER_SINGLE, QString());
    _filter->addFilter("DFLOW", FIELD(DFLOW_DATE), "日期：", MFREPORT_FILTER_RANGE, QString());
}

bool MFReportAreaPersonalFlow::setBody()
{
    // 示该用户的 名字，卡号，业务名称，业务金额，余额，操作日期，操作时间
    bool bRet = false;
    QString strSql;
    strSql = QString("select DACC.%1,DACC.%2,DFLOW.%3, DFLOW.%4, DFLOW.%5, DFLOW.%6, DFLOW.%7 "
                     "from DFLOW inner join DACC on DFLOW.%8=DACC.%9 where %10").
            arg(FIELD(DACC_NAME), FIELD(DACC_CARD), FIELD(DFLOW_METHOD), FIELD(DFLOW_AMOUNT), FIELD(DFLOW_BALANCE),
                FIELD(DFLOW_DATE)).arg(FIELD(DFLOW_TIME)).
            arg(FIELD(DFLOW_ACC_SEQ), FIELD(DACC_SEQ)).
            arg(_filterString);
    QSqlQuery query;
    if(!query.exec(strSql))
    {
        mylog << query.lastError().text();
        mylog << strSql;
        return bRet;
    }
    beginTable();
    beginRow();

    th("姓名");
    th("卡号");
    th("业务名称");
    th("业务金额");
    th("余额");
    th("操作日期");
    th("操作时间");

    endRow();


    QSqlRecord r = query.record();
    while(query.next())
    {
        bRet = true;
        beginRow();
        for(int i=0; i<r.count(); i++)
        {
            td(query.value(i).toString());
        }
        endRow();
    }

    endTable();
    return bRet;
}

MFReportPersonalFlow::MFReportPersonalFlow(QWidget *parent) : MFReport(new MFReportAreaPersonalFlow(), parent)
{
    setTitle(MENU_REPORT_PERSONAL);

    _filter->addFilter("DACC", FIELD(DACC_ID), "住院号：", MFREPORT_FILTER_SINGLE, QString());
    _filter->addFilter("DFLOW", FIELD(DFLOW_DATE), "日期：", MFREPORT_FILTER_RANGE, QString());
}
