#ifndef MFVIEW_H
#define MFVIEW_H

#include <QMap>
#include "mw.h"
#include "mfworkarea.h"

#include "mw.h"
#include "mfview.h"
#include "mfsetupsqlconn.h"
#include "mfviews.h"
#include "mfreport.h"
#include <QTreeWidgetItemIterator>

class MFView : public QWidget
{
    Q_OBJECT
public:
    explicit MFView(QWidget *parent = 0):
        QWidget(parent)
    {
        _cur = NULL;
        setLayout(new QVBoxLayout());
        layout()->setMargin(0);
        layout()->setSpacing(0);
        //  layout()->setMargin(layout()->margin()/2);

        //   _stop_start_card_detect = new QPushButton("关闭读卡器检测程序", this);
        //   _stop_start_card_detect->show();

        MFSetWndExStyle(this, WS_EX_STATICEDGE);

        //   SetWindowLong(this->winId(), GWL_EXSTYLE,
        //                GetWindowLong(this->winId(), GWL_EXSTYLE) | WS_EX_STATICEDGE);
    }



    void paintEvent(QPaintEvent *)
    {
        QPainter p(this);
#if 0

        QPixmap map("res/dongfang.jpg");

#if 1
        QSize msize = map.size();
        map = map.scaled(msize/3);
#endif
        QRect rc = rect();
        QPoint pt = rc.topRight();
        pt.setX(pt.x() - map.width() - 2);
        pt.setY(pt.y() + 2);



        map.setMask(map.createMaskFromColor(Qt::white, Qt::MaskInColor));

        p.drawPixmap(pt,map);

#endif
        p.end();
    }

    void resizeEvent(QResizeEvent *)
    {
        QRect rc = rect();
        QSize sz = QSize(120, 30);// _stop_start_card_detect->size();

        rc.setLeft(rc.right() - sz.width());
        rc.setBottom(rc.top() + sz.height());

        //    _stop_start_card_detect->setGeometry(rc);
    }

    //  QPushButton* _stop_start_card_detect;
    void mousePressEvent(QMouseEvent *ev)
    {
        QPixmap map(size());
        this->render(&map);

        QImage img;
        img = map.toImage();

        QColor col((img.pixel(ev->pos())));

        mylog<<"*******************"<<col.red() << col.green() << col.blue();

    }
    QMap<QString, MFWidget*> _subviews;
    MFWidget* _cur;
    void showWorkArea(QString str)
    {
        MFWidget* w = findWorkArea(str);
        if(w)
        {
            if(_cur)
                _cur->hide();
            _cur = w;
            _cur->show();
        }
    }

    MFWidget* findWorkArea(QString str)
    {
        MFWidget* w = NULL;
        QMap<QString, MFWidget*>::iterator it = _subviews.find(str);
        if(it !=_subviews.end())
            return it.value();

        if(str == MENU_ACC_MAN)
        {
            w  =new MFDAcc(this);
        }
        if(str == MENU_DEVICE_UPGRADE)
        {
            w = new MFDeviceUpgrade(this);
        }
        else if(str == MENU_SYS_INIT)
        {
            w = new MFSetupSqlConn(this);
        }
        else if(str == MENU_ABOUT_RIGHT)
        {
            w = new MFAboutThisWork1(this);
        }
        else if(str == MENU_DEVICE_DATA)
        {
            w = new MFDeviceData(this);
        }
        else if(str == MENU_ACC_LOST)
        {
            w = new MFLostCard(this);
        }
        else if(str == MENU_REPORT_ORDER)
        {
            w = new MFDOrder(this);
        }
        else if(str == MENU_DEVICE_MAN)
        {
            w = new MFDevice(this);
        }
        else if(str == MENU_FOOD_ALL)
        {
            w = new MFGDL(this);
        }
        else if(str==MENU_FOOD_WEEK)
        {
            w = new MFGDWSetup(this);
        }
        else if(str == MENU_FOOD_METIAL)
        {
            w  = new MFDM(this);
        }
        else if(str == MENU_REPORT_FLOW)
        {
            w = new MFDFlow(this);
        }
        else if(str == MENU_SYS_LOGIN)
        {
            w = new MFLogin1(this);
        }
        else if(str == MENU_DEVICE_POSOP)
        {
            w = new MFPOSOP(this);
        }
        else if(str == MENU_DEVICE_DP)
        {
            w = new MFDeviceDP(this);
        }
        else if(str == MENU_SYS_MODIFY_PASSWORD)
        {
            w = new MFModifyPassword1(this);
        }
        else if(str == MENU_SYS_SETUP_PARAM)
        {
            w = new MFSetupParam(this);
        }
        else if(str == MENU_SYS_OP_MAN)
        {
            w = new MFDOPMan(this);
        }
        else if(str == MENU_REPORT_GDW)
        {
            w = new MFReportDP(this);
        }
        else if(str == MENU_REPORT_FAFAN)
        {
            w = new MFReportFafan(this);
        }
        else if(str == MENU_REPORT_LINFAN)
        {
            w = new MFReportLinfan(this);
        }
        else if(str == MENU_REPORT_ZUOFAN)
        {
            w = new MFReportZuofan(this);
        }
        else if(str == MENU_REPORT_CAIGOU)
        {
            w = new MFReportCaigou(this);
        }
        else if(str == MENU_REPORT_CAIWU)
        {
            w = new MFReportCaiwu(this);
        }
        else if(str == MENU_REPORT_PERSONAL)
        {
            w = new MFReportPersonalFlow(this);
        }
        else if(str == MENU_SYS_EXIT)
        {
            qApp->quit();
            return NULL;
        }
        else
        {}

        w->hide();
        layout()->addWidget(w);

        _subviews.insert(str, w);
        return w;
    }

signals:
    
public slots:
    
};

#endif // MFVIEW_H
