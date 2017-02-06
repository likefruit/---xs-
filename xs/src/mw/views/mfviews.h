#ifndef MFVIEWS_H
#define MFVIEWS_H

#include "mftableviewcontainer.h"
#include "mddef.h"
#include "mfgdwcopy.h"

class MFAboutThisWork : public MFWidget
{
    Q_OBJECT
public:
    explicit MFAboutThisWork(QWidget *parent = 0) :
        MFWidget(parent)
    {
        QLabel* l = new QLabel(QString(MW_APP_NAME"\n2013-2023 东方易畅（北京）信息技术有限公司 版权所有")/*.arg(MFConf()[DCONF_COMPANY].at(0))*/);
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(l);
    }

signals:

public slots:

};


class MFAboutThisWork1 : public MFWorkArea
{
    Q_OBJECT
public:
    explicit MFAboutThisWork1(QWidget *parent = 0):
        MFWorkArea(NULL, new MFAboutThisWork(), parent)
    {
        setTitle(MENU_ABOUT_RIGHT);
        this->_commandBox->hide();
    }
};

class MFLogin : public MFWidget
{
    Q_OBJECT
public:
    explicit MFLogin(QWidget *parent = 0):
        MFWidget(parent)
    {
        _db = QSqlDatabase::addDatabase("QMYSQL");
        xs_dict_t* dict = xs_config_load("db.conf");
        _db.setHostName(xs_dict_find_value(dict, "host"));
        _db.setUserName(xs_dict_find_value(dict, "user"));
        _db.setPassword(xs_dict_find_value(dict, "pass"));
        _db.setDatabaseName(xs_dict_find_value(dict, "db"));
        _db.open();

        _user = new QLineEdit("系统管理员");
        _pass = new QLineEdit();
        _pass->setEchoMode(QLineEdit::Password);

        QPushButton* ok = new QPushButton("登陆");
        QPushButton* quit = new QPushButton("退出");

        QGridLayout* grid = new QGridLayout(this);
        QHBoxLayout* hLayout = new QHBoxLayout();
        grid->setColumnStretch(0, 1);
        grid->setColumnStretch(3, 1);

        grid->addWidget(new QLabel("用户名:"), 1, 1);
        grid->addWidget(new QLabel("密码："), 2, 1);
        grid->addWidget(_user, 1, 2);
        grid->addWidget(_pass, 2, 2);

        grid->addLayout(hLayout, 3, 2);
        hLayout->addStretch(1);
        hLayout->addWidget(ok);
        hLayout->addWidget(quit);

        grid->setRowStretch(0, 6);
        grid->setRowStretch(4, 10);

        connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));
        connect(ok, SIGNAL(clicked()), this, SLOT(okClicked()));
        _login = false;
    }
    QLineEdit* _user;
    QLineEdit* _pass;
    QString _auth;
    bool _login;

    void keyPressEvent(QKeyEvent *ev)
    {
        if(ev->key() == (int)Qt::Key_Return)
        {
            okClicked();
            ev->accept();
            return;
        }
        MFWidget::keyPressEvent(ev);
    }


    QSqlDatabase _db;

signals:

public slots:
    void okClicked()
    {
        bool b;
        //    MFMainWnd* main = MFMainWnd::This;
        unsigned char buf[32];
        memset(buf,0, sizeof(buf));
        if(this->_pass->text().length()>0)
        {
            MDString(this->_pass->text().toAscii().data(), buf);
        }

        QSqlQuery query;

        b = query.exec("select * from DOP where F1='" + this->_user->text().toAscii() + "'"); // and F2='" + QString((const char*)buf) + "'");
        if(!b)
        {
            QMessageBox::warning(this, "查询错误", "查询数据库错误，请检查数据库配置");
            return;
        }
        if(!query.next())
        {
            QMessageBox::warning(this, "错误", "用户名或者密码错误");
            return;
        }
        if(b && query.lastError().type() == QSqlError::NoError)
        {
            if(query.numRowsAffected() == 1)
            {
                if(query.value(2).toString().length() == 0 || query.value(2).toString() == (const char*)buf)
                {
                    this->_auth = query.value(4).toString();
                    mylog << query.value(0) << query.value(1) << query.value(2) << query.value(3) << query.value(4) << this->_auth;

                    MFLoadConf();
                    MFStartCardreaderDetect();

                    _login = true;
                    MFSetUser(this->_user->text());
                    MFSetAuth(this->_auth);
                    MFShowWindow(MENU_REPORT_FLOW);
                    return;
                }
            }
        }
        QMessageBox::warning(this, "错误", "用户名或者密码错误");
    }
};

class MFLogin1 : public MFWorkArea
{
    Q_OBJECT
public:
    explicit MFLogin1(QWidget *parent = 0):
        MFWorkArea(NULL, new MFLogin(), parent)
    {
        setTitle("登录");
        this->_commandBox->hide();
    }
};

class MFModifyPassword : public MFWidget
{
    Q_OBJECT
public:
    explicit MFModifyPassword(QWidget *parent = 0) :
        MFWidget(parent)
    {
        /*
        QLineEdit* _oldPass;
        QLineEdit* _newPass;
        QLineEdit* _newPass2;
        QLabel* _user;

        QPushButton* _ok;
        QPushButton* _cancel;
        QGroupBox* _box;
        */

        _vboxLayout = new QVBoxLayout();
        _hboxLayout = new QHBoxLayout();
        _gridLayout = new QGridLayout();

        _oldPass = new QLineEdit();
        _newPass = new QLineEdit();
        _newPass2 = new QLineEdit();
        _user = new QLabel(MFGetOper());
        _ok = new QPushButton("确认");
        //  _cancel = new QPushButton("cancel");

        connect(_ok, SIGNAL(clicked()), this, SLOT(OnOK()));

        QGridLayout* layout = new QGridLayout(this);
        layout->setColumnStretch(0, 1);
        layout->setColumnStretch(2, 1);
        layout->setRowStretch(0, 1);
        layout->setRowStretch(2, 2);
        layout->addLayout(_vboxLayout, 1, 1);

        _vboxLayout->addLayout(_gridLayout);
        _vboxLayout->addLayout(_hboxLayout);

        _gridLayout->addWidget(new QLabel("用户名："), 0, 0);
        _gridLayout->addWidget(new QLabel("旧密码："), 1, 0);
        _gridLayout->addWidget(new QLabel("新密码："), 2, 0);
        _gridLayout->addWidget(new QLabel("再输入一次新密码："), 3, 0);

        _gridLayout->addWidget(_user, 0, 1);
        _gridLayout->addWidget(_oldPass, 1, 1);
        _gridLayout->addWidget(_newPass, 2, 1);
        _gridLayout->addWidget(_newPass2, 3, 1);

        _oldPass->setEchoMode(QLineEdit::Password);
        _newPass->setEchoMode(QLineEdit::Password);
        _newPass2->setEchoMode(QLineEdit::Password);

        _hboxLayout->addStretch(1);
        _hboxLayout->addWidget(_ok);
        //_hboxLayout->addWidget(_cancel);
    }
    QLineEdit* _oldPass;
    QLineEdit* _newPass;
    QLineEdit* _newPass2;
    QLabel* _user;

    QPushButton* _ok;
    //  QPushButton* _cancel;
    QGroupBox* _box;
    QGridLayout* _gridLayout;
    QVBoxLayout* _vboxLayout;
    QHBoxLayout* _hboxLayout;

signals:

public slots:
    void OnOK()
    {
        if(_newPass->text() != _newPass2->text())
        {
            QMessageBox::warning(this, "错误", "新密码不一致");
        }
        else
        {
            unsigned char pass[32];
            memset(pass, 0, sizeof(pass));
            MDString((char*)this->_newPass->text().toAscii().data(), pass);
            QSqlQuery query;
            QString _sql= QString("update DOP set F2='") + QString::fromAscii((const char*)pass) + "' where F1='" + _user->text() + "'";
            if(!query.exec(_sql))
            {
                mylog << "aaa" << query.lastError().text();
                mylog << _sql;
            }
        }
    }

};

class MFModifyPassword1 : public MFWorkArea
{
    Q_OBJECT
public:
    explicit MFModifyPassword1(QWidget *parent = 0):
        MFWorkArea(NULL, new MFModifyPassword(), parent)
    {
        setTitle(MENU_SYS_MODIFY_PASSWORD);
        this->_commandBox->hide();
    }
};

class MFPOSOP : public MFTableViewContainer
{
    Q_OBJECT
public:
    explicit MFPOSOP(QWidget *parent = 0):
        MFTableViewContainer("DPOSOP", MD_DEVICE, parent)
    {
#define DPOSOPF0 "id"
#define DPOSOPF1 "姓名"
#define DPOSOPF2 "密码"
#define DPOSOPF3 "电话"
        setTitle(MENU_DEVICE_POSOP);

        reset();

    }

    void reset()
    {
        this->_table->setModel(NULL);
        this->_table->setModel(this->_table->_model);

        //  this->addFieldDict(DPOSOPF0, "F0", 0);
        this->addFieldDict(DPOSOPF1, "F1", 1);
        this->addFieldDict(DPOSOPF2, "F2", 2);
        this->addFieldDict(DPOSOPF3, "F3", 3);


        this->_table->hideColumn(0);
    }

signals:

public slots:

};

class MFSetupParam : public MFTableViewContainer
{
    Q_OBJECT
public:
    explicit MFSetupParam(QWidget *parent = 0):
        MFTableViewContainer("DCONF", MD_SYSTEM, parent)
    {
#define DCONFF0 "参数序号"
#define DCONFF1 "参数项目"
#define DCONFF2 "参数值"

        setTitle(MENU_SYS_SETUP_PARAM);

        reset();

        connect(this->_submit, SIGNAL(clicked()), this, SLOT(slotLoadConf()));
    }

    void reset()
    {
        this->_table->setModel(NULL);
        this->_table->setModel(this->_table->_model);
        this->addFieldDict(DCONFF0, "F0", 0);
        this->addFieldDict(DCONFF1, "F1", 1);
        this->addFieldDict(DCONFF2, "F2", 2);
        this->_delete->setDisabled(true);

        this->_table->setItemDelegateForColumn(0, readOnlyDelegate());
        this->_table->setItemDelegateForColumn(DCONF_KEY, new MFItemDelegateCannotModify());
        if(MFGetOper() != "系统管理员")
        {
            this->_table->setItemDelegate(readOnlyDelegate());
            this->_submit->setDisabled(true);
            this->_insert->setDisabled(true);
            this->_delete->setDisabled(true);
            this->_revert->setDisabled(true);
        }
    }

    void showEvent(QShowEvent *);

signals:

public slots:
    void slotLoadConf()
    {
        MFLoadConf();
    }

};

class MFDAcc : public MFTableViewContainer
{
    Q_OBJECT
public:
    explicit MFDAcc(QWidget *parent = 0) :
        MFTableViewContainer("DACC", MD_ACCOUNT, parent)
    {
#define DACCF0 "账户号"
#define DACCF1 "姓名"
#define DACCF2 "电话"
#define DACCF3 "身份证"
#define DACCF4 "病区"
#define DACCF5 "科室"
#define DACCF6 "床位"
#define DACCF7 "卡号"
#define DACCF8 "状态"
#define DACCF9 "余额"
#define DACCF10 "押金"
#define DACCF11 "结算方式"
#define DACCF12 "结算金额"
#define DACCF13 "拼音码"
#define DACCF14 "密码"
#define DACCF15 "帐户类型"
#define DACCF16 "月补助金额"
#define DACCF17 "领补助日期"
        setTitle(MENU_ACC_MAN);
        reset();
        this->_insert->setDisabled(true);
        this->_delete->setDisabled(true);
        this->_import->setDisabled(true);
    }

    void reset()
    {
        this->_table->setModel(NULL);
        this->_table->setModel(this->_table->_model);
        this->addFieldDict(DACCF0, "F0", 0);
        this->addFieldDict(DACCF1, "F1", 1);
        this->addFieldDict(DACCF2, "F2", 2);
        this->addFieldDict(DACCF3, "F3", 3);
        this->addFieldDict(DACCF4, "F4", 4);
        this->addFieldDict(DACCF5, "F5", 5);
        this->addFieldDict(DACCF6, "F6", 6);
        this->addFieldDict(DACCF7, "F7", 7);
        this->addFieldDict(DACCF8, "F8", 8);
        this->addFieldDict(DACCF9, "F9", 9);
        this->addFieldDict(DACCF10, "F10", 10);
        this->addFieldDict(DACCF11, "F11", 11);
        this->addFieldDict(DACCF12, "F12", 12);
        this->addFieldDict(DACCF13, "F13", 13);
        this->addFieldDict(DACCF14, "F14", 14);
        this->addFieldDict(DACCF15, "F15", 15);
        this->addFieldDict(DACCF16, "F16", 16);
        this->addFieldDict(DACCF17, "F17", 17);

        for(int i=0; i<=17; i++)
        {

            if(i==4)
                this->_table->setItemDelegateForColumn(i, new MFItemDelegateAutoEdit(MFConf()[DCONF_ZONE]));
            else if(i==5)
                this->_table->setItemDelegateForColumn(i, new MFItemDelegateAutoEdit(MFConf()[DCONF_DEPT]));
            else
                this->_table->setItemDelegateForColumn(i, readOnlyDelegate());
        }
    }

signals:

public slots:

};
class MFDOrder : public MFTableViewContainer
{
    Q_OBJECT
public:

    explicit MFDOrder(QWidget *parent = 0):
        MFTableViewContainer("DORDER", MD_REPORT, parent)
    {

        setTitle(MENU_REPORT_ORDER);
        reset();

        this->_import->setDisabled(true);

        this->_insert->setDisabled(true);
        this->_delete->setDisabled(true);
        this->_submit->setDisabled(true);
        this->_revert->setDisabled(true);

    }

    void reset()
    {
        this->_table->setModel(NULL);
        this->_table->setModel(this->_table->_model);

        this->addFieldDict(DFLOWF10, "F0", 0);
        this->addFieldDict(DFLOWF11, "F1", 1);
        this->addFieldDict(DFLOWF12, "F2", 2);
        this->addFieldDict(DFLOWF13, "F3", 3);
        this->addFieldDict(DFLOWF14, "F4", 4);
        this->addFieldDict(DFLOWF15, "F5", 5);
        this->addFieldDict(DFLOWF16, "F6", 6);
        this->addFieldDict(DFLOWF17, "F7", 7);
        this->addFieldDict(DFLOWF18, "F8", 8);
        this->addFieldDict(DFLOWF19, "F9", 9);
        this->addFieldDict(DFLOWF20, "F10", 10);

        this->_table->setItemDelegate(readOnlyDelegate());
    }
#if 0
    void showEvent(QShowEvent *ev)
    {
        revert();
        MFTableViewContainer::showEvent(ev);
    }
#endif

signals:

public slots:

};
#include "mfdaccinfo.h"
class MFLostCard : public MFWorkArea
{
    Q_OBJECT
public:
    explicit MFLostCard(QWidget *parent = 0):
        MFWorkArea(_lostCard = new MFDAccLost(), _widget = new QWidget(), parent)
    {
        setTitle(MENU_ACC_LOST);
        //this->_commandBox->hide();
        this->_lostCard->_commandBox->hide();
        _button = new QPushButton("确定挂失");
        this->_bottomLeftLayout->addWidget(_button);
        connect(_button, SIGNAL(clicked()), _lostCard, SLOT(handle()));
    }

    MFDAccLost* _lostCard;
    QWidget* _widget;
    QPushButton* _button;
};

class MFDBOM : public MFTableViewContainer
{
    Q_OBJECT
public:
    explicit MFDBOM(QWidget *parent = 0):
        MFTableViewContainer("DBOM", MD_DINNER, parent)
    {
#define DBOMF0 "序列号"
#define DBOMF1 "食品名称"
#define DBOMF2 "材料名称"
#define DBOMF3 "配置量(克)"
        setTitle("食谱材料配比设定");

        //   gdlSelectChange(QMap<int, int>());
        //   _did = 0;

        reset();
    }
    void reset()
    {
        this->_table->setModel(NULL);
        this->_table->setModel(this->_table->_model);
        this->addFieldDict(DBOMF0, "F0", 0);
        this->addFieldDict(DBOMF1, "F1", 1);
        this->addFieldDict(DBOMF2, "F2", 2);
        this->addFieldDict(DBOMF3, "F3", 3);

        //      this->_workFilter->setDisabled(true);

        this->_table->setItemDelegateForColumn(0, readOnlyDelegate());
        this->_table->setItemDelegateForColumn(2, new MFItemDelegateAutoEdit("DM",1, this));
        setItemDelegate();
    }
    // int _did;
    QString _dinnerName;
    void setItemDelegate()
    {
        MFSqlTableModel* GDL = MFGetModel("GDL", MFSqlTableModel::QUERY);
#if 0
        if(_did > 0)
        {
            GDL->setFilterAndSelect(QString("F0=%1").arg(_did));
            _table->setItemDelegateForColumn(1, new MFItemDelegateAutoEdit(QStringList() << (GDL->getData(0, 1).toString()), this));
        }
        else
        {
            GDL->setFilterAndSelect(QString());
            _table->setItemDelegateForColumn(1, new MFItemDelegateAutoEdit("GDL",1, this));
        }
#else
        GDL->setFilterAndSelect(QString());
        _table->setItemDelegateForColumn(DBOM_DINNER, new MFItemDelegateAutoEdit("GDL",GDL_NAME, this));
#endif
    }

signals:

public slots:

    void gdlSelectChange(QItemSelection selected, QItemSelection)
    {
        if(selected.count() == 0)
        {
            ////    if(_did == 0)
            //        return;
            //    _did = 0;
            _dinnerName.clear();
            //  this->_table->_model->setFilterAndSelect(this->_workFilter->_filter->getFilterString());
            this->_table->_model->setFilterAndSelect(QString());
        }
        else
        {

            //    this->_table->_model->revertAll();

            QModelIndex index = selected.at(0).topLeft();

            const QAbstractItemModel *model = index.model();
            QModelIndex idIndex = model->index(index.row(), GDL_NAME);
            _dinnerName = model->data(idIndex).toString();

            mylog << _dinnerName;

#if 0
            if(varId.toInt() == _did)
            {
                return;
            }

            _did = varId.toInt();
#endif


            //    this->_table->_model->setFilterAndSelect(QString("F0=%1").arg(_did));
            if(!this->_table->_model->setFilterAndSelect(QString("F%1='%2'").arg(DBOM_DINNER).arg(_dinnerName)))
            {
                mylog << "select error"  << QString("F%1=%2").arg(DBOM_DINNER).arg(_dinnerName);
            }

            if(_table->_model->rowCount() == 0)
            {
                mylog << "aaa";
                if(_table->_model->insertRecord(-1, QSqlRecord()))
                {
                    mylog << "aaa";
                    _table->_model->setData(_table->_model->index(0, DBOM_DINNER), _dinnerName);
                }
            }

        }
        //   setItemDelegate();
    }

};

class MFDOPMan : public MFTableViewContainer
{
    Q_OBJECT
public:
    explicit MFDOPMan(QWidget *parent = 0) :
        MFTableViewContainer("DOP", MD_SYSTEM, parent)
    {
#define DOPF0 "id"
#define DOPF1 "操作员名字"
#define DOPF2 "操作员密码"
#define DOPF3 "操作员电话号码"
#define DOPF4 "权限"


        setTitle(MENU_SYS_OP_MAN);
        reset();

    }
    void reset()
    {
        this->_table->setModel(NULL);
        this->_table->setModel(this->_table->_model);
        //  this->addFieldDict(DCONFF0, "F0", 0);
        this->addFieldDict(DOPF1, "F1", 1);
        //   this->addFieldDict(DOPF2, "F2", 2);
        this->addFieldDict(DOPF3, "F3", 3);
        this->addFieldDict(DOPF4, "F4", 4);
        this->_table->hideColumn(0);
        this->_table->hideColumn(2);

    }
signals:

public slots:

};

class MFDM : public MFTableViewContainer
{
    Q_OBJECT
public:
    explicit MFDM(QWidget *parent = 0):
        MFTableViewContainer("DM", MD_DINNER, parent)
    {
#define DMF1 "食料名称"
        setTitle(MENU_FOOD_METIAL);

        reset();

        connect(_table->_model, SIGNAL(beforeDelete(int)), this, SLOT(deleteRow(int)));
    }

    void reset()
    {
        this->_table->setModel(NULL);
        this->_table->setModel(this->_table->_model);
        this->addFieldDict(DMF1, "F1", 1);
        this->_table->hideColumn(0);
    }

signals:

public slots:
    void deleteRow(int idx)
    {
        QObject* obj = this->sender();


        if(obj->inherits("QSqlTableModel"))
        {

            mylog << "yes from table model";
        }
        else
        {
            mylog << "****************not from table model";
        }

        QSqlRecord record = _table->_model->record(idx);
        QModelIndex index = _table->_model->index(idx, 1);
        mylog << _table->_model->data(index, Qt::DisplayRole);
        mylog << record.value("F0");
        mylog << record.value("F1");
        mylog << "aaaa";
    }
};
class MFGDList : public MFTableViewContainer
{
    Q_OBJECT
public:
    explicit MFGDList(QWidget *parent = 0) :
        MFTableViewContainer("GDL", MD_DINNER, parent)
    {
#define GDLF0 "序列号"
#define GDLF1 "名称"
#define GDLF2 "单价"
#define GDLF3 "灶名"
#define GDLF4 "类别"
#define GDLF5 "拼音码"
        setTitle(MENU_FOOD_ALL);

        reset();
    }

    void reset()
    {
        this->_table->setModel(NULL);
        this->_table->setModel(this->_table->_model);

        this->addFieldDict(GDLF0, "F0", 0);
        this->addFieldDict(GDLF1, "F1", 1);
        this->addFieldDict(GDLF2, "F2", 2);
        this->addFieldDict(GDLF3, "F3", 3);
        this->addFieldDict(GDLF4, "F4", 4);
        this->addFieldDict(GDLF5, "F5", 5);
        //    this->addFieldDict(GDLF5, "F5", 5);

        //        this->_table->hideColumn(5);
        this->_table->setItemDelegateForColumn(0, readOnlyDelegate());
   //     this->_table->setItemDelegateForColumn(1, readOnlyDelegate());
        this->_table->setItemDelegateForColumn(1, new MFItemDelegateCannotModify());
#if 1
        this->_table->setItemDelegateForColumn(3, new MFItemDelegateAutoEdit(MFConf()[DCONF_DCLASS]));
        this->_table->setItemDelegateForColumn(4, new MFItemDelegateAutoEdit(MFConf()[DCONF_DTYPE]));
#endif
    }


signals:

public slots:

};

class MFGDL : public MFWidget
{
    Q_OBJECT
public:
    explicit MFGDL(QWidget *parent = 0):
        MFWidget(parent)
    {
        _splitter = new QSplitter(Qt::Horizontal, this);
        _gdlist = new MFGDList(_splitter);
        _bom = new MFDBOM(_splitter);
        _splitter->show();

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(_splitter);

        layout->setMargin(0);
        layout->setSpacing(0);

        MFSetWndExStyle(_gdlist, WS_EX_STATICEDGE);
        MFSetWndExStyle(_bom, WS_EX_STATICEDGE);

        connect(_gdlist->_table, SIGNAL(sigSelectionChanged(QItemSelection,QItemSelection)),
                _bom, SLOT(gdlSelectChange(QItemSelection,QItemSelection)));
    }

    MFGDList* _gdlist;
    MFDBOM* _bom;
    QSplitter* _splitter;

signals:

public slots:

};

class MFGDW : public MFTableViewContainer
{
    Q_OBJECT
public:
    explicit MFGDW(QWidget *parent = 0):
        MFTableViewContainer("GDW", MD_DINNER, parent)
    {
#define GDWF0 "序列号"
#define GDWF1 "周编号"
#define GDWF2 "日期"
#define GDWF3 "餐别"
#define GDWF4 "菜品"

        setTitle(MENU_FOOD_WEEK);

        reset();

        connect(_table, SIGNAL(sigRightClicked()), this, SLOT(slotRightClick()));

        MFGDWCopy* gdwClone = new MFGDWCopy(this);

        QPushButton* button = new QPushButton("复制周食谱");
        this->_bottomMiddleLayout->addWidget(button);
        connect(button, SIGNAL(clicked()), gdwClone, SLOT(exec()));

        //      initExtraButton();
    }


    void reset()
    {
        this->_table->setModel(NULL);
        this->_table->setModel(this->_table->_model);

        this->addFieldDict(GDWF0, "F0", 0, false);
        this->addFieldDict(GDWF1, "F1", 1);
        this->addFieldDict(GDWF2, "F2", 2);
        this->addFieldDict(GDWF3, "F3", 3);
        this->addFieldDict(GDWF4, "F4", 4, false);
        this->_table->setItemDelegateForColumn(4, new MFItemDelegateAutoEdit("GDL", 1, this));

        this->_insert->setDisabled(true);

    }

    QString _weekSelect;

#if 0
    void initExtraButton()
    {


        int index = 4;
        this->_bottomLeftLayout->insertSpacing(index++, 20);
        this->_bottomLeftLayout->insertWidget(index++, _buttonEdit = new QPushButton("编辑周食谱"));
        this->_bottomLeftLayout->insertWidget(index++, _buttonPrit = new QPushButton("打印周食谱"));
        this->_bottomLeftLayout->insertWidget(index++, _buttonDown = new QPushButton("下载周食谱"));

        connect(_buttonEdit, SIGNAL(clicked()), this, SLOT(extraButtonClick()));
        connect(_buttonPrit, SIGNAL(clicked()), this, SLOT(extraButtonClick()));
        connect(_buttonDown, SIGNAL(clicked()), this, SLOT(extraButtonClick()));

        _buttonEdit->setDisabled(true);
    }
#endif

protected:
    void clearFilter()
    {
        while(this->_workFilter->_filter->_filters.count() > 0)
        {
            this->_workFilter->_filter->deleteFilter();
        }
    }

    void OnCreate()
    {
        MFFilter::filter *f;

        MFTableViewContainer::OnCreate();
        f = this->_workFilter->_filter->_filters.at(0);
        f->_2->setCurrentIndex(0);

        addFilter();
        f = this->_workFilter->_filter->_filters.at(1);
        f->_2->setCurrentIndex(1);

        addFilter();
        f = this->_workFilter->_filter->_filters.at(2);
        f->_2->setCurrentIndex(2);

    }


#if 1
    bool canAddFilter()
    {
        return _workFilter->_filter->_filters.count() < 3;
    }
    #endif
    #if 0
    void newFilterAdded(MFFilter::filter *f)
    {
        int count = _workFilter->_filter->_filters.count();

        f->_3->setCurrentIndex(1);

        if(count == 1)
        {
            f->_2->setCurrentIndex(f->_2->findText(GDWF1));
            f->_4->clear();
            f->_4->setAutoList(MFConf()[DCONF_WF]);
            //    f->_4->addItems(MFConf()[DCONF_WF]);
        }
        if(count == 2)
        {
            f->_1->setDisabled(true);
            f->_2->setCurrentIndex(f->_2->findText(GDWF2));
            f->_4->clear();
            f->_4->setAutoList(MFConf()[DCONF_WD]);
            //    f->_4->addItems(MFConf()[DCONF_WD]);
        }
        if(count == 3)
        {
            f->_2->setCurrentIndex(f->_2->findText(GDWF3));
            f->_1->setDisabled(true);
            f->_4->clear();
            f->_4->setAutoList(MFConf()[DCONF_DC]);
            //   f->_4->addItems(MFConf()[DCONF_DC]);
        }

        f->_2->setDisabled(true);
        f->_3->setDisabled(true);

    }
#else
    void newFilterAdded(MFFilter::filter *f);
#endif

signals:

public slots:

    void gdlSelectChange(QItemSelection selected, QItemSelection);
#if 0
    void extraButtonClick()
    {
        QObject* sender = this->sender();

        if(sender == _buttonEdit)
        {
            _buttonEdit->setDisabled(true);
            _buttonPrit->setDisabled(false);
            _buttonDown->setDisabled(false);
            //   initFilter(3);
        }
        if(sender == _buttonPrit)
        {
            _buttonEdit->setDisabled(false);
            _buttonPrit->setDisabled(true);
            _buttonDown->setDisabled(false);
            //   initFilter(1);
        }
        if(sender == _buttonDown)
        {
            _buttonEdit->setDisabled(false);
            _buttonPrit->setDisabled(false);
            _buttonDown->setDisabled(true);
            //   initFilter(1);
        }
    }
#endif

    void slotRightClick()
    {
        QItemSelectionModel* selections = _table->selectionModel();
        QModelIndexList selectList = selections->selectedIndexes();
        if(selectList.count() == 0)
            return;

        QMap<QString, QString> map [selectList.at(0).model()->columnCount()];

        foreach(QModelIndex index, selectList)
        {
            int row = index.row();
            for(int col = 1; col < index.model()->columnCount(); col ++)
            {
                map[col].insert(index.model()->data(index.model()->index(row, col)).toString(), "");
            }
        }

        QMenu menu;
        QAction* action;
        for(int i=1; i</*selectList.at(0).model()->columnCount()*/ 2; i++)
        {
            foreach(QString str, map[i].keys())
            {
                if(i == 1)
                {
                    action = menu.addAction(QIcon(), QString("打印周食谱：%1").arg(str), this, SLOT(slotPrint()));
                    action->setData(str);
                    menu.addAction(QIcon(), QString("下载周食谱：%1").arg(str), this, SLOT(slotDownload()));
                    action->setData(str);
                }
            }
        }
        menu.exec(QCursor::pos());
    }

    void slotPrint()
    {}

    void slotDownload()
    {
        QAction* action = (QAction*)this->sender();
        _weekSelect = action->data().toString();
        MFShowWindow(MENU_DEVICE_DP);
    }
};
class MFGDWSetup : public MFWidget
{
    Q_OBJECT
public:
    explicit MFGDWSetup(QWidget *parent = 0) :
        MFWidget(parent)
    {
        _splitter = new QSplitter(Qt::Horizontal, this);
        _gdw = new MFGDW(_splitter);
        _gdlist = new MFGDList(_splitter);

        _gdlist->_commandBox->setDisabled(true);

        MFSetWndExStyle(_gdlist, WS_EX_STATICEDGE);
        MFSetWndExStyle(_gdw, WS_EX_STATICEDGE);
#if 0
        _report = new MFReportDP(_splitter);
        _download = new MFDeviceDP(_splitter);
        _report->hide();
        _download->hide();
#endif



        _splitter->show();

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(_splitter);

        layout->setSpacing(0);
        layout->setMargin(0);

        connect(_gdlist->_table, SIGNAL(sigSelectionChanged(QItemSelection,QItemSelection)),
                _gdw, SLOT(gdlSelectChange(QItemSelection,QItemSelection)));
#if 0
        connect(_gdw->_buttonDown, SIGNAL(clicked()), this, SLOT(extraButtonClicked()));
        connect(_gdw->_buttonEdit, SIGNAL(clicked()), this, SLOT(extraButtonClicked()));
        connect(_gdw->_buttonPrit, SIGNAL(clicked()), this, SLOT(extraButtonClicked()));

        connect(_gdw->_filter, SIGNAL(sigDataChange()), _report->_workArea, SLOT(updateContent()));
#endif
    }


    MFGDList* _gdlist;
    MFGDW* _gdw;
    QSplitter* _splitter;
#if 0
    MFReportDP* _report;
    MFDeviceDP* _download;

    void OnCreate()
    {
        setSizes();
    }
    void setSizes()
    {
        QList<int> sizes;
        sizes.append(_gdw->sizeHint().width());
        sizes.append(1000);
        sizes.append(1000);
        sizes.append(1000);

        _splitter->setSizes(sizes);
    }
#endif
signals:

public slots:
#if 0
    void extraButtonClicked()
    {
        QObject* sender = this->sender();
        if(sender == _gdw->_buttonDown)
        {
            _download->show();
            _report->hide();
            _gdlist->hide();
        }

        if(sender == _gdw->_buttonEdit)
        {
            _download->hide();
            _report->hide();
            _gdlist->show();
        }

        if(sender == _gdw->_buttonPrit)
        {
            _download->hide();
            _report->show();
            _gdlist->hide();
        }

        setSizes();
    }
#endif

};
class MFDevice : public MFTableViewContainer
{
    Q_OBJECT
public:
    explicit MFDevice(QWidget *parent = 0) :
        MFTableViewContainer("DD", MD_DEVICE, parent)
    {
#define DDF0 "id"
#define DDF1 "设备号"
#define DDF2 "设备类型"
#define DDF3 "备注"

        setTitle(MENU_DEVICE_MAN);


        reset();
        this->_insert->setDisabled(true);
        this->_delete->setDisabled(true);
        this->_import->setDisabled(true);
    }

    void reset()
    {
        this->_table->setModel(NULL);
        this->_table->setModel(this->_table->_model);

        this->addFieldDict(DDF1, "F1", 1);
        this->addFieldDict(DDF2, "F2", 2);
        this->addFieldDict(DDF3, "F3", 3);
        this->_table->setItemDelegateForColumn(DD_ID, readOnlyDelegate());
        this->_table->setItemDelegateForColumn(DD_TYPE, readOnlyDelegate());
        this->_table->hideColumn(0);
    }

signals:

public slots:

};
class MFDFlow : public MFTableViewContainer
{
    Q_OBJECT
public:
    explicit MFDFlow(QWidget *parent = 0):
        MFTableViewContainer("DFLOW", MD_REPORT, parent)
    {
#define DFLOWF0 "流水号"
#define DFLOWF1 "账户号"
#define DFLOWF2 "卡号"
#define DFLOWF3 "操作金额"
#define DFLOWF4 "操作方式"
#define DFLOWF5 "余额"
#define DFLOWF6 "操作员"
#define DFLOWF7 "操作日期"
#define DFLOWF8 "操作时间"
#define DFLOWF9 "订单号"
        setTitle(MENU_REPORT_FLOW);

        reset();

        this->_insert->setDisabled(true);
        this->_delete->setDisabled(true);
        this->_submit->setDisabled(true);
        this->_revert->setDisabled(true);
        this->_import->setDisabled(true);


        this->_bottomMiddleLayout->addWidget(_ticket = new QPushButton("补打小票"));
        connect(_ticket, SIGNAL(clicked()), this, SLOT(slotPrintTicket()));

        connect(this->_table, SIGNAL(sigSelectionChanged(QItemSelection,QItemSelection)),
                this, SLOT(SelectChange(QItemSelection,QItemSelection)));

        _ticket->setDisabled(true);
    }

    QPushButton *_ticket;

    void reset()
    {
        this->_table->setModel(NULL);
        this->_table->setModel(this->_table->_model);

        this->addFieldDict(DFLOWF0, "F0", 0);
        this->addFieldDict(DFLOWF1, "F1", 1);
        this->addFieldDict(DFLOWF2, "F2", 2);
        this->addFieldDict(DFLOWF3, "F3", 3);
        this->addFieldDict(DFLOWF4, "F4", 4);
        this->addFieldDict(DFLOWF5, "F5", 5);
        this->addFieldDict(DFLOWF6, "F6", 6);
        this->addFieldDict(DFLOWF7, "F7", 7);
        this->addFieldDict(DFLOWF8, "F8", 8);
        this->addFieldDict(DFLOWF9, "F9", 9);




        this->_table->setItemDelegate(readOnlyDelegate());


    }

    int _id;

signals:

public slots:
    void SelectChange(QItemSelection selected,QItemSelection)
    {
        if(selected.count() != 1)
        {
            _ticket->setDisabled(true);
            return;
        }

        _ticket->setDisabled(true);
        QModelIndex index = selected.at(0).topLeft();
        const QAbstractItemModel *gdlModel = index.model();
        QModelIndex idIndex = gdlModel->index(index.row(), DFLOW_SEQ);
        QModelIndex methodIndex = gdlModel->index(index.row(), DFLOW_METHOD);
        QVariant varId = gdlModel->data(idIndex);
        QVariant method = gdlModel->data(methodIndex);

        if(method.toString() == DCONF_OP_ACC_CHARGE
                || method.toString() == DCONF_OP_ACC_OPEN
                || method.toString() == DCONF_OP_ACC_CLOSE
                || method.toString() == DCONF_OP_ACC_LOST
                || method.toString() == DCONF_OP_ACC_CHANGE_LOST
                )
        {
            _id = varId.toInt();
            _ticket->setDisabled(false);
        }
    }

    void slotDoPrint(QPrinter* printer)
    {
        MFPrintFlowByID(printer, _id);
    }
    void slotPrintTicket()
    {
        QPrintDialog dlg;
        connect(&dlg, SIGNAL(accepted(QPrinter*)), this, SLOT(slotDoPrint(QPrinter*)));
        dlg.exec();
    }
};

class MFDeviceHandle : public MFWorkArea
{
    Q_OBJECT
public:
    explicit MFDeviceHandle(QWidget* setup, QWidget *parent) :
        MFWorkArea(setup, _log = new QTextEdit(), parent)
    {
        /*
        QComboBox* _com;
        QComboBox* _week;
        QTextEdit* _log;
        */

        _quit = false;

        _com = new MFAutoLineEdit();
        _com->setFit(false);
#ifdef WIN32
        _com->addItem("COM1");
        _com->addItem("COM2");
#else
        _com->addItem("/dev/ttyUSB0");
        _com->addItem("/dev/ttyUSB1");
        _com->setCurrentIndex(1);
#endif
        _com->setEditable(true);

        _log->setReadOnly(true);



        this->_bottomLeftLayout->addWidget(new QLabel("选择端口："));
        _bottomLeftLayout->addWidget(_com);

        _bottomRightLayout->addWidget(_start = new QPushButton("开始"));
        _bottomRightLayout->addWidget(_stop = new QPushButton("结束"));

        connect(_start, SIGNAL(clicked()), this, SLOT(start()));
        connect(_stop, SIGNAL(clicked()), this, SLOT(stop()));

        _stop->setDisabled(true);
    }

    QPushButton* _start;
    QPushButton* _stop;

    class MFSerialData
    {
    public:

    };

    QMap<QString, MFSerialData*> _map;

    void clean_map(QString id=QString())
    {
        QMap<QString, MFSerialData*>::iterator it;
        if(id.length() == 0)
        {
            while(_map.size())
            {
                it = _map.begin();
                delete it.value();
                _map.erase(it);
            }
            _map.clear();
        }
        else
        {
            it = _map.find(id);
            if(it != _map.end())
            {
                delete it.value();
                _map.erase(it);
            }
        }
    }

    MFAutoLineEdit* _com;
    QTextEdit* _log;

    bool _quit;

    virtual void prepare()=0;
    virtual void complete() = 0;

    bool deviceInitIfNew(QString& id)
    {
        MFSqlTableModel* dd = MFGetModel("DD", MFSqlTableModel::QUERY);
        dd->setFilterAndSelect(QString("F1='%1'").arg(id));
        mylog << id << "dd rowCount" << dd->rowCount();
        if(dd->rowCount() > 0)
        {
            appendLog("this is old device");
            return true;
        }


        /*  */
        if(!dd->insertRecordByValuesAndCommit(QList<QVariant>() << QVariant() << id << "pos" << QVariant()))
        {
            appendLog(QString("new device insert database error"));
            dd->revertAll();
            return false;
        }

        //  return sendConf(fd);
        return true;
    }

#ifdef WIN32

    bool setup_serial(HANDLE hSerial)
    {
        COMMTIMEOUTS TimeOuts;
        if(!SetupComm(hSerial, 2048, 2048))
            return false;

        //设定读超时
        TimeOuts.ReadIntervalTimeout = MAXDWORD;
        TimeOuts.ReadTotalTimeoutMultiplier = 0;
        TimeOuts.ReadTotalTimeoutConstant = 0;

        //设定写超时
        TimeOuts.WriteTotalTimeoutMultiplier = 100;
        TimeOuts.WriteTotalTimeoutConstant = 500;
        if(!SetCommTimeouts(hSerial, &TimeOuts))
            return false;//设置超时

        DCB dcb;
        GetCommState(hSerial, &dcb);
        dcb.BaudRate = 115200; //波特率为9600
        dcb.ByteSize = 8; //每个字节有8位
        dcb.Parity = NOPARITY; //无奇偶校验位
        dcb.StopBits = ONESTOPBIT; //两个停止位
        if(!SetCommState(hSerial, &dcb))
            return false;

        if(!PurgeComm(hSerial, PURGE_TXCLEAR|PURGE_RXCLEAR))
            return false;

        return true;
    }
    HANDLE open_serial()
    {
        HANDLE hSerial;

        hSerial = CreateFileA(_com->currentText().toAscii().data(),
                              GENERIC_READ|GENERIC_WRITE,
                              0,
                              NULL,
                              OPEN_EXISTING,
                              0,
                              NULL);
        if(hSerial == INVALID_HANDLE_VALUE)
        {
            mylog << "create serial file error";
            return INVALID_HANDLE_VALUE;
        }
        if(! setup_serial(hSerial) )
        {
            mylog << "configure serial error";
            CloseHandle(hSerial);
            return INVALID_HANDLE_VALUE;
        }

        mylog << "******************************open serial";
        return hSerial;
    }
    bool get_dev_id(HANDLE hSerial, QString& id)
    {
        xs_model_t* model = (xs_model_t*)xs_model_alloc(2);

        model->argv(0) = xs_strdup(__xs_pos);
        model->argv(1) = xs_strdup(__xs_getid);

        if(!serial_send_model(hSerial, model))
            return false;

        model = serial_recv_model(hSerial);
        if(!model)
        {
            return false;
        }
        if(xs_success(model->argv(0)))
        {
            id = model->argv(1);
            xs_model_delete(model);
            return true;
        }

        xs_model_delete(model);
        return false;
    }

    /* 处理串口，纯虚函数，给子类用 */
    virtual bool handle_dev(HANDLE hSerial)=0;
    bool serial_send_block(HANDLE hSerial, char* buf, int size)
    {
        bool ret = false;
        DWORD dwWrite;
        DWORD dwError;
        ClearCommError(hSerial,&dwError,NULL);
        PurgeComm(hSerial, PURGE_TXCLEAR|PURGE_RXCLEAR);
        while(size > 0)
        {
            ret = WriteFile(hSerial, buf, size, &dwWrite, NULL);
            if(!ret)
                break;
            size -= dwWrite;
        }

        return ret;
    }
    bool serial_recv_block(HANDLE hSerial, char* buf, int size)
    {
        bool ret = false;
        int cur = 0;
        DWORD dwRead;
        DWORD dwError;
        int count = 0;
        ClearCommError(hSerial,&dwError,NULL);
        //    PurgeComm(hSerial, PURGE_TXCLEAR|PURGE_RXCLEAR);
        while(size > 0)
        {
            ret = ReadFile(hSerial, buf+cur, size, &dwRead, NULL);
            if(!ret)
            {
                if(GetLastError()==ERROR_IO_PENDING)
                {
                    mylog << "error pending";
                    Sleep(10); continue;
                }
                break;
            }
            if(dwRead == 0)
            {
                if(count ++ == 100)
                    return false;
                qApp->processEvents();
                Sleep(100);
                continue;
            }
            cur += dwRead;
            size -= dwRead;

        }
        return ret;
    }
    bool serial_send_model(HANDLE hSerial, xs_model_t* model)
    {
        bool ret;
        int size;
        char* buf;
        buf = xs_model_to_buf_and_free_model(model, &size);
        ret = serial_send_block(hSerial, buf, size);
        xs_free(buf);
        return ret;
    }
    xs_model_t* serial_recv_model(HANDLE hSerial)
    {
        xs_model_t* model = NULL;
        int size;
        char* buf;

        if(!serial_recv_block(hSerial, (char*)&size, 4))
        {
            mylog << "recv header error";
            return model;
        }
        size = ntohl(size);

        if(size <= 0 || size >= 8192)
        {
            mylog << "recv header size is " << size << " " << htonl(size);
            return model;
        }

        buf = (char*)xs_malloc(size+1);

        buf[size] = 0;
        if(!serial_recv_block(hSerial, buf, size))
        {
            xs_free(buf);
            return model;
        }

        model = xs_model_from_buf(buf, size);
        xs_free(buf);

        return model;
    }
    int win_model_call(HANDLE hSerial, xs_model_t** r, QStringList vars)
    {
        int argc = vars.count();
        int i;
        xs_model_t* m = (xs_model_t*)xs_zalloc(sizeof(xs_model_t) + argc * sizeof(char*));
        m->argc = argc;
        for(i=0; i<argc; i++)
        {
            m->argv[i] = xs_strdup(MFConvertToGB2312(vars.at(i)).data());
        }

        if(!serial_send_model(hSerial, m))
        {
            xs_model_delete(m);
            return -1;
        }

        m = serial_recv_model(hSerial);
        if(m == NULL)
        {
            return -1;
        }

        *r = m;

        return 0;
    }






#else
    bool get_dev_id(int fd, QString& id)
    {
        xs_model_t* m;
        int ret = mf_model_call(fd, &m, QStringList() <<  __xs_pos<< __xs_getid);
        if(ret != 0)
        {
            appendLog("model call return not zero");
            return false;
        }

        if(xs_success(m->argv(0)))
        {
            appendLog(QString("id is %1").arg(m->argv(1)));
            id = m->argv(1);
            xs_model_delete(m);
            return true;
        }

        appendLog("err recv id");

        xs_model_delete(m);
        return false;
    }

    bool deviceSendConf(QString key, QString value, int fd)
    {
        xs_model_t* m;
        int ret;
        ret = mf_model_call(fd, &m, QStringList() << __xs_pos<< __xs_conf<< __xs_data<< key<< value);
        return isSuccess(ret, m);
    }

    bool sendConf(int fd)
    {
        xs_model_t* m;
        int ret = mf_model_call(fd, &m, QStringList() << __xs_pos<<__xs_conf<< __xs_start);
        if(!isSuccess(ret, m))
            return false;

        QMap<QString, QStringList> conf;
        conf = MFConf();
        foreach(QString key, conf.keys())
        {
            if(key == DCONF_BOOKDATE)
            {
                if(!deviceSendConf("delay", conf[key].at(0), fd))
                    return false;
            }

            else if(key == DCONF_COMPANY)
            {
                if(!deviceSendConf("company", conf[key].at(0), fd))
                    return false;
            }
        }

        ret = mf_model_call(fd, &m, QStringList() << __xs_pos<< __xs_conf<< __xs_end);
        return isSuccess(ret, m);
    }



    virtual bool handle_dev(int fd)=0;
#endif
    bool isSuccess(int ret, xs_model_t* m)
    {
        if(ret != 0)
        {
            appendLog(QString("error: ret=%1").arg(ret));
            return false;
        }

        if(!xs_success(m->argv(0)))
        {
            appendLog(QString("error, reason: %1").arg(m->argv(1)));
            xs_model_delete(m);
            return false;
        }

        xs_model_delete(m);
        return true;
    }

    void appendLog(QString str)
    {
        _log->append(str);
        qApp->processEvents();
    }

signals:

public slots:
#ifdef WIN32
    void start()
    {
        _start->setDisabled(true);
        _stop->setDisabled(false);

        int ok=1;
        MFSetPosTransferingFlag(true);
        _quit = false;

        /* 操作之前的准备 */
        prepare();

        /* 连接串口，并发送数据过去 */
        while(ok)
        {
            qApp->processEvents();
            if(_quit)
                break;

            Sleep(100);

            /* 打开串口 */

            HANDLE hSerial = open_serial();
            if(hSerial == INVALID_HANDLE_VALUE)
            {
                mylog << "open serial error";
                continue;
            }

            /* 从串口获取设备id号 */
            QString id;
            if(!get_dev_id(hSerial, id))
            {
                /* 如果获取串口失败，则继续循环 */
                CloseHandle(hSerial);
                appendLog(QString("无效设备，设备提供商"));

                MFSleep(2000);
                continue;
            }

            /* 如果设备已经被处理，则不需要再处理 */
            if(_map.find(id) != _map.end())
            {
                appendLog(QString("设备(%1)已经处理过，请连接其他设备").arg(id));

                MFSleep(2000);

                CloseHandle(hSerial);
                continue;
            }

            appendLog(QString("已经连接设备：%1").arg(id));

            /* 如果是新设备，则保存在数据库里 */
            deviceInitIfNew(id);

            /* 调用虚函数进行真正的处理 */
            if(handle_dev(hSerial))
            {
                _map.insert(id, new MFSerialData());
                appendLog(QString("设备(%1)已经处理完毕").arg(id));
            }
            else
            {
                MFSleep(2000);
                appendLog(QString("设备(%1)已经处理错误").arg(id));
            }

            /* 关闭串口，下一个设备来的时候会再次打开 */
            CloseHandle(hSerial);
        }

        clean_map();
        complete();

        MFSetPosTransferingFlag(false);
    }
#else

    int mf_model_call(int fd, xs_model_t** m, QStringList strList)
    {
        int ret;
        xs_model_t* model = (xs_model_t*)xs_model_alloc(strList.count());
        for(int i=0; i<model->argc; i++)
        {
            model->argv(i) = xs_strdup(MFConvertToGB2312(strList.at(i)).data());
        }

        ret = xs_model_send_block(model, fd, 60000);
        if(ret != 0)
            return ret;

        ret = xs_model_recv_block((void**)m, fd, 60000);
        return ret;
    }

    void start()
    {
        _start->setDisabled(true);
        _stop->setDisabled(false);
        int i = 1;
        MFSetPosTransferingFlag(true);
        _quit = false;
        while(i--)
        {
            qApp->processEvents();
            if(_quit)
                break;
            usleep(100*1000);
            int fd = open(this->_com->lineEdit()->text().toAscii().data(), O_RDWR);
            if(fd < 0)
                continue;

            if(0 != uart_set_proper(fd, 115200, 8, 1, 'N'))
            {
                ::close(fd);
                mylog << "set proper error";
                continue;
            }

            QString id;
            if(!get_dev_id(fd, id))
            {
                appendLog("invalid device, no device id");
                ::close(fd);
                continue;
            }

            appendLog(QString("get id %1").arg(id));

            /* already handle */
            if(_map.find(id) != _map.end())
            {
                appendLog(QString("设备(%1)已经操作过").arg(id));
                ::close(fd);
                continue;
            }

            deviceInitIfNew(id);
            appendLog("enter handler id=" + id);

            if(handle_dev(fd))
            {
                _map.insert(id, new MFSerialData());
            }

            ::close(fd);
        }
        clean_map();

        MFSetPosTransferingFlag(false);
    }
#endif

    void stop()
    {
        _start->setDisabled(false);
        _stop->setDisabled(true);
        _quit = true;
    }

};

class MFDeviceDP : public MFDeviceHandle
{
    Q_OBJECT
public:
    explicit MFDeviceDP(QWidget *parent = 0);

    QLabel* _label;
    MFAutoLineEdit* _gdwBox;
    MFSqlTableModel* _gdwModel;
    QGroupBox* _setup;

    QCheckBox* _downloadGDW;
    QCheckBox* _downloadConf;
    QCheckBox* _downloadBlack;
    QCheckBox* _downloadOper;

    void setupGroupBox();

    void prepare()
    {
    }
    void complete()
    {}

#if 1
    QString getSeq(QString str)
    {
        return QString::number(str.split("[").at(1).split("]").at(0).toInt());
    }
#endif

#ifdef WIN32
    bool downloadPOSOP(HANDLE hSerial);

    bool sendFood(HANDLE hSerial, int row);

    bool handle_dev(HANDLE hSerial);
    bool downloadFood(HANDLE hSerial);
    bool sendConf(HANDLE hSerial);
    bool downloadBlack(HANDLE hSerial);
    bool deviceSendConf(QString key, QString value, HANDLE hSerial);
#else
    bool downloadPOSOP(int fd)
    {
        MFSqlTableModel* posop =MFGetModel("DPOSOP", MFSqlTableModel::SHOW);
        int rowCount = posop->rowCount();
        int ret;
        xs_model_t* m;
        ret = mf_model_call(fd, &m,QStringList() << __xs_pos<< __xs_oper<< __xs_start);
        if(!isSuccess(ret, m))
            return false;

        for(int i=0; i<rowCount; i++)
        {
            ret = mf_model_call(fd, &m, QStringList() <<  __xs_pos<< __xs_oper<< __xs_data<<
                                posop->getData(i, 0).toString()<<
                                posop->getData(i, 1).toString()<<
                                posop->getData(i, 2).toString());
            if(!isSuccess(ret, m))
                return false;
        }

        ret = mf_model_call(fd, &m, QStringList() << __xs_pos<< __xs_oper<< __xs_end);
        return isSuccess(ret, m);
    }

    bool sendFood(int fd, int row)
    {
        xs_model_t* m;
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
        day = _gdwModel->getData(row, 2).toString();
        time = _gdwModel->getData(row, 3).toString();
        name = _gdwModel->getData(row, 4).toString();
        gdl->setFilterAndSelect(QString("F1='%1'").arg(name));
#if 0
        b = query.exec("create table GDL (F0 int(10) primary key auto_increment, "  /* id */
                       "F1 varchar(128) not null, "               /* dinner name*/
                       "F2 double(9,2) default 0.0, "               /* price */
                       "F3 varchar(32) not null, "                    /* DCLASS  */
                       "F4 varchar(32) not null, unique(F1))");                   /* DTYPE  */
#endif
        if(gdl->rowCount() != 1)
        {
            appendLog(QString("gdl error id=%1").arg(id));

            return false;
        }
        id = gdl->getData(0, 0).toString();
        price = gdl->getData(0, 2).toString();
        cls = gdl->getData(0, 3).toString();
        type = gdl->getData(0, 4).toString();

        mylog << "day and time is " << day << time;

        ret = mf_model_call(fd, &m, QStringList() << __xs_pos<< __xs_food<< __xs_data<<
                            id<<
                            name<<
                            type<<
                            cls<<
                            price<<
                            day<<
                            time);
        return isSuccess(ret, m);
    }

    bool handle_dev(int fd)
    {
        if(_gdwBox->currentText().length() == 0)
            return false;

        xs_model_t* m;
        int rowCount;
        int ret = mf_model_call(fd, &m, QStringList() << __xs_pos<< __xs_food<< __xs_start);
        if(!isSuccess(ret, m))
            return false;

        _gdwModel->setFilterAndSelect(QString("F1='%1'").arg(_gdwBox->currentText()));
        rowCount = _gdwModel->rowCount();
        for(int i=0; i<rowCount; i++)
        {
            if(!sendFood(fd, i))
                return false;
        }

        ret = mf_model_call(fd, &m, QStringList() << __xs_pos<< __xs_food<< __xs_end);
        if(!isSuccess(ret, m))
            return false;

        if(!downloadPOSOP(fd))
            return false;

        return sendConf(fd);
    }
#endif

signals:

public slots:
    void slotDownloadGDWCheckChange(int status)
    {
        if(status == Qt::Unchecked)
        {
            this->_gdwBox->setDisabled(true);
        }
        else
        {
            this->_gdwBox->setDisabled(false);
        }
    }
};
class MFDeviceData : public MFDeviceHandle
{
    Q_OBJECT
public:
    explicit MFDeviceData(QWidget *parent = 0) :
        MFDeviceHandle(NULL, parent)
    {
        setTitle(MENU_DEVICE_DATA);
    }

    void prepare()
    {}
    void complete()
    {}

    bool updateDACC_DFLOW_ORDER_table(double total, QString accid, QString orderid, QString cardno, QString zone, QString dept, QString bed)
    {
        MFSqlTableModel* modelAcc = MFGetModel("DACC", MFSqlTableModel::QUERY);
        MFSqlTableModel* modelOrder = MFGetModel("DORDER", MFSqlTableModel::QUERY);
        MFSqlTableModel* modelFlow = MFGetModel("DFLOW", MFSqlTableModel::QUERY);

        modelFlow->setFilterAndSelect(QString("F8='%1'").arg(orderid));
        /* already insert */
        if(modelFlow->rowCount() == 1)
        {
            return true;
        }

        /* acc do not exist */
        modelAcc->setFilterAndSelect(QString("F0=%1").arg(accid));
        if(modelAcc->rowCount() == 0)
        {
            modelOrder->setFilterAndSelect(QString("F1='%1'").arg(orderid));
            if(modelOrder->rowCount())
            {
                for(int i=0; i<modelOrder->rowCount(); i++)
                {
                    modelOrder->setData(modelOrder->index(i, 7), QString("账户不存在"));
                }
                modelOrder->submitAll();
            }
            return false;
        }

        /* 挂失补卡过 */
        if(modelAcc->getData(0, DACC_CARD).toString() != cardno)
        {
            modelOrder->setFilterAndSelect(QString("F1='%1'").arg(orderid));
            if(modelOrder->rowCount())
            {
                for(int i=0; i<modelOrder->rowCount(); i++)
                {
                    modelOrder->setData(modelOrder->index(i, 7), QString("对应卡号不存在，可能该账户今天挂失补卡过"));
                }
                modelOrder->submitAll();
            }
            return false;
        }

        /* acc exist but status error */
        if(modelAcc->getData(0, 8).toString() != DCONF_CS_GENERAL)
        {
            modelOrder->setFilterAndSelect(QString("F1='%1'").arg(orderid));
            if(modelOrder->rowCount())
            {
                for(int i=0; i<modelOrder->rowCount(); i++)
                {
                    modelOrder->setData(modelOrder->index(i, 7), QString("账户状态不正确：%1").arg(modelAcc->getData(0, 8).toString()));
                }
                modelOrder->submitAll();
            }
            return false;
        }

        QString ct = modelAcc->getData(0, DACC_SETTLE).toString();
        if(ct ==  DCONF_CT_DAILY && orderid.at(0) == QChar('o'))
        {
            // this is BOOK order and acc is DAILY
            total = modelAcc->getData(0, 12).toDouble();
        }

        QString opacc = DCONF_OP_ACC_BOOK;
        if(orderid.at(0) == QChar('u'))
        {
            opacc = DCONF_OP_ACC_BUY;
        }
        else if(ct ==  DCONF_CT_DAILY)
        {
            opacc = DCONF_OP_ACC_DAILY;
        }

        double balance = modelAcc->getData(0, DACC_BALANCE).toDouble();
//        QString cardno = modelAcc->getData(0, 7).toString();

        QSqlQuery query;
        if(QSqlDatabase::database().transaction()) // 启动事务操作
        {
            // UPDATE Person SET Address = 'Zhongshan 23', City = 'Nanjing' WHERE LastName = 'Wilson'
            // INSERT INTO table_name (列1, 列2,...) VALUES (值1, 值2,....)
            query.exec(QString("update DORDER set F6='%1', F7=null where F1='%2'").arg(MD_ORDER_STATUS_OK, orderid));
            query.exec(QString("update DACC set F9=%1, F4='%3', F5='%4',F6='%5' where F0=%2").arg(balance-total).arg(accid).arg(zone, dept, bed));
            query.exec(QString("insert into DFLOW (F1, F2, F3, F4, F5, F6, F7, F8, F9) values(%1,%2, %3, '%4', %5, '%6', '%7', '%8', '%9')").
                       arg(accid).arg(cardno).arg(total).arg(opacc).arg(balance-total).arg(MFGetOper()).arg(MFNowDate(), MFNowTime()).arg(orderid));
            //
            if(!QSqlDatabase::database().commit())
            {
                qDebug() << QSqlDatabase::database().lastError(); // 提交
                if(!QSqlDatabase::database().rollback())
                    qDebug() << QSqlDatabase::database().lastError(); // 回滚
                return false;
            }
            return true;
        }

        return false;
    }

#ifdef WIN32
    bool handle_dev(HANDLE hSerial)
    {
        hSerial = hSerial;
        MFSqlTableModel* sqlModel = MFGetModel("DORDER", MFSqlTableModel::QUERY);
        bool ret;
        md_book_node_t* node;
        double total=0.0;
        QString accid1;
        QString cardno;
        QString orderid1;
        QString cb;
        QString bed;
        QString zone;
        QString dept;

        md_req_t* req;

        /* first send start, and next, next, next ... */
        xs_model_t* rsp = NULL;
        if(0 != win_model_call(hSerial, &rsp, QStringList() << __xs_pos << __xs_book << __xs_start))
        {
            mylog << "start error";
            return false;
        }

        if(!xs_success(rsp->argv(0)))
        {
            xs_model_delete(rsp);
            mylog << "rsp error" << rsp->argv(1);
            return false;
        }

        int order_count = atoi(rsp->argv(1));
        xs_model_delete(rsp);

        mylog << "order count =" << order_count;

        /* 如果数量 > 0 */
        if(order_count > 0)
        {
            for(int i=0; i<order_count; i++)
            {
                for(int j=0; ;j++)
                {
                    if(win_model_call(hSerial, (xs_model_t**)&node, QStringList()
                                      << __xs_pos << __xs_book << __xs_data << xs_itoa(i) << xs_itoa(j)) != 0)
                    {
                        mylog << "error";
                        return false;
                    }

                    if(QString(node->book_type1) == __xs_err  || node->book_type1 == NULL)
                    {
                        mylog << "end break";
                        xs_model_delete((xs_model_t*)node);
                        break;
                    }

                    if(j==0)
                    {
                        accid1 = node->account;
                        orderid1 = node->orderid;
                        cardno = node->card;
                        total = 0;
                    }
                    total += QString(node->count).toInt() * QString(node->price).toDouble();

                    if(node->time)
                    {
                        /* if order Table already have data, do not insert */
                        if(!sqlModel->setFilterAndSelect(QString("%1 and %2").
                                                         arg(FIELDEQUAL(DORDER_ORDERID, node->orderid),
                                                             FIELDEQUAL(DORDER_DINNER, MFConvertFromGB2312(node->dinner)))))

                        {
                            mylog << "sql model query error";
                            exit(1);
                        }
                    }
                    else
                    {
                        cb = MFConf()[DCONF_DC].at(atoi(node->cb)-1);
                        /* if order Table already have data, do not insert */
                        if(!sqlModel->setFilterAndSelect(QString("%1 and %2 and %3").
                                                         arg(FIELDEQUAL(DORDER_ORDERID, node->orderid),
                                                             FIELDEQUAL(DORDER_DINNER, MFConvertFromGB2312(node->dinner)),
                                                             FIELDEQUAL(DORDER_CB, cb))))

                        {
                            mylog << "sql model query error";
                            exit(1);
                        }
                    }
                    zone = MFConvertFromGB2312(node->zone);
                    dept = MFConvertFromGB2312(node->dept);
                    bed = MFConvertFromGB2312(node->bed);

                    if(sqlModel->rowCount() == 0)
                    {
                        if(node->time)
                        {
                            // for buy
                            /* if record not in table, and insert error, stop trans data */
                            if(!sqlModel->insertRecordByValuesAndCommit(QList<QVariant>()
                                                                        << QString()
                                                                        << QString(node->orderid)
                                                                        << "即时消费"
                                                                        << QString(node->count)
                                                                        << QString(node->price)
                                                                        << MFConvertFromGB2312(node->oper)
                                                                        << QString(MD_ORDER_STATUS_ERR)
                                                                        << QString(MD_ORDER_STATUS_ERR_INIT)
                                                                        << QString(node->date)  /* date */
                                                                        << QString()            /* cb */
                                                                        << QString(node->time)))    /* need time */
                            {
                                mylog << "insert to sql error:" << sqlModel->lastError().text();
                                return false;
                            }
                        }
                        else
                        {
                            if(atoi(node->cb) == 0 || atoi(node->cb) > MFConf()[DCONF_DC].count())
                            {
                                mylog << node->cb;
                                return false;
                            }

                            /* if record not in table, and insert error, stop trans data */
                            if(!sqlModel->insertRecordByValuesAndCommit(QList<QVariant>() << QString()
                                                                        << QString(node->orderid)
                                                                        << MFConvertFromGB2312(node->dinner)
                                                                        << QString(node->count)
                                                                        << QString(node->price)
                                                                        << MFConvertFromGB2312(node->oper)
                                                                        << QString(MD_ORDER_STATUS_ERR)
                                                                        << QString(MD_ORDER_STATUS_ERR_INIT)
                                                                        << QString(node->date)
                                                                        << cb))
                            {
                                mylog << "insert to sql error";
                                return false;
                            }
                        }
                    }
                }

                updateDACC_DFLOW_ORDER_table(total, accid1, orderid1, cardno, zone, dept, bed);


                win_model_call(hSerial, (xs_model_t**)&rsp, QStringList() << __xs_pos << __xs_book << __xs_rm << xs_itoa(i));
                if(rsp) xs_model_delete(rsp);

                /* update acc and dflow and order table */
            }
        }

        req = xs_model(md_req_t);
        req->act = xs_strdup(__xs_book);
        req->pos = xs_strdup(__xs_pos);
        req->u.book.option = xs_strdup(__xs_end);
        ret = serial_send_model(hSerial, (xs_model_t* )req);

        rsp = this->serial_recv_model(hSerial);
        if(rsp) xs_model_delete(rsp);

        mylog << "here";

        MFGetModel("DACC", MFSqlTableModel::EDIT)->select();
        MFGetModel("DFLOW", MFSqlTableModel::EDIT)->select();
        MFGetModel("DORDER", MFSqlTableModel::EDIT)->select();

        return true;
    }


#else
    bool handle_dev(int fd)
    {
        fd = fd;
        return true;
    }
#endif

signals:

public slots:

};

class MFDeviceUpgrade : public MFDeviceHandle
{
    Q_OBJECT
public:
    explicit MFDeviceUpgrade(QWidget *parent = 0) :
        MFDeviceHandle(NULL, parent)
    {
        setTitle(MENU_DEVICE_UPGRADE);

        _fileName = new QLineEdit();
        _fileName->setMinimumWidth(300);
        _selectFile = new QPushButton("选择文件");

        connect(_selectFile, SIGNAL(clicked()), this, SLOT(selectFile()));



        _bottomLeftLayout->addWidget(new QLabel("传输文件："));
        _bottomLeftLayout->addWidget(_fileName);
        _bottomLeftLayout->addWidget(_selectFile);

        //   _bottomLeftLayout->addWidget(new QLabel("目标文件："));
        //   _bottomLeftLayout->addWidget(_target = new QComboBox());
        //  _target->addItem(__xs_gui);
        //  _target->addItem(__xs_daemon);
    }

    QLineEdit* _fileName;
    //   QComboBox* _target;
    QPushButton* _selectFile;

    void prepare()
    {}
    void complete()
    {}

#ifdef WIN32
    bool handle_dev(HANDLE hSerial);
#else
    bool handle_dev(int fd)
    {
        char tmp[1024];
        uint32_t header;
        xs_model_t* m;
        int filefd;
        int readlen;
        int ret = xs_model_call(fd, &m, 4, __xs_pos, __xs_trans, __xs_gui, xs_itoa_r(xs_file_length(_fileName->text().toAscii().data()), tmp));
        if(!isSuccess(ret, m))
            return false;

        _log->append(QString("trans command complete, now send data len=%1").arg(m->argv(1)));


        filefd = open(_fileName->text().toAscii().data(), O_RDWR);
        if(filefd < 0)
        {
            _log->append(QString("打开文件(%1)失败").arg(_fileName->text()));
            return false;
        }

        while(1)
        {
            qApp->processEvents();
            readlen = read(filefd, tmp, sizeof(tmp));
            if(readlen < 0)
            {
                ::close(fd);
                _log->append("读文件失败");
                return false;
            }

            /* set length in header 4 bytes */
            header = htonl(readlen);

            /* send header */
            ret = xs_sock_send_block(fd, (char*)&header, 4, 0);
            if(ret != 4)
            {
                _log->append("发送消息头失败");
                ::close(fd);
                return false;
            }

            /* send the buffer */
            ret = xs_sock_send_block(fd, tmp, readlen, 0);
            if(ret != readlen)  /* send error */
            {
                _log->append("发送文件内容失败");
                ::close(fd);
                return false;
            }

            /* recv rsp */
            ret = xs_model_recv_block((void**)&m, fd, 0);
            if(ret != 0)
            {
                _log->append("接收响应失败");
                ::close(fd);
                return false;
            }
            /* if not success */
            if(!xs_success(m->argv(0)))
            {
                ::close(fd);
                _log->append(QString("服务器返回失败，错误原因:%1").arg(m->argv(1)));
                xs_model_delete(m);
                return false;
            }

            /* if all data send */
            if(atoi(m->argv(1)) == 0)
            {
                _log->append("文件发送成功");
                xs_model_delete(m);
                break;
            }
            else
            {
                _log->append(QString("file remain %1").arg(m->argv(1)));
            }
            xs_model_delete(m);
        }

        ::close(filefd);
        return true;
    }
#endif
signals:

public slots:
    void selectFile()
    {
        QFileDialog dlg;
        if(dlg.exec() == QDialog::Accepted)
        {
            _fileName->setText(dlg.selectedFiles().at(0));
        }
    }
};

#endif // MFVIEWS_H
