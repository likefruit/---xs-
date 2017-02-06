#ifndef MFSETUPSQLCONN_H
#define MFSETUPSQLCONN_H


#include "mw.h"
#include "mfwidget.h"
#define QUERY_RESULT() (b && query.lastError().type() == QSqlError::NoError)
#define QUERY_ERR() do{_log->append(query.lastError().text()); query.exec("drop database mdb"); return false;}while(0)

class MFSetupSqlConn : public MFWidget
{
    Q_OBJECT
public:
    explicit MFSetupSqlConn(QWidget *parent = 0);

    
    QTextEdit* _log;
    QWidget* w[2][5];

    bool initDb(QString host, QString user, QString pass, QString db);
    bool createDb(QSqlDatabase& _db, QString db);

    void saveconfig(QString host, QString user, QString pass, QString db);
signals:
    
public slots:
    void ok();
    void cancel();
};

#endif // MFSETUPSQLCONN_H
