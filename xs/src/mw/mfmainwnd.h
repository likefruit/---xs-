#ifndef MFMAINWND_H
#define MFMAINWND_H

#include "mw.h"
#include "mfview.h"
#include "mfsetupsqlconn.h"
#include "mftoolbar.h"
#include "mfcarddetect.h"
#include "mfviews.h"
#include "mfdaccinfo.h"
#include "mfdaccoption.h"

class MFMainWnd : public QMainWindow
{
    Q_OBJECT
public:
    explicit MFMainWnd(QWidget *parent = 0);

    void initMenu();
    void loadConf();

    static MFMainWnd* This;
    MFCardDetect* _cardDetect;

    QLabel _info;
    QLabel _oper;
    MFView* _view;
    MFSetupSqlConn* _setupDlg;

    MFToolBar* _toolbar;
    QList<QAction*> _acts;
    // MFLogin* _login;
    QString _user;
    QString _auth;
    bool haveAuth(QString str)
    {
        return _auth.indexOf(str) != -1;
    }
    bool havenotAuth(QString str)
    {
        return !haveAuth(str);
    }
    void logoff()
    {
        _user.clear();
        _auth.clear();
    }
    bool isLogin()
    {
        return _user.length() > 0;
    }

    QWidget* _frm;

    QSqlQuery* _query;
    QAction* _initAction;
    QMenu* _systemMenu;
    bool _btrans_data_with_pos;

    QMap<QString, QStringList> _conf;

    bool event(QEvent *event);
    void showEvent(QShowEvent *);





    void showWindow(QString name);

    //    QSqlQuery* getQuery();

    struct MenuItem
    {
        QIcon _icon;
        QString _display;
        QString _treedisplay;
        QString _shotcut;
        bool _isAction;

        QList<MenuItem*> _childs;
    };

    QList<MenuItem*> _menus;
    //    QLabel* _logo;


    QPushButton* _button_stop_or_start_card_detect;
    QToolBar* _toolbar1;

    void resizeEvent(QResizeEvent *ev)
    {
        QRect rc = _toolbar1->geometry();
        QSize sz(100, 30);
        rc.setLeft(rc.right() - sz.width());
        rc.setTop(rc.top() + (rc.height() - sz.height())/2);
        rc.setBottom(rc.bottom() - (rc.height() - sz.height())/2-10);
        _button_stop_or_start_card_detect->setGeometry(rc);

        QMainWindow::resizeEvent(ev);
    }

    void initToolBar()
    {
        _toolbar1 = this->addToolBar("mytoolbar");
        _toolbar1->setFloatable(false);
        _toolbar1->setAllowedAreas(Qt::TopToolBarArea);

        _button_stop_or_start_card_detect = new QPushButton("关闭读卡器检测",this);
        _button_stop_or_start_card_detect->setEnabled(false);
        connect(_button_stop_or_start_card_detect, SIGNAL(clicked()), this, SLOT(triggleCardDetect()));
        _toolbar1->addActions(_acts);
        _toolbar1->setIconSize(_toolbar1->iconSize() * 2);

        QSize sz = _toolbar1->sizeHint();
        _toolbar1->setMinimumSize(sz.width() + 100, sz.height());
    }

    void handleCard(unsigned char* buf, unsigned long cardno);

    /* 是否可以给补助了 */
    bool canSupply(int accid);
    bool canCancelBook(int accid);

signals:
    
public slots:
    void triggleMenu();
    void cardDataArrive(unsigned char*buf, unsigned long cardno)
    {
        if(is_valid_card(buf) == 0)
        {
            if(!isLogin() || !haveAuth(MD_ACCOUNT))
            {
                QMessageBox::warning(this, "错误", "你没有操作账户权限");
            }
            else
            {
                handleCard(buf, cardno);
            }
        }
        else
        {
            QMessageBox::warning(this, "错误", "这是一张非法卡");
        }

        this->_cardDetect->setStop(false);
        delete []buf;
    }

    void triggleCardDetect()
    {
        this->_cardDetect->_stop = !this->_cardDetect->_stop;
        if(this->_cardDetect->_stop)
            _button_stop_or_start_card_detect->setText("开启读卡器检测");
        else
            _button_stop_or_start_card_detect->setText("关闭读卡器检测");
    }

};



#endif // MFMAINWND_H
