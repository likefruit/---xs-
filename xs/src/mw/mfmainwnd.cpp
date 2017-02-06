#include "mw.h"
#include "mfmainwnd.h"
#include "mfsetupsqlconn.h"
#include "dlgs/mfdaccoption.h"
#include "dlgs/mfdaccinfo.h"
#include "opcard.h"
#include <QTcpServer>
#include "mwlib/diskid32.h"
#include "mfseedinfo.h"



MFMainWnd* MFMainWnd::This = NULL;

const QMap<QString, QStringList>& MFConf()
{
    return MFMainWnd::This->_conf;
}

MFMainWnd::MFMainWnd(QWidget *parent) :
    QMainWindow(parent)
{
    This = this;
    _view = new MFView(this);
    _btrans_data_with_pos = false;

    this->setWindowTitle("请登录"MW_APP_NAME);

    _query = NULL;
    _view->show();

    /* tool bar */
    _toolbar = new MFToolBar();
    QWidget* title = new MFToolbarTitle();

    QVBoxLayout* l = new QVBoxLayout(title);

    l->addWidget(new QLabel("功能菜单："));
    QDockWidget* dockWidget = new QDockWidget();
    dockWidget->setTitleBarWidget(title);
    dockWidget->setWidget(_toolbar);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea|Qt::LeftDockWidgetArea);



    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    _toolbar->resize(200, 100);
//    _toolbar->setBackgroundRole(QColor())
//    dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);

    MFSetWndExStyle(dockWidget, WS_EX_STATICEDGE);

    if(xs_file_exist("db.conf"))
    {
        qApp->postEvent(this, new QEvent(EV_LOGIN));
    }
    else
    {
        qApp->postEvent(this, new QEvent(EV_INIT));
    }
    initMenu();
    this->setCentralWidget(_view);

   initToolBar();
//    toolbar->addActions(_acts);

#if 0
   _logo = new QLabel(this);
   _logo->setPixmap(QPixmap("res/dongfang.jpg"));
   _logo->setWindowFlags(Qt::ToolTip);
   _logo->show();
   _logo->setScaledContents(true);
   _logo->resize(_logo->size()/3);
   _logo->setAttribute(Qt::WA_TranslucentBackground);
   _logo->setStyleSheet("background:transparent");
#endif
}


void MFMainWnd::loadConf()
{
    _conf.clear();
    MFSqlTableModel* model = MFGetModel("DCONF", MFSqlTableModel::SHOW);
    for(int i=0; i<model->rowCount(); i++)
    {
        _conf.insert(model->data(model->index(i, 1)).toString(),
                     model->data(model->index(i, 2)).toString().split("|"));
    }
   // mylog << _conf;
    setWindowTitle(_conf[DCONF_COMPANY].at(0) + "  "MW_APP_NAME);
}


void MFMainWnd::showWindow(QString name)
{
    /* 遍历所有 */
    QTreeWidgetItemIterator it(this->_toolbar->_view);
        while (*it) {
             //do something like
             if((*it)->text(0) == name)
             {
                 this->_toolbar->_view->setCurrentItem(*it, 0);
                 break;
             }
            ++it;
        }
    _view->showWorkArea(name);
}

bool MFMainWnd::canSupply(int accid)
{
    MFSqlTableModel* model = MFGetModel("DACC", MFSqlTableModel::QUERY);
    model->setFilterAndSelect(FIELDEQUAL(DACC_SEQ, accid));
    if(model->rowCount() == 0)
        return false;

    /* 如果补助为0，则不能补助 */
    if(model->getData(0, DACC_SUPPLY_AMOUNT).toDouble() < 0.01)
        return false;

    /* 如果补助日期为空，则可以补助 */
    QString date = model->getData(0, DACC_SUPPLY_DATE).toString();
    if(date.length() == 0)
        return true;

    /* 如果补助月份不是这个月，也可以补助 */
    QString today = MFNowMonth();
    if(date != today)
        return true;

    return false;
}

bool MFMainWnd::canCancelBook(int accid)
{
    /* 只能退今天，或者将来的餐 */
    accid = accid;
    return true;
}

void MFMainWnd::handleCard(unsigned char* buf, unsigned long cardno)
{
    QString status;
    int accid = MFGetAccID(cardno, status);
    if(accid > 0 && status == DCONF_CS_LOST)
    {
        MFDAccFind dlg(buf, cardno, this);
        dlg.exec();
        return;
    }
    int ret;
    MFDAccInfo* accinfo= NULL;
    MFDAccOption* option;
    if(accid <= 0)
    {
        option = new MFDAccOption(false);
    }
    else
    {
        option = new MFDAccOption(true);
        option->_check5->setEnabled(canSupply(accid));
        option->_check6->setEnabled(canCancelBook(accid));
    }
    ret = option->exec();

    if(ret != QDialog::Accepted)
    {
        return;
    }

    if(option->_check1->isChecked())
    {
        accinfo = new MFDAccOpen(buf, cardno);
    }
    if(option->_check2->isChecked())
    {
        accinfo = new MFDAccChange(buf, cardno);
    }
    if(option->_check3->isChecked())
    {
        accinfo = new MFDAccCharge(buf, cardno, accid);
    }
    if(option->_check4->isChecked())
    {
        accinfo = new MFDAccClose(buf, cardno, accid);
    }
    if(option->_check5->isChecked())
    {
        accinfo = new MFDAccSupply(buf, cardno, accid);
    }
    if(option->_check6->isChecked())
    {
        accinfo = new MFDAccCancel(buf, cardno, accid);
    }

    delete option;
    if(accinfo)
    {
        accinfo->exec();
        accinfo->deleteLater();
    }
}

void MFMainWnd::initMenu()
{

    QString menuContent[][20][3] =
    {
        {
            {MENU_SYS, "S", ""},
            {MENU_SYS_LOGIN, "", ""},
            {MENU_SYS_MODIFY_PASSWORD, "", ""},
            {MENU_SYS_SETUP_PARAM, "", ""},
            {MENU_SYS_OP_MAN, "O", ""},
            {MENU_SEP, "", ""},
            {MENU_SYS_EXIT, NULL, ""},
            {NULL, NULL, ""}
        },

        {
            {MENU_ACC, "Z", ""},
            {MENU_ACC_MAN, "", ""},
            {MENU_ACC_LOST, "S", "res/账户挂失.png"},

            {NULL, ""},
        },

        {
            {MENU_FOOD, "Z", ""},
            {MENU_FOOD_ALL, "E", "res/总食谱编辑.png"},
            {MENU_FOOD_WEEK, "P", "res/周食谱编辑.png"},
            {MENU_FOOD_METIAL, "", ""},
            {NULL, "", ""},
        },

        {
            {MENU_DEVICE, "H", ""},
            {MENU_DEVICE_MAN, "", ""},
            {MENU_DEVICE_POSOP, "U", ""},
            {MENU_DEVICE_DATA, "I", "res/订餐数据采集.png"},
            {MENU_DEVICE_DP, "B", "res/下载周食谱.png"},
            {MENU_DEVICE_UPGRADE, NULL, ""},

            {NULL, "", ""},

        },

        {
            {MENU_REPORT, "R", ""},
            {MENU_REPORT_FLOW, "R", ""},
            {MENU_REPORT_ORDER, "T", ""},
            {MENU_REPORT_GDW, "D", "res/打印周食谱.png"},
            {MENU_REPORT_ZUOFAN, "Z", "res/打印做饭表.png"},
            {MENU_REPORT_LINFAN, "L", ""},
            {MENU_REPORT_FAFAN, "F", "res/打印发饭表.png"},
            {MENU_REPORT_CAIGOU, "C", "res/打印领料表.png"},
            {MENU_REPORT_CAIWU, "A", "res/打印财务报表.png"},
            {MENU_REPORT_PERSONAL, "", ""},

            {NULL, "", ""},

        },

        {
            {MENU_ABOUT, "A", ""},
            {MENU_ABOUT_RIGHT, "H", ""},
            {NULL, "", ""},

        },

        {
            {NULL, NULL, ""}
        }
    };
    QTreeWidgetItem* item = NULL;
    QMenu* menu = NULL;
    QAction* act = NULL;

    int i,j;
    for(i=0;;i++)
    {
        for(j=0;;j++)
        {
            if(menuContent[i][j][0] == NULL)
                break;
            if(j==0)
            {
                /* add menu */
                item = _toolbar->addItem(menuContent[i][j][0]);
                if(menuContent[i][j][1].length() > 0)
                    menu = this->menuBar()->addMenu(menuContent[i][j][0] + "(&" + menuContent[i][j][1] + ")" );
                else
                    menu = this->menuBar()->addMenu(menuContent[i][j][0]);
            }
            else
            {
                if(menuContent[i][j][0] == MENU_SEP)
                {
                    menu->addSeparator();
                }
                else
                {
                    if(menuContent[i][j][1].length() > 0)

                        /* add item */
                        act = menu->addAction(QIcon(menuContent[i][j][2]),
                                          menuContent[i][j][0] + "(&" + menuContent[i][j][1] + ")",
                                          this, SLOT(triggleMenu()), QKeySequence("Ctrl+"+menuContent[i][j][1]));
                    else
                        act = menu->addAction(QIcon(menuContent[i][j][2]),
                                          menuContent[i][j][0],
                                          this, SLOT(triggleMenu()));

                    //_toolbar->addItem(menuContent[i][j][0], item, act);
                    //if(menuContent[i][j][1].length())
                    //_toolbar->addItem(menuContent[i][j][0] + "(" + menuContent[i][j][1] + ")", item, act);
                    //else
                        _toolbar->addItem(menuContent[i][j][0], item, act);
                    act->setData(menuContent[i][j][0]);
                    if(QString(menuContent[i][j][2]).length())
                        _acts << act;
                }
            }
        }
        if(j == 0)
            break;
    }

    this->statusBar()->addWidget(&_info, 1);
    this->statusBar()->addWidget(&_oper, 1);
    _info.setText(tr("    版权所有：东方易畅（北京）信息技术有限公司"));
}

void MFMainWnd::showEvent(QShowEvent* ev)
{
  //  this->setGeometry(0, 0, 800, 600);
    QMainWindow::showEvent(ev);
}

void MFMainWnd::triggleMenu()
{
    QAction* action = (QAction*)this->sender();
    if(isLogin() && !_btrans_data_with_pos)
    {
#if 0


        if(MENU_ACC_LOST == action->data().toString())
        {
            MFDAccLost accinfo;
            accinfo.exec();
            return;
        }
#endif
        /* 遍历所有 */
        QTreeWidgetItemIterator it(this->_toolbar->_view);
            while (*it) {
                 //do something like
                 if((*it)->text(0) == action->data().toString())
                 {
                     this->_toolbar->_view->setCurrentItem(*it, 0);
                     break;
                 }
                ++it;
            }
        _view->showWorkArea(action->data().toString());
    }
}

bool MFMainWnd::event(QEvent *event)
{
    if(event->type() == EV_INIT)
    {
        _view->showWorkArea(MENU_SYS_INIT);

    }
    if(event->type() == EV_LOGIN)
    {
        _view->showWorkArea(MENU_SYS_LOGIN);
    }

    return QMainWindow::event(event);
}

QMap<QString, MFSqlTableModel*> MFSqlTableModel::_models[MFSqlTableModel::ALL];
MFItemDelegateReadOnly* readOnlyDelegate()
{
    static MFItemDelegateReadOnly* ptr = NULL;
    if(!ptr)
        ptr = new MFItemDelegateReadOnly();
    return ptr;
}

void MFCreateModel(QString name, int role, QObject* parent)
{
    if(name == "DACC")
        new MFSqlTableModelDACC(name, role, parent);
    else if(name == "DBOM")
        new MFSqlTableModelDBOM(name, role, parent);
    else if(name == "DCONF")
        new MFSqlTableModelDCONF(name, role, parent);
    else if(name == "DD")
        new MFSqlTableModelDD(name, role, parent);
    else if(name == "DFLOW")
        new MFSqlTableModelDFLOW(name, role, parent);
    else if(name == "DLOG")
        new MFSqlTableModelDLOG(name, role, parent);
    else if (name == "DM")
        new MFSqlTableModelDM(name, role, parent);
    else if(name == "DOP")
        new MFSqlTableModelDOP(name, role, parent);
    else if(name == "DPOSOP")
        new MFSqlTableModelDPOSOP(name, role, parent);
    else if(name == "GDL")
        new MFSqlTableModelGDL(name, role, parent);
    else if (name == "GDW")
        new MFSqlTableModelGDW(name, role, parent);
    else if (name == "DORDER")
            new MFSqlTableModelDORDER(name, role, parent);
    else
        mylog << "no model for name=" << name;
}
MFMainWnd* wnd;
bool MFHaveAuth(QString auth)
{
    return wnd->haveAuth(auth);
}
QString MFGetOper()
{
    return wnd->_user;
}
QString MFGetCompany()
{
    return MFConf()[DCONF_COMPANY].at(0);
}
void MFSetPosTransferingFlag(bool flag)
{
    wnd->_btrans_data_with_pos = flag;
}

#ifdef WIN32
bool CheckLicense()
{
    /*
     *  if license file not exist
     *      show seed(and offer input command)
     *
     *  when clicked input
     *      show input dialog for user to input license, check and save
     *
     *
     *  if license file exist
     *      read and check, if license check error, delete license file, call me again
     *
     */
    unsigned char buf[32];
    char seed[33];
    MDString((char*)getDiskInfo().serialNumber, buf);
    xs_hex2str(buf, 16, seed);
    MFSeedInfo s(seed);

    if(!xs_file_exist(MW_LICENSE))
    {
        MFLog("license not exist");
        if(s.exec() != QDialog::Accepted)
        {
            MFLog("not accept");
            return false;
        }
    }
    else
    {
        if(!s.check_license(seed, MW_LICENSE))
        {
            QFile::remove(MW_LICENSE);
            return CheckLicense();
        }
    }

    return true;
}
#endif


int main(int argc, char *argv[])
{
    int ret;
    QApplication a(argc, argv);
    xs_init();

    /* only allow one instance */
    QTcpServer server;
#if 0
    QTextCodec *codec = QTextCodec::codecForName("UTF8");

    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
    if(!server.listen(QHostAddress::Any, 19898))
    {
        QMessageBox::warning(NULL, "错误", QString("一台电脑只能开一个[%1]").arg(MW_APP_NAME));
        exit(1);
    }

#ifdef WIN32
    if(!CheckLicense())
    {
        exit(1);
    }
#endif




    wnd = new MFMainWnd();


    wnd->setWindowIcon(QIcon("res/mw.ico"));


    /* 启动卡检测进程 */
    MFCardDetect cardDetect(wnd);
    cardDetect.start();

#if 0
    Toloyi toloyi;
    toloyi.start();
#endif

    wnd->_cardDetect = &cardDetect;

    wnd->connect(wnd, SIGNAL(destroyed()), &cardDetect, SLOT(setQuit()));
    wnd->connect(&cardDetect, SIGNAL(dataArrive(unsigned char*, unsigned long)),
                 wnd, SLOT(cardDataArrive(unsigned char*, unsigned long)),
                 Qt::QueuedConnection);

    wnd->showMaximized();



    //pthread_t pid;
    //pthread_create(&pid, NULL, toloyi, NULL);

    ret = a.exec();
    xs_fini();

    return ret;
}

void MFStartDetectCard()
{
    wnd->_cardDetect->_stop = false;
}
void MFStopDetectCard()
{
    wnd->_cardDetect->_stop = true;
}

void MFLoadConf()
{
    wnd->loadConf();
}

void MFStartCardreaderDetect()
{
    wnd->_button_stop_or_start_card_detect->setEnabled(true);
    wnd->_cardDetect->setStop(false);
}

void MFSetUser(QString str)
{
    wnd->_oper.setText(QString("操作员：%1").arg(str));
    wnd->_user = str;
}
void MFSetAuth(QString str)
{
    wnd->_auth = str;
}

void MFShowWindow(QString str)
{
    wnd->showWindow(str);
}

QWidget* MFGetWindow(QString str)
{
     return wnd->_view->findWorkArea(str);
}

int MFGetAccID(unsigned long cardno, QString& status, MFSqlTableModel* model)
{
    if(model == NULL)
        model = MFGetModel("DACC", MFSqlTableModel::QUERY);
    model->setFilter(QString("F7=%1 and (F8='%2' or F8='%3')").arg(cardno).arg(DCONF_CS_GENERAL, DCONF_CS_LOST));
    model->select();
    if(model->rowCount() == 1)
    {
        status = model->getData(0, DACC_STATUS).toString();
        return model->getData(0, 0).toInt();
    }
    return -1;
}

void MFPrintFlowByID(QPrinter* print, int flowid)
{
    MFSqlTableModel* model = MFGetModel("DFLOW", MFSqlTableModel::QUERY);
    model->setFilterAndSelect(QString("F0=%1").arg(flowid));
    if(model->rowCount()!=1)
    {
        mylog << "error select" << flowid;
        return;
    }

    MFSqlTableModel* acc = MFGetModel("DACC", MFSqlTableModel::QUERY);
    acc->setFilterAndSelect(QString("F0=%1").arg(model->getData(0, 1).toInt()));
    if(acc->rowCount() != 1)
    {
        mylog << "error select " << model->getData(0, 1).toInt();
        return;
    }

/*
../mw/mfmainwnd.cpp 375 0 0 0 0
../mw/mfmainwnd.cpp 381 QRect(0,0 272x1123)
*/
    int row = 10;

    QPainter p(print);

    p.drawText(0, row, MFConf()[DCONF_COMPANY].at(0) + " Tel:" +  MFConf()[DCONF_COMPANY_TEL].at(0)),       row += 20;
 //   p.drawText(0, row, MFGetOper() + " " +  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")),  row += 20;
    p.drawText(0, row, QString("%1 %2 %3").arg(MFGetOper(), model->getData(0, DFLOW_DATE).toString(),
                                               model->getData(0, DFLOW_TIME).toString())); row+=20;
    p.drawText(0, row, "-----------------------------------------------------------------------------"),     row += 20;
    p.drawText(20, row, "客户名称：" + acc->getData(0, 1).toString()), row += 20;
    p.drawText(20, row, "客户编号：" + acc->getData(0, 0).toString()), row += 20;
    p.drawText(20, row, "业务名称：" + model->getData(0, 4).toString()), row += 20;
    p.drawText(20, row, "业务金额：" + model->getData(0, 3).toString()), row += 20;
    p.drawText(20, row, "账户余额：" + model->getData(0, 5).toString()), row += 20;
    if(model->getData(0, 4).toString() ==  DCONF_OP_ACC_OPEN)
        p.drawText(20, row, "押金：" + acc->getData(0, 10).toString()), row += 20;

    p.drawText(0, row, "-----------------------------------------------------------------------------"),     row += 20;
    p.drawText(0, row, "此小票为业务凭证，请妥善保管\n\n"),     row += 20;
}

QByteArray MFConvertToGB2312(QString str)
{
#if 1
    static QTextCodec* codec = QTextCodec::codecForName("GB2312");
    if(codec == NULL)
    {
        MFLog("no coding of GB2312");
        exit(1);
    }
    return codec->fromUnicode(str);
#else
    return str.toUtf8().data();
#endif
}

QString MFConvertFromGB2312(const char* gb2312)
{
#if 1
    static QTextCodec* codec = QTextCodec::codecForName("GB2312");
    return codec->toUnicode(gb2312);
#else
    return QString(gb2312);
#endif
}

void MFSleep(int msec)
{
    for(int i=0; i<msec; i+=100)
    {
#ifdef WIN32
        Sleep(100);
#else
        usleep(100*1000);
#endif
        qApp->processEvents();
    }
}



MFSqlTableModel* MFGetModel(QString name, int role, QObject* parent)
{
    return MFSqlTableModel::getModel(name, role, parent);
}
