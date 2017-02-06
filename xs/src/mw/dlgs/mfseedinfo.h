#ifndef MFSEEDINFO_H
#define MFSEEDINFO_H

#include <QDialog>
#include "mw.h"
class MFSeedInfo : public QDialog
{
    Q_OBJECT
public:
    explicit MFSeedInfo(QString seed, QWidget *parent = 0);

    QLineEdit* _license;
    QString _seed;
    bool check_license(QString seed, QString file);
    
signals:
    
public slots:
    void slotOk();
    void slotBrowser();
    
};

#endif // MFSEEDINFO_H
