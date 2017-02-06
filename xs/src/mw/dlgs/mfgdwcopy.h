#ifndef MFGDWCOPY_H
#define MFGDWCOPY_H

#include <QDialog>
#include "mw.h"
#include "mfwidget.h"
class MFGDWCopy : public QDialog
{
    Q_OBJECT
public:
    explicit MFGDWCopy(QWidget *parent = 0);

    MFAutoLineEdit* _from;
    MFAutoLineEdit* _to;

    void showEvent(QShowEvent *);
    
signals:
    
public slots:
    void copy();
    void setToList(QString from);

    
};

#endif // MFGDWCOPY_H
