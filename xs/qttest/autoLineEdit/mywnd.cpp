#include "mywnd.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QDir>

MyWnd::MyWnd(QWidget *parent) :
    QWidget(parent)
{
}

void scanDir(QDir& dir)
{
    static QStringList careSuffix;
    if(careSuffix.length() == 0)
    {
        careSuffix << "jpg" << "jpeg";
    }

    QDir dirSub;
    foreach(QFileInfo afile, dir.entryInfoList())
    {

        if(!afile.isDir() && careSuffix.indexOf(afile.suffix().toLower()))
        {
            qDebug() << afile.filePath();
        }
        else
        {
            if(afile.fileName() == "." || afile.fileName() == "..")
                continue;
            dirSub = QDir(afile.filePath());
            scanDir(dirSub);
        }
    }
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
#if 1
    MFAutoLineEdit* e1 = new MFAutoLineEdit(QStringList() << QString("aaa") << "abc" << "cde");

#if 1
    QComboBox box;
    box.setLineEdit(e1);

    box.show();
#else
    e1->show();


    MyWnd wnd;
    QHBoxLayout* layout;

    wnd.setLayout(layout = new QHBoxLayout());
    layout->addWidget(e1);
    layout->addWidget(new QLineEdit());
    wnd.show();
#endif
#endif

#if 0
    QDir dir = QDir::root();

    qDebug() << dir.entryList();
#endif

    QDir dir(".");
    scanDir(dir);




    return app.exec();
    return 0;
}
