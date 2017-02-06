#ifndef MFDACCOPTION_H
#define MFDACCOPTION_H

#include <QDialog>
#include "mw.h"

class MFDAccOption : public QDialog
{
    Q_OBJECT
public:
    QGroupBox* _group;
    QRadioButton* _check1;
    QRadioButton* _check2;
    QRadioButton* _check3;
    QRadioButton* _check4;
    QRadioButton* _check5;
    QRadioButton* _check6;

    bool _already_open;

    explicit MFDAccOption(bool already_open = true, QWidget *parent = 0);

    void initGroup();
    void initCommands(QHBoxLayout* layout);
    
signals:
    
public slots:
    
};

#endif // MFDACCOPTION_H
