#ifndef MFTABLEVIEWCONTAINER_H
#define MFTABLEVIEWCONTAINER_H


#include "mw.h"
#include "mftableview.h"
#include "mfworkarea.h"
#include "mffilter.h"
#if 0
#define ALL_FIELD "所有字段"
#define RELATION_LIKE "模糊匹配"
#define RELATION_LIKE1 "like"
#endif
/*
   过滤：[过滤类型] [过滤关系(> = < 模糊)] [过滤值 ]
   |----------------------------------|
   | display data                     |
   |                                  |
   |                                  |
   |                                  |
   |                                  |
   |                                  |
   |                                  |
   |                                  |
   |                                  |
   |                                  |
   |----------------------------------|

   [delete] [insert] [submit] [自动提交]
*/

class MFTableViewContainer : public MFWorkArea
{
    Q_OBJECT
public:

    MFTableView* _table;

    QString _md;
    QString _strFilter;
    MFWorkFilter* _workFilter;

    QPushButton* _delete; QPushButton* _insert;QPushButton* _submit; QPushButton* _revert;
    QPushButton* _import;
    QPushButton* _export_;
    QString _extraFilterString;

    explicit MFTableViewContainer(QString tableName, QString md, QWidget *parent = 0);


 //   QMap<int, QString> _fields;

    void addFieldDict(QString humanName, QString machineName, int index, bool b = true);

    void OnCreate();

    virtual bool canAddFilter()
    {
        return _table->_model->record().count() > _workFilter->_filter->_filters.count();
    }
    virtual void newFilterAdded(MFFilter::filter* )
    {

    }

    virtual void reset() = 0;

signals:
    
public slots:
    void deleteRow();
    void insertRow();
    void revert();
    void submit();


    void addFilter();

    void slotImport();
    void slotExport();
};

#endif // MFTABLEVIEWCONTAINER_H
