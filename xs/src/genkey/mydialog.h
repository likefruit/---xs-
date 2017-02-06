#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QTextCodec>
#include "../com/xs.h"
class MyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MyDialog(QWidget *parent = 0);
    QLineEdit* _license;
    QLineEdit* _key;
    
signals:
    
public slots:
    void slotOk();
    void slotBrowser();
};

#endif // MYDIALOG_H
