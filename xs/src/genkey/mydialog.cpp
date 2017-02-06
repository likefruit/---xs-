#include "mydialog.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include "../mwcom/mwlib/mwlib.h"
MyDialog::MyDialog(QWidget *parent) :
    QDialog(parent)
{
    QPushButton* button;
    QPushButton* browser;
    QGridLayout* layout = new QGridLayout(this);

    layout->addWidget(new QLabel("License种子串："), 0, 0);
    layout->addWidget(_key = new QLineEdit(), 0, 1);
    layout->addWidget(new QLabel("License文件路径："), 1, 0);
    layout->addWidget(_license = new QLineEdit(), 1, 1);
    layout->addWidget( browser = new QPushButton("浏览"), 2, 0);
    layout->addWidget(button = new QPushButton("确认"), 2, 1);

    connect(button, SIGNAL(clicked()), this, SLOT(slotOk()));
    connect(browser, SIGNAL(clicked()), this, SLOT(slotBrowser()));
}

void MyDialog::slotOk()
{
    xs_dict_t* dict = NULL;
    int size;
    dict = xs_dict_add_str(NULL, "key", _key->text().toAscii().data());
    dict = xs_dict_add_str(dict, "expire", "9999-99-99");

    char* buf = xs_dict_to_buf(dict, &size);

    mwlib_encrypt((unsigned char* )buf, size+1);


    QFile file(_license->text());
    qDebug()  << _license->text();
    file.open(QFile::WriteOnly);
    file.write(buf, size+1);
    file.close();

    xs_free(buf);

}

void MyDialog::slotBrowser()
{
    QString str = QFileDialog::getSaveFileName(this, "保存License文件");
    if(str.length())
        _license->setText(str);
}
int main(int argc, char** argv)
{
    xs_init();
    QApplication app(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF8");

    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    MyDialog dialog;
    dialog.show();

    app.exec();
    xs_fini();
    return 0;
}
