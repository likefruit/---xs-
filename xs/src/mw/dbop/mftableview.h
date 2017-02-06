#ifndef MFTABLEVIEW_H
#define MFTABLEVIEW_H

#include <QTableView>
class MFTableViewContainer;
class MFTableViewItemDel;
#include <QStandardItemModel>
#include "mftableitemdelegate.h"
#include "mfsqltablemodel.h"
#include <QDebug>

class MFTableView : public QTableView
{
    Q_OBJECT
public:
    MFSqlTableModel* _model;

    explicit MFTableView(QString modelName, QWidget *parent = 0);


    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void sigSelectionChanged(QItemSelection selected, QItemSelection deselected);
    void sigRightClicked();
    
public slots:
    
};

#endif // MFTABLEVIEW_H
