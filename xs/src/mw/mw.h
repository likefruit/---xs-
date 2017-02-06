#ifndef MW_H
#define MW_H

#include <QEvent>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlError>
#include <QObject>
#include <QWidget>
#include <QThread>
#include <QEvent>
#include <QDialog>
#include <QLineEdit>
#include <QMainWindow>
#include <QPicture>
#include <QImage>
#include <QBitmap>
#include <QLabel>
#include <QMenu>
#include <QTextCodec>
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QPainter>
#include <QPair>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlField>
#include <QMenu>
#include <QDebug>
#include <QTextEdit>
#include <QTextBrowser>
#include <QDockWidget>
#include <QGroupBox>
#include <QSqlRecord>
#include <QComboBox>
#include <QSplitter>
#include <QMap>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QPrinter>
#include <QCompleter>
#include <QListView>
#include <QStringListModel>
#include <QFileDialog>

#define toAscii toUtf8
#define fromAscii fromUtf8

#define GDL_SEQ 0
#define GDL_NAME 1
#define GDL_PRICE 2
#define GDL_CLASS 3
#define GDL_TYPE 4
#define GDL_PINYIN 5

#define GDW_SEQ 0
#define GDW_WEEK 1
#define GDW_DAY 2
#define GDW_TIME 3
#define GDW_FOOD 4

#define DM_SEQ 0
#define DM_NAME 1

#define DBOM_SEQ 0
#define DBOM_DINNER 1
#define DBOM_METERIAL 2
#define DBOM_WEIGHT 3

#define DCONF_SEQ 0
#define DCONF_KEY 1
#define DCONF_VALUE 2

#define DLOG_SEQ 0
#define DLOG_TIMESTAMP 1
#define DLOG_LOG 2

#define DOP_SEQ 0
#define DOP_NAME 1
#define DOP_PASS 2
#define DOP_TEL 3
#define DOP_AUTH 4

#define DACC_SEQ 0
#define DACC_NAME 1
#define DACC_TEL 2
#define DACC_ID 3
#define DACC_AREA 4
#define DACC_DEPT 5
#define DACC_BED 6
#define DACC_CARD 7
#define DACC_STATUS 8
#define DACC_BALANCE 9
#define DACC_DEPOSIT 10
#define DACC_SETTLE 11
#define DACC_AMOUNT 12
#define DACC_PINYIN 13
#define DACC_PASS 14
#define DACC_TYPE 15
#define DACC_SUPPLY_AMOUNT 16
#define DACC_SUPPLY_DATE 17

#define DD_SEQ 0
#define DD_ID 1
#define DD_TYPE 2
#define DD_MEM 3

#define DPOSOP_SEQ 0
#define DPOSOP_NAME 1
#define DPOSOP_PASS 2
#define DPOSOP_TEL 3

#define DFLOW_SEQ 0
#define DFLOW_ACC_SEQ 1
#define DFLOW_CARDNO 2
#define DFLOW_AMOUNT 3
#define DFLOW_METHOD 4
#define DFLOW_BALANCE 5
#define DFLOW_OPER 6
#define DFLOW_DATE 7
#define DFLOW_TIME 8
#define DFLOW_ORDERID 9

#define DORDER_SEQ 0
#define DORDER_ORDERID 1
#define DORDER_DINNER 2
#define DORDER_COUNT 3
#define DORDER_PRICE    4
#define DORDER_POSOP    5
#define DORDER_STATUS   6
#define DORDER_ERROR_INFO   7
#define DORDER_DAY  8
#define DORDER_CB 9
#define DORDER_TIME 10

#define FIELD(_index) (QString("F%1").arg(_index))
#define FIELDEQUAL(_index, value) (QString("F%1='%2'").arg(_index).arg(value))
#define FIELDNOT(_index, value) (QString("F%1<>'%2'").arg(_index).arg(value))
#define FIELDGREAT(_index, value) (QString("F%1>'%2'").arg(_index).arg(value))
#define FIELDLESS(_index, value) (QString("F%1<'%2'").arg(_index).arg(value))
#define FIELDGREATEEQUAL(_index, value) (QString("F%1>='%2'").arg(_index).arg(value))
#define FIELDLESSEQUAL(_index, value) (QString("F%1<='%2'").arg(_index).arg(value))
#define FIELDLIKE(_index, value) (QString("F%1 like '%%2%'").arg(_index).arg(value))

#define DFLOWF10 "流水号"
#define DFLOWF11 "订单号"
#define DFLOWF12 "菜品"
#define DFLOWF13 "数量"
#define DFLOWF14 "单价"
#define DFLOWF15 "订餐员"
#define DFLOWF16 "订单状态"
#define DFLOWF17 "错误状态"
#define DFLOWF18 "就餐日期"
#define DFLOWF19 "餐别"
#define DFLOWF20 "就餐时间"

#define MW_ALL "不限"
#define MW_LICENSE "license"

#ifdef WIN32
#include <windows.h>
#include <dbt.h>//
#include <winioctl.h>//IOCTL_STORAGE_EJECT_MEDIA未定义的问题
#endif
#include "xs.h"
#include "opcard.h"

#define mylog qDebug() << __FILE__ << __LINE__

#define EV_CREATE         (QEvent::Type)(QEvent::User+1)
#define EV_LOGIN        (QEvent::Type)(QEvent::User+2)
#define EV_INIT         (QEvent::Type)(QEvent::User+3)

#define MENU_SYS_INIT "init"
#define MW_APP_NAME "预约点餐管理系统"

#define MENU_SYS "系统"
#define MENU_SYS_LOGIN "登陆"
#define MENU_SYS_DATA_BACKUP "数据备份"
#define MENU_SYS_DATA_RECOVERY "数据回复"
#define MENU_SYS_MODIFY_PASSWORD "修改密码"
#define MENU_SYS_SETUP_PARAM "设置参数"
#define MENU_SYS_OP_MAN "操作员管理"
#define MENU_SYS_EXIT "退出"

#define MENU_ACC "账户"
#define MENU_ACC_MAN "账户列表"
#define MENU_ACC_LOST "账户挂失"
//#define MENU_ACC2_CLOSE "销户"
//#define MENU_ACC3_LOST "挂失"
//#define MENU_ACC4_MODIFY "修改账户"

#define MENU_FOOD "食谱"
#define MENU_FOOD_ALL "总食谱"
#define MENU_FOOD_WEEK "周食谱"
#define MENU_FOOD_METIAL "食材管理"
//#define MENU_FOOD_DC "饮食计划"

#define MENU_DEVICE "设备"
#define MENU_DEVICE_MAN "设备管理"
#define MENU_DEVICE_POSOP "订餐员管理"
#define MENU_DEVICE_DATA "数据采集"
#define MENU_DEVICE_DP "下载参数"
#define MENU_DEVICE_UPGRADE "升级程序"

#define MENU_REPORT "报表"
#define MENU_REPORT_FLOW "账户流水"
#define MENU_REPORT_ORDER "订单流水"
#define MENU_REPORT_GDW "打印周食谱"
#define MENU_REPORT_ZUOFAN "打印做饭表"
#define MENU_REPORT_LINFAN "打印领饭表"
#define MENU_REPORT_FAFAN "打印发饭表"
#define MENU_REPORT_CAIGOU "打印领料表"
#define MENU_REPORT_CAIWU   "财务收支表"
#define MENU_REPORT_PERSONAL   "个人业务流水"

#define MENU_ABOUT "关于"
#define MENU_ABOUT_RIGHT "关于本软件"

#define MENU_SEP "SEP"

#define MW_FILTER_TITLE "设置过滤条件"
#define MW_REPORT_STYLE_NORMAL "标准格式"
#define MW_REPORT_STYLE_COMPACT "紧凑格式"

#ifdef WIN32
#define MFSetWndExStyle(_p, _s) SetWindowLong((_p)->winId(), GWL_EXSTYLE, \
              GetWindowLong((_p)->winId(), GWL_EXSTYLE) | _s)
#else
#define MFSetWndExStyle(a, b)
#endif

//#define MFSetWndStyle(_p, _s) SetWindowLong((_p)->winId(), GWL_STYLE, GetWindowLong((_p)->winId(), GWL_STYLE) | _s)

const QMap<QString, QStringList>& MFConf();
void MFLoadConf();
bool MFHaveAuth(QString auth);
QString MFGetOper();
QString MFGetCompany();
void MFSetPosTransferingFlag(bool flag);
void MFStartCardreaderDetect();
void MFSetUser(QString str);
void MFSetAuth(QString str);
void MFShowWindow(QString str);
QWidget* MFGetWindow(QString str);
void MFStartDetectCard();
void MFStopDetectCard();

QByteArray MFConvertToGB2312(QString str);
static inline void MFLogOutput(QString str)
{
    QFile log("mw.log");
    log.open(QFile::WriteOnly|QFile::Append);
  //  log.write(MFConvertToGB2312(str));
    log.write(str.toUtf8());
    log.close();
}
#define MFLog(_str) MFLogOutput(QString("%1 %2: %3\r\n").arg(QString(__FILE__), QString::number(__LINE__), QString(_str)))


QString MFConvertFromGB2312(const char* gb2312);
#if 0
int write_card_data1(unsigned long cardno, unsigned char* buf, char* balance, char* name, char* zone,
                     char* dept, char* num, char* id, char* orderid);
#endif
static inline int MFWriteCard(unsigned long cardno, unsigned char* buf, QString balance,
                              QString name, QString zone, QString dept,
                              QString num, QString id, QString orderid, QString daily)
{


    int ret =  write_card_data(cardno, buf,
               balance.length()? MFConvertToGB2312(balance).data():NULL,
               name.length()?      MFConvertToGB2312(name).data():NULL,
                     zone.length()?MFConvertToGB2312(zone).data():NULL,
                     dept.length()?MFConvertToGB2312(dept).data():NULL,
                     num.length() ? MFConvertToGB2312(num).data():NULL,
                     id.length()?MFConvertToGB2312(id).data():NULL,
                            orderid.length()?MFConvertToGB2312(orderid).data():NULL,
                           daily.length()?MFConvertToGB2312(daily).data():NULL);


    return ret;

}
static inline void MFParseCard(unsigned char* buf, QString& balance,
                               QString& name, QString& zone, QString& dept,
                               QString& num, QString& id, QString& orderid, QString& daily)
{
    char balance1[48];
    char name1[48];
    char zone1[48];
    char dept1[48];
    char num1[48];
    char id1[48];
    char orderid1[48];
    char daily1[24];
    read_card_data(buf, balance1, name1, zone1, dept1, num1, id1, orderid1, daily1);

    balance = MFConvertFromGB2312(balance1);
    name = MFConvertFromGB2312(name1);
    zone = MFConvertFromGB2312(zone1);
    dept = MFConvertFromGB2312(dept1);
    num = MFConvertFromGB2312(num1);
    id = MFConvertFromGB2312(id1);
    orderid = MFConvertFromGB2312(orderid1);
    daily = MFConvertFromGB2312(daily1);
    mylog << daily;
}

static inline QString MFGenOrder(QString accid, QString method)
{
    return QString("c")+QDateTime::currentDateTime().toString("yyyyMMddhhmmss")+"."+accid+"." +method;
}

static inline QString MFGenOrder(int accid, QString method)
{
    return MFGenOrder(QString::number(accid), method);
}

#if 1
static inline QString MFNowDateTime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}
#endif

static inline QString MFNowDate()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd");
}

static inline QString MFNowMonth()
{
    return QDateTime::currentDateTime().toString("yyyy-MM");
}

static inline QString MFNowTime()
{
    return QDateTime::currentDateTime().toString("hh:mm:ss");
}

void MFPrintFlowByID(QPrinter* print, int flowid);

void MFSleep(int msec);

#define MD_ORDER_STATUS_OK "正常"
#define MD_ORDER_STATUS_ERR "异常"
#define MD_ORDER_STATUS_CANCEL "取消"

#define MD_ORDER_STATUS_ERR_INIT "未处理完毕"

#define DCONF_BOOKDATE "定餐滞后日期"
#define DCONF_COMPANY "单位名称"
#define DCONF_COMPANY_TEL "单位电话"
#define DCONF_DTYPE "类型"
#define DCONF_DACC_TYPE "账户类型"
#define DCONF_DCLASS "灶类"
#define DCONF_WF "周食谱定义"
#define DCONF_WD "周食谱日期定义"
#define DCONF_DC "餐别定义"
#define DCONF_CS "卡状态"
    #define DCONF_CS_GENERAL "正常"
    #define DCONF_CS_LOST "挂失"
    #define DCONF_CS_END "已退卡"
    #define DCONF_CS_INIT "未启用"
    #define DCONF_CS_ALL (DCONF_CS_GENERAL"|"DCONF_CS_LOST"|" DCONF_CS_END"|" DCONF_CS_INIT)
#define DCONF_CT "结算方式"
    #define DCONF_CT_NOW "立即结算"
    #define DCONF_CT_DAILY "日结"
    #define DCONF_CT_ALL (DCONF_CT_NOW "|" DCONF_CT_DAILY)

#define DCONF_OP_ACC "账户操作"
    #define DCONF_OP_ACC_BOOK "订餐"
    #define DCONF_OP_ACC_CANCELBOOK "退餐"
    #define DCONF_OP_ACC_CHARGE "充值"
    #define DCONF_OP_ACC_OPEN "开卡"
    #define DCONF_OP_ACC_CLOSE "退卡"
    #define DCONF_OP_ACC_LOST "挂失"
    #define DCONF_OP_ACC_CHANGE_LOST "挂失补卡"
    #define DCONF_OP_ACC_LOST_RECOVERY "挂失恢复"
    #define DCONF_OP_ACC_BUY "消费"
    #define DCONF_OP_ACC_DAILY "日结算"
    #define DCONF_OP_ACC_SUPPLY "补助"
    #define DCONF_OP_ACC_SUPPLY_CALC "退补助"

#define DCONF_OP_ACC_ALL (DCONF_OP_ACC_BOOK"|" \
                          DCONF_OP_ACC_CANCELBOOK"|" \
                          DCONF_OP_ACC_CHARGE"|" \
                          DCONF_OP_ACC_OPEN"|" \
                          DCONF_OP_ACC_CLOSE"|" \
    DCONF_OP_ACC_LOST"|" \
    DCONF_OP_ACC_LOST_RECOVERY"|" \
                          DCONF_OP_ACC_CHANGE_LOST"|" \
    DCONF_OP_ACC_BUY"|" DCONF_OP_ACC_DAILY"|"DCONF_OP_ACC_SUPPLY"|"DCONF_OP_ACC_SUPPLY_CALC)
#define DCONF_DEPOSIT "押金"
#define DCONF_CHANGE_CARD "挂失换卡费"
#define DCONF_MD "模块定义"
/* 模块定义 */
    #define MD_SYSTEM "系统"
    #define MD_ACCOUNT "账户"
    #define MD_DINNER "食谱"
    #define MD_DEVICE "设备"
    #define MD_REPORT "报表"
    #define DCONF_MD_ALL (MD_SYSTEM "|" MD_ACCOUNT"|" MD_DINNER "|"MD_DEVICE "|"MD_REPORT)

#define DCONF_ZONE "病区"
#define DCONF_DEPT "科室"
#define DCONF_SUPPLY_RECICLE "是否平补助"
#define DCONF_SUPPLY_AMOUNT "月补助金额"



#endif // MW_H
