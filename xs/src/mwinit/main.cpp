#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <stdio.h>
#include <QTextCodec>
#include "opcard.h"
#include "xs.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("UTF8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    xs_init();

    unsigned long cardno;
    unsigned long cardno_old = -1;
    unsigned char buf[1024];

    int ret;

    while(1)
    {
#ifdef WIN32
        Sleep(1000);
#endif
        ret = read_card(&cardno, buf, NULL, NULL);
        for(int i=0; i<4; i++)
        {
            if(cardno == *(unsigned long*)(buf+i*4))
            {
                qDebug() << i;
            }
        }
        if(ret < 0)
        {
            qDebug() << "error read card return =" << ret;
            continue;
        }
        if(cardno == cardno_old)
        {
            qDebug() << "please change card";
#ifdef WIN32
            Sleep(5000);
#endif
            continue;
        }

        init_card(buf);

        ret = write_card(&cardno, buf, NULL, NULL);
        if(ret < 0)
        {
            qDebug() << "write card error";
            continue;
        }

        qDebug() << "write card ok" ;
        cardno_old = cardno;

    }

    xs_fini();

    return a.exec();
}
