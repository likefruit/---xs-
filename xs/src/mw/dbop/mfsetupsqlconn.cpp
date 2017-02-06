#include "mfsetupsqlconn.h"

MFSetupSqlConn::MFSetupSqlConn(QWidget *parent) :
    MFWidget(parent)
{
    int i, j;
    QVBoxLayout* _layout = new QVBoxLayout(this);
    QGridLayout* grid = new QGridLayout();
    _layout->addLayout(grid);

    w[0][0] = new QLabel("数据库服务器：");
    w[0][1] = new QLabel("数据库用户名：");
    w[0][2] = new QLabel("数据库密码：");
    w[0][3] = new QLabel("数据库名称：");
    w[0][4] = new QPushButton("初始化数据库");

    w[1][0] = new QLineEdit("127.0.0.1");
    w[1][1] = new QLineEdit("root");
    w[1][2] = new QLineEdit("iamxgl");
    ((QLineEdit*)w[1][2])->setEchoMode(QLineEdit::Password);
    w[1][3] = new QLineEdit("mdb");
    w[1][4] = new QPushButton("退出");

    for(i=0; i<2; i++)
    {
        for(j=0; j<4; j++)
        {
            grid->addWidget(w[i][j], j, i);
        }
    }
    QHBoxLayout* bLayout = new QHBoxLayout();
    grid->addLayout(bLayout, j, 1);
    bLayout->addStretch(1);
    bLayout->addWidget(w[0][4]);
    bLayout->addWidget(w[1][4]);

    _log = new QTextEdit("初始化数据库日志:");
    _layout->addWidget(_log, 1);
    _log->setReadOnly(true);

    connect((QPushButton*)w[1][4], SIGNAL(clicked()), this, SLOT(cancel()));
    connect((QPushButton*)w[0][4], SIGNAL(clicked()), this, SLOT(ok()));
}

bool MFSetupSqlConn::initDb(QString host, QString user, QString pass, QString db)
{
    QSqlDatabase _db = QSqlDatabase::addDatabase("QMYSQL");
    mylog << _db.lastError().text();

    _db.setHostName(host);
    _db.setUserName(user);
    _db.setPassword(pass);
    bool b = _db.open();
    if(!b)
    {
        _log->append(_db.lastError().text());
        mylog << "open database error";
        return false;
    }
    _log->append("成功连接数据库");

    QSqlQuery query(_db);
    b = query.exec("use "+db);
    if(QUERY_RESULT())
    {
        _log->append("该数据库已经存在");
        return true;
    }

    return createDb(_db, db);
}

bool MFSetupSqlConn::createDb(QSqlDatabase& _db, QString db)
{
    bool b;
    QSqlQuery query(_db);
    QString str;

    /* create db */
    b = query.exec("create database "+db +" character set UTF8");
    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }
    _log->append("成功创建数据库");

    b = query.exec("use " + db);
    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }
    _log->append("成功切换数据库");

    /*************************************************************************** */
    /* need pinyin */
    b = query.exec("create table GDL (F0 int(10) primary key auto_increment, "  /* id */
                   "F1 varchar(128) not null, "               /* dinner name*/
                   "F2 double(9,2) default 0.0, "               /* price */
                   "F3 varchar(32) not null, "                    /* DCLASS  */
                   "F4 varchar(32) not null, F5 varchar(16) not null, unique(F1))");                   /* DTYPE  */
    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }

    _log->append("成功创建总菜谱表");

    /*  weekly ... */
    b = query.exec("create table GDW (F0 int(10) primary key auto_increment, F1 varchar(16) not null, " /* 周食谱名称 */
                   "F2 varchar(16) not null, "   /* 日期 */
                   "F3 varchar(16) not null, " /* 餐别 序列号 */
                   "F4 varchar(128) not null, " /* 菜名name */
                   "unique(F1, F2, F3, F4))"
                   );
    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }

    _log->append("成功创建周菜谱表");


    /*************************************************************************** */

    b = query.exec("create table DM (F0 int(10) primary key auto_increment, "     /* 材料id */
                   "F1 varchar(64) not null unique)");         /* 材料名称，唯一，不能为空 */

    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }
    _log->append("成功创建材料表");
    query.exec("insert into DM (F1) values ('西红柿')");
    query.exec("insert into DM (F1) values ('土豆')");
    query.exec("insert into DM (F1) values ('排骨')");
    query.exec("insert into DM (F1) values ('五花肉')");
    query.exec("insert into DM (F1) values ('牛肉')");

    /*************************************************************************** */

    b = query.exec("create table DBOM (F0 int(10) primary key auto_increment, F1 varchar(128) not null, "               /* 菜品name */
                   "F2 varchar(64) not null, "               /* 材料name */
                   "F3 int(10) not null, "               /* 需要量（克） */
                   "unique(F2, F1))");              /* 主键为符合主健 */
    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }

    _log->append("成功创建配料表");

    /*************************************************************************** */

    b = query.exec("create table DCONF (F0 int(10) primary key auto_increment, F1 varchar(32) not null, "  /* 配置key */
                   "F2 varchar(2048) not null)");                    /* 配置value，这个表用来保存程序的配置信息 */
    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }

    _log->append("成功创建参数表");

    QMap<QString, QString> conf;
    conf.insert(DCONF_DACC_TYPE, "普通账户|网络账户");
    conf.insert(DCONF_DTYPE, "荤菜|素菜|主食|汤");
    conf.insert(DCONF_COMPANY, "广州军区武汉总医院");
    conf.insert(DCONF_COMPANY_TEL, "81112222");
    conf.insert(DCONF_BOOKDATE, "1");
    conf.insert(DCONF_DCLASS, "普灶|戒糖|戒盐|低嘌呤|少油");
    conf.insert(DCONF_WF, "[01]第一周|[02]第二周|[03]第三周|[04]第四周|[05]第五周");
    conf.insert(DCONF_WD, "[1]星期一|[2]星期二|[3]星期三|[4]星期四|[5]星期五|[6]星期六|[7]星期日");
    conf.insert(DCONF_DC, "[1]早餐|[2]中餐|[3]晚餐");
    conf.insert(DCONF_CS, DCONF_CS_ALL);
    conf.insert(DCONF_CT, DCONF_CT_ALL);
    conf.insert(DCONF_ZONE, "专家楼|内科大楼|外科大楼|外围大楼");
    conf.insert(DCONF_DEPT, "神经内科|内分泌科|消化内科|呼吸内科|肿瘤治疗|普通外科|泌尿外科|骨科|妇产科");
    conf.insert(DCONF_DEPOSIT, "10");
    conf.insert(DCONF_CHANGE_CARD, "10");
    conf.insert(DCONF_OP_ACC, DCONF_OP_ACC_ALL);
    conf.insert(DCONF_SUPPLY_RECICLE, "是");
    conf.insert(DCONF_SUPPLY_AMOUNT, "300");
    QMap<QString, QString>::iterator it;
    for(it=conf.begin(); it!=conf.end(); it++)
    {
        query.prepare("insert into DCONF (F1, F2) values (?, ?)");
        query.addBindValue(it.key());
        query.addBindValue(it.value());
        query.exec();
    }

    /*************************************************************************** */

    b = query.exec("create table DLOG (F0 int(10) auto_increment primary key, "  /* 日志记录id */
                   "F1 TIMESTAMP not null default now(), "   /* 日志记录时间点 */
                   "F2 text not null)");                     /* 日志 */

    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }
    _log->append("成功创建日志表");

    /*************************************************************************** */

    b = query.exec("create table DOP (F0 int(10) primary key auto_increment, "  /* 操作员id，这个操作员是操作软件的操作员，不是pos机器操作员 */
                   "F1 varchar(32) unique not null, "         /* 操作员名字 */
                   "F2 varchar(64) , "                /* 操作员密码 */
                   "F3 varchar(32) not null, F4 varchar(128))");               /* 操作员电话号码 */
    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }

    _log->append("成功创建管理员表");
    query.exec(QString("insert into DOP (F1, F2, F3, F4) values ('系统管理员', '', '13388888888', '%1')").arg(DCONF_MD_ALL));
    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }

    /*****************************************************************/
    /* account */
    b = query.exec();

    /* need pinyin */
    str = "create table DACC (F0 int(10) primary key auto_increment, "
            "F1 varchar(128) not null, " /* acc name*/
            "F2 varchar(32) not null, " /* 电话号码 */
            "F3 varchar(32) not null, " /* identify */
            "F4 varchar(32), "               /*病区*/
            "F5 varchar(32), "              /*科室*/
            "F6 varchar(32), "              /* 床位 */
            "F7 bigint(20) not null, "             /* DCARD */
            "F8 varchar(16) not null, "            /* 状态 */
            "F9 double(9,2) default 0.0, "       /* 余额 */
            "F10 double(9,2) default 0.0, "       /* 押金 */
            "F11 varchar(16) , "                /* 结算方式 */
            "F12 double(9,2) default 0.0,"      /* 结算金额 */
            "F13 varchar(16),"      /* 拼音码 */
            "F14 varchar(64),"      /* 密码 */
            "F15 varchar(16),"      /* 帐户类型 */
            "F16 double(9,2) default 0.0,"      /* 月补助金额 */
            "F17 varchar(16))"      /* 领补助日期 */
            ;

    b = query.exec(str);

    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }
    _log->append("成功创建账户表");

    /* device */

    b = query.exec("create table DD ("
                   "F0 int(10) primary key auto_increment, "
                   "F1 varchar(128) not null unique, "           /*device id*/
                   "F2 varchar(16) not null, "                   /* type,pos机，读卡器，终端刷卡机  */
                   "F3 varchar(1024)"                           /* mem, this device location etc */
                   ")");

    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }
    _log->append("成功创建设备表");

    /* posop ***************************************************** */

    b = query.exec("create table DPOSOP ("
                   "F0 int(10) primary key auto_increment, "
                   "F1 varchar(16) not null unique, "           /*姓名*/
                   "F2 int(10) not null, "                   /* 操作者密码 */
                   "F3 varchar(32)"                           /* 电话 */
                   ")");


    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }
    _log->append("成功创建手持机/终端操作员表");

    /*************************************************************************** */
    b = query.exec("create table DFLOW (F0 bigint(10) primary key auto_increment, " /* 自动增长*/
                   "F1 int(10) not null, "              /* 账户号 */
                   "F2 bigint(20) not null, "              /* 卡号 */
                   "F3 double(9,2) not null, "    /* 操作金额 */
                   "F4 varchar(32) not null, "  /* 方式，（订餐扣费，月结扣费，日结算扣费，充值，立即消费扣费 */
                   "F5 double(9,2) not null, "                   /* 本次操作后卡余额 */
                   "F6 varchar(32) not null, "                   /* 操作员 */
                   "F7 varchar(32) not null , F8 varchar(32) not null, "                   /* 操作日期 原来用的TIMESTAMP，由于Error while moving album - Illegal mix of collations | Gallery改成了字符串，字符集问题不好解决*/
                   "F9 varchar(128) not null unique)"                  /* 订单单号 */
                   );

    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }
    _log->append("成功创建账户流水表");

    /*************************************************************************** */
    b = query.exec("create table DORDER (F0 bigint(10) primary key auto_increment, " /* 自动增长*/
                   "F1 varchar(128) not null, "              /* 订单号 */
                   "F2 varchar(128) not null, "                  /* 菜品名称 */
                   "F3 int(10) not null, "                  /* 数量 */
                   "F4 double(9,2) not null, "              /* 价格 */
                   "F5 varchar(32) not null, "              /* 订餐员 */
                   "F6 varchar(16) not null, "              /* 订单状态 */
                   "F7 varchar(128) , "              /* 错误状态 */
                   "F8 varchar(32), "
                   "F9 varchar(32), "
                   "F10 varchar(32) , "
                   "unique(F1,F2,F9))"                               /* 就餐日期 和餐别 就餐时间（即时消费）*/
                   );

    if(!QUERY_RESULT())
    {
        QUERY_ERR();
    }
    _log->append("成功创建订单流水表");

    _log->append("点击退出按钮退出本界面，进入系统登录界面");

    return true;

}

void MFSetupSqlConn::saveconfig(QString host, QString user, QString pass, QString db)
{
    xs_dict_t* dict = xs_dict_add_str(NULL, "host", host.toUtf8());
    xs_dict_add_str(dict, "user", user.toUtf8());
    xs_dict_add_str(dict, "pass", pass.toUtf8());
    xs_dict_add_str(dict, "db", db.toUtf8());

    if(xs_config_save("db.conf", dict) < 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("保存数据库配置失败"));
    }
    else
    {
        //   this->accept();
    }
}

void MFSetupSqlConn::ok()
{
    mylog << "onok clicked";
    QLineEdit* _user;
    QLineEdit* _pass;
    QLineEdit* _host;
    QLineEdit* _db;
    _host = (QLineEdit*)w[1][0];
    _user = (QLineEdit*)w[1][1];
    _pass = (QLineEdit*)w[1][2];
    _db = (QLineEdit*)w[1][3];

    if(_user->text().length() &&
            _pass->text().length() &&
            _host->text().length() &&
            _db->text().length())
    {
        if(initDb( _host->text(),_user->text(), _pass->text(), _db->text()))
        {
            saveconfig( _host->text(),_user->text(), _pass->text(), _db->text());
        }
        else
        {
            _log->append("有错误发生，点击退出按钮退出程序，请检查数据库连接配置信息");
        }
    }
    else
    {
        QMessageBox::warning(this, tr("错误"), tr("必须填写所有空格"));
    }

}

void MFSetupSqlConn::cancel()
{
    if(xs_file_exist("db.conf"))
    {
        MFShowWindow(MENU_SYS_LOGIN);
    }
    else
    {
        qApp->quit();
    }
}
