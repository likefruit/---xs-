
#include "mftableview.h"

MFTableView::MFTableView(QString modelName, QWidget *parent) :
    QTableView(parent)
{
    setModel(_model = MFGetModel(modelName, MFSqlTableModel::EDIT));

    setSortingEnabled(true);
    setAlternatingRowColors(true);


}


void MFTableView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QTableView::selectionChanged(selected, deselected);
    emit sigSelectionChanged(selected, deselected);
}

void MFTableView::mouseReleaseEvent(QMouseEvent *event)
{
    QTableView::mouseReleaseEvent(event);
    if(event->button() == Qt::RightButton)
    {
        if(this->selectionModel()->hasSelection())
        {
            mylog << "right clicked";
            emit sigRightClicked();
        }
    }
}
