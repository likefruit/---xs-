#ifndef MFREPORT_H
#define MFREPORT_H
#include "mfworkarea.h"
#include "mfsqltablemodel.h"

class MFReportRecord : public QObject
{
public:
    QVariant* _vars;

    MFReportRecord(int size, QObject* parent = NULL):
        QObject(parent)
    {
        _vars = new QVariant[size];
    }

    ~MFReportRecord()
    {
        delete [] _vars;
    }

    void setValue(int i, QVariant value)
    {
        _vars[i] = value;
    }

    const QVariant& value(int i)
    {
        return _vars[i];
    }
};

class MFReportFilter : public QGroupBox
{
    Q_OBJECT
public:
    explicit MFReportFilter(QString name = MW_FILTER_TITLE): QGroupBox(name)
    {
        _layout = new QHBoxLayout();
        QHBoxLayout* v = new QHBoxLayout(this);
        v->addLayout(_layout);
        v->addStretch(1);

    }
#define MFREPORT_FILTER_SINGLE 0
#define MFREPORT_FILTER_RANGE 1
    void addFilter(QString table, QString field, QString title, int type, QString filter_string)
    {
        MFSingleFilter* filter;
        if(_filters.count() > 0)
        {
            _layout->addWidget(new QLabel("      "));
        }

        if(type == MFREPORT_FILTER_SINGLE)
        {
            filter = new MFSingleFilter(table, field, title, filter_string);
        }
        else if (type == MFREPORT_FILTER_RANGE)
        {
            filter = new MFRangeFilter(table, field, title, filter_string);
        }
        else
        {
            return;
        }
        _layout->addWidget(filter);

        _filters.append(filter);

        connect(filter, SIGNAL(selectChange(QString)), this, SIGNAL(filterChange(QString)));
    }

    QHBoxLayout* _layout;

    void reloadList()
    {
        foreach(MFFilterWidget* f, _filters)
        {
            f->reloadList();
        }
    }

    QString filter()
    {
        QString ret;
        foreach(MFFilterWidget* f, _filters)
        {
            if(ret.length()==0)
                ret = f->filter();
            else
            {
                QString str = f->filter();
                if(str.length())
                    ret += (" and " + f->filter());
            }

        }
        return ret;
    }


    QList<MFFilterWidget*> _filters;
signals:
    void filterChange(QString);
};
class MFReport;
class MFReportArea : public QTextBrowser
{
    Q_OBJECT
public:
    explicit MFReportArea(QString title, QWidget *parent = 0):QTextBrowser(parent)
    {
        _title = title;
        qApp->postEvent(this, new QEvent(EV_CREATE));
        //qApp->postEvent(this, new QEvent(EV_INIT));
        this->setReadOnly(true);
        _td_font_size = 3;
    }

    virtual void OnCreate()
    {

    }

    bool event(QEvent *e)
    {
        if(e->type() == EV_CREATE/* || e->type() == EV_INIT*/)
        {
            OnCreate();
            return true;
        }
        return QTextEdit::event(e);
    }

    QString _title;
    QString _content;
    MFReportFilter* _filter;
    int _td_font_size;
    MFReport* _report;
    void setReport(MFReport* report)
    {
        _report = report;
    }


    void starHtml()
    {
        _content.append("<html>");
        _content.append(QString("<head></head>"));
        _content.append("<body>");
    }
    void endHtml()
    {
        _content.append("</body></html>");
    }

    void beginTable(QString caption = QString(),int border=1)
    {
        pText(caption);
        _content.append(QString("<table width=100% border='%1'>").arg(border));

    }
    void pText(QString str)
    {
        if(str.length())
            _content.append(QString("<p><font size=%2>%1</font></p>").arg(str).arg(_td_font_size));
    }

    void br()
    {
        _content.append("<br>");
    }

    void endTable()
    {
        _content.append("</table>");
    }

    void beginTd()
    {
        _content.append("<td>");
    }
    void endTd()
    {
        _content.append("</td>");
    }

    void beginRow()
    {
        _content.append("<tr>");
    }

    void endRow()
    {
        _content.append("</tr>");
    }

    void th(QString value, int w = -1)
    {
        if(w == -1)
            _content.append(QString("<th><font size=%2>%1</font></th>").arg(value).arg(_td_font_size));
        else
            _content.append(QString("<th><font size=%2 width=%3%>%1</font></th>").arg(value).arg(_td_font_size).arg(w));
    }

    void td(QString value, QString align = "center", int colspan=1, int width=-1)
    {
        if(width == -1)
            _content.append(QString("<td align='%2' colspan=%3><font size=%4>%1</font></td>").arg(value).arg(align).arg(colspan).arg(_td_font_size));
        else
            _content.append(QString("<td align='%2' colspan=%3 width=%5%><font size=%4>%1</font></td>").arg(value).arg(align).arg(colspan).arg(_td_font_size).arg(width));
    }

    virtual void setTitle()
    {
        _content.append(QString("<h1 align=\"center\">%1</h1>").arg(_title));
    }

    virtual void setHeaer()
    {

        beginTable(QString(), 0);
        beginRow();
        this->td("制表时间：" + MFNowDateTime(), "left");
        this->td("制表人：" + MFGetOper(), "right");
        endRow();

        QString strFilter;
        for(int i=0; i<_filter->_filters.count(); i++)
        {
            if(i!=0)
                strFilter += "  ";
            strFilter += this->_filter->_filters.at(i)->printLabel();
        }
        beginRow();

        this->td(QString("筛选条件：【%1】").arg(strFilter), "left", 2);

        endRow();


        endTable();
    }
    virtual bool setBody() = 0;
    virtual void setExtra()
    {}

    QSize _pageSize;
    QString _filterString;

signals:

public slots:
    bool updateContent()
    {
        bool bRet;
        _content.clear();

        starHtml();
        setTitle();
        setHeaer();
        bRet = setBody();
        setExtra();
        endHtml();

        this->setHtml(_content);
        return bRet;
    }

    void slotPrePrintReq(QPrinter* printer)
    {
        updateContent();
        QSizeF size = document()->pageSize();
        this->document()->setPageSize(printer->pageRect().size());
        this->print(printer);
        this->document()->setPageSize(size);

    }

    void slotPrintReq(QPrinter* printer)
    {
        updateContent();
        QSizeF size = document()->pageSize();
        this->document()->setPageSize(printer->pageRect().size());
        this->print(printer);
        this->document()->setPageSize(size);
    }
};


class MFReport : public MFWorkArea
{
    Q_OBJECT
public:
    explicit MFReport(/*QWidget* filter, */MFReportArea* workArea, QWidget *parent = 0):
        MFWorkArea(_filter = new MFReportFilter(), workArea, parent)
    {
        _reportArea = workArea;
        _reportArea->setReport(this);
        _reportArea->_filter = _filter;

        this->_bottomLeftLayout->addWidget(_fondLarger = new QPushButton("增大字体"));
        this->_bottomLeftLayout->addWidget(_fontLittle = new QPushButton("减小字体"));
        this->_bottomLeftLayout->addSpacing(20);
        this->_bottomLeftLayout->addWidget(new QLabel("报表格式："));
        this->_bottomLeftLayout->addWidget(_comboStyle = new MFAutoLineEdit()); _comboStyle->setFit();
        _comboStyle->addItem(MW_REPORT_STYLE_NORMAL);
        //this->_bottomLeftLayout->addSpacing(20);
        this->_bottomRightLayout->addWidget(_printPrev = new QPushButton("打印预览"));
        this->_bottomRightLayout->addWidget(_print = new QPushButton("打印"));

        connect(_printPrev, SIGNAL(clicked()), this, SLOT(slotPrint()));
        connect(_print, SIGNAL(clicked()), this, SLOT(slotPrint()));

        _previewDlg = new QPrintPreviewDialog(this);
        _printDlg = new QPrintDialog(this);

        connect(_previewDlg, SIGNAL(paintRequested(QPrinter *)), _reportArea, SLOT(slotPrePrintReq(QPrinter*)));
        connect(_printDlg, SIGNAL(accepted(QPrinter*)), _reportArea, SLOT(slotPrintReq(QPrinter*)));

        connect(_filter, SIGNAL(filterChange(QString)), this, SLOT(filterChange(QString)));
        connect(_fondLarger, SIGNAL(clicked()), this, SLOT(slotFontChange()));
        connect(_fontLittle, SIGNAL(clicked()), this, SLOT(slotFontChange()));
        connect(_comboStyle, SIGNAL(currentIndexChanged(QString)), this, SLOT(filterChange(QString)));
    }

    QPushButton* _printPrev;
    QPushButton* _print;
    QPushButton* _fondLarger;
    QPushButton* _fontLittle;
    MFAutoLineEdit* _comboStyle;

    MFReportArea* _reportArea;
    QPrintPreviewDialog* _previewDlg;
    QPrintDialog* _printDlg;

    MFReportFilter* _filter;

    void showEvent(QShowEvent *)
    {
        mylog << "show event";
        this->_reportArea->_filter->reloadList();
    }

    void OnCreate()
    {
        _reportArea->_filterString = _filter->filter();
        _reportArea->updateContent();
    }

signals:

public slots:
    void slotPrint()
    {
        QObject* sender = this->sender();
        if(sender == _printPrev)
        {
            _previewDlg->exec();
        }
        else /* print */
        {
            _printDlg->exec();
        }
    }

    void filterChange(QString)
    {
        _reportArea->_filterString = _filter->filter();
        _reportArea->updateContent();
    }

    void slotFontChange()
    {
        QObject* obj = sender();
        if(obj == _fondLarger)
            this->_reportArea->_td_font_size ++;
        else
            this->_reportArea->_td_font_size --;

        if(this->_reportArea->_td_font_size < 3)
        {
            this->_reportArea->_td_font_size = 3;
            return;
        }
        if(this->_reportArea->_td_font_size > 8)
        {
            this->_reportArea->_td_font_size = 8;
            return;
        }

        mylog << this->_reportArea->_td_font_size;
        _reportArea->updateContent();
    }
};

class MFReportDPArea : public MFReportArea
{
    Q_OBJECT
public:
    explicit MFReportDPArea(QWidget *parent = 0):MFReportArea("周食谱", parent)
    {

    }

    void OnCreate()
    {
        _report->_comboStyle->addItem(MW_REPORT_STYLE_COMPACT);
    }

    bool setBody();
    bool setBodyCompact();
    bool setBodyNormal();

signals:

public slots:

};

class MFReportDP : public MFReport
{
    Q_OBJECT
public:
    explicit MFReportDP(QWidget *parent = 0);
};



class MFReportAreaZuofan : public MFReportArea
{
    Q_OBJECT
public:
    explicit MFReportAreaZuofan(QWidget *parent = 0):MFReportArea("做饭表", parent)
    {

    }

    bool setBody();
};

class MFReportZuofan : public MFReport
{
    Q_OBJECT
public:
    explicit MFReportZuofan(QWidget *parent = 0);
};

class MFReportAreaLinfan : public MFReportArea
{
    Q_OBJECT
public:
    explicit MFReportAreaLinfan(QWidget *parent = 0):MFReportArea("领饭表", parent)
    {

    }

    bool setBody();

};

class MFReportLinfan : public MFReport
{
    Q_OBJECT
public:
    explicit MFReportLinfan(QWidget *parent = 0) ;
};

class MFReportAreaFafan : public MFReportArea
{
    Q_OBJECT
public:
    explicit MFReportAreaFafan(QWidget *parent = 0):MFReportArea("发饭表", parent)
    {

    }

    bool setBody();
};

class MFReportFafan : public MFReport
{
    Q_OBJECT
public:
    explicit MFReportFafan(QWidget *parent = 0);

    QPushButton* _batchPrintByDept;
    QPushButton* _batchPrintByZone;

public slots:
    void batchPrint();
};

class MFReportAreaCaigou : public MFReportArea
{
    Q_OBJECT
public:
    explicit MFReportAreaCaigou(QWidget *parent = 0):MFReportArea("领料表", parent)
    {

    }

    bool setBody();
};

class MFReportCaigou : public MFReport
{
    Q_OBJECT
public:
    explicit MFReportCaigou(QWidget *parent = 0);
};

class MFReportAreaCaiwu : public MFReportArea
{
    Q_OBJECT
public:
    explicit MFReportAreaCaiwu(QWidget *parent = 0):MFReportArea("财务收支表", parent)
    {

    }

    bool setBody();
};

class MFReportCaiwu : public MFReport
{
    Q_OBJECT
public:
    explicit MFReportCaiwu(QWidget *parent = 0);
};


class MFReportAreaPersonalFlow : public MFReportArea
{
    Q_OBJECT
public:
    explicit MFReportAreaPersonalFlow(QWidget *parent = 0):MFReportArea("个人业务流水", parent)
    {

    }

    bool setBody();
};

class MFReportPersonalFlow : public MFReport
{
    Q_OBJECT
public:
    explicit MFReportPersonalFlow(QWidget *parent = 0);
};




#endif // MFREPORT_H
