#include "mfseedinfo.h"
#include "mwlib/mwlib.h"

MFSeedInfo::MFSeedInfo(QString seed, QWidget *parent) :
    QDialog(parent)
{
    QPushButton* button;
    QPushButton* browser;
    QGridLayout* layout = new QGridLayout(this);

    layout->addWidget(new QLabel("License种子串："), 0, 0);
    layout->addWidget(new QLineEdit(seed), 0, 1);
    layout->addWidget(new QLabel("License文件路径："), 1, 0);
    layout->addWidget( _license = new QLineEdit(), 1, 1);
    layout->addWidget( browser = new QPushButton("浏览"), 2, 0);
    layout->addWidget(button = new QPushButton("确认"), 2, 1);

    _seed = seed;

    connect(button, SIGNAL(clicked()), this, SLOT(slotOk()));
    connect(browser, SIGNAL(clicked()), this, SLOT(slotBrowser()));
}

void MFSeedInfo::slotOk()
{
    QString path = _license->text();

    if(!QFile::exists(path))
    {
        MFLog("file not exist");
        reject();
        return;
    }

    if(check_license(_seed, _license->text()))
    {
        QFile::copy(path, MW_LICENSE);
        accept();
    }
    else
        reject();
}

bool MFSeedInfo::check_license(QString seed, QString filename)
{
    QFile file(filename);
    file.open(QFile::ReadOnly);
    QByteArray buf = file.readAll();
    file.close();

    mylog << buf.length();

    char* p = (char*)xs_malloc(buf.length()+1);
    memcpy(p, buf.data(), buf.length());
    p[buf.length()] = 0;

    mwlib_decrypt((unsigned char*)p, buf.length());

    mylog << p;

    xs_dict_t* dict = xs_dict_from_buf(p, NULL);
    char* key = xs_dict_find_value(dict, "key");
    if(seed == key)
    {
        return true;
    }

    return false;
}

void MFSeedInfo::slotBrowser()
{
    QString str = QFileDialog::getOpenFileName(this, "打开License文件");
    if(str.length())
        _license->setText(str);
}
