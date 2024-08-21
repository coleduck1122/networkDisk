#include "opedb.h"

#include <QDebug>
#include <QMessageBox>

OpeDB::OpeDB(QObject *parent)
    : QObject{parent}
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("E:\\Code\\Pan\\Project\\TcpServer\\cloud.db");
    if (m_db.open())
    {
        QSqlQuery query;
        query.exec("select * from userInfo");
        while (query.next())
        {
            QString data = QString("%1,%2,%3")
                           .arg(query.value(0).toString())
                           .arg(query.value(1).toString())
                           .arg(query.value(2).toString());
            qDebug() << data;
        }
    }
    else
    {
        QMessageBox::critical(NULL, "连接数据库", "连接数据库失败");
    }
}

OpeDB::~OpeDB()
{
    m_db.close();
}

bool OpeDB::handleRegist(const char *name, const char *pwd)
{
    if (NULL == name || NULL == pwd)
    {
        return false;
    }
    QString data = QString("insert into userInfo(name, pwd) values('%1', '%2')").arg(name).arg(pwd);
    QSqlQuery query;
    return query.exec(data);
}

bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    if (NULL == name || NULL == pwd)
    {
        return false;
    }
    QString data = QString("select * from userInfo where name='%1' and pwd='%2' and online=0").arg(name).arg(pwd);
    QSqlQuery query;
    query.exec(data);

    if (query.next())
    {
        data = QString("update userInfo set online=1 where name='%1' and pwd='%2'").arg(name).arg(pwd);
        query.exec(data);
        return true;
    }
    else
    {
        return false;
    }
}

void OpeDB::handleOffline(const char *name)
{
    if (NULL == name)
    {
        return;
    }
    QString data = QString("update userInfo set online=0 where name='%1'").arg(name);
    QSqlQuery query;
    query.exec(data);
}
