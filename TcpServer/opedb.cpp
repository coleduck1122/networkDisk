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
    m_db.setDatabaseName("E:\\Code\\Github\\networkDisk\\TcpServer\\cloud.db");
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

int OpeDB::handleSearchUsr(const char *name)
{
    if (NULL == name)
    {
        return -1;
    }
    QString data = QString("select online from userInfo where name='%1'").arg(name);
    QSqlQuery query;
    query.exec(data);

    if (query.next())
    {
        int ret = query.value(0).toInt();
        return ret;
    }
    else
    {
        return -1;
    }
}

int OpeDB::handleAddFriend(const char *pername, const char *name)
{
    if (NULL == pername || NULL == name)
    {
        return -1;  //为空
    }

    QString usr1 = QString("select * from userInfo where name='%1'").arg(name);
    QString usr2 = QString("select * from userInfo where name='%1'").arg(pername);
    QSqlQuery query1, query2;
    query1.exec(usr1);
    query2.exec(usr2);
    if (!query1.next() || !query2.next())
    {
        return 0;   //有一方不存在
    }

    if (0 == strcmp(pername, name))
    {
        return 1;   //不能添加自己为好友
    }

    QString data = QString("select * from friend where (id=(select id from userInfo where name='%1') "
                           "and friendId=(select id from userInfo where name='%2')) ").arg(name).arg(pername);
    QSqlQuery query;
    query.exec(data);
    if (query.next())
    {
        return 2;   //双方已经是好友
    }
    else
    {
        QString data = QString("select online from userInfo where name='%1'").arg(pername);
        QSqlQuery query;
        query.exec(data);

        if (query.next())
        {
            int ret = query.value(0).toInt();
            if (0 == ret)
            {
                return 3;   //对方在线
            }
            else if (1 == ret)
            {
                return 4;   //对方在线，发送申请
            }
        }
    }
    return 0;
}

bool OpeDB::handleAgreeAddFriend(const char *pername, const char *name)
{
    QString data1 = QString("select id from userInfo where name='%1'").arg(pername);
    QString data2 = QString("select id from userInfo where name='%1'").arg(name);
    QSqlQuery query1, query2;
    query1.exec(data1);
    query2.exec(data2);
    int perId = 0, myId = 0;
    if (query1.next())
    {
        perId = query1.value(0).toInt();
    }
    if (query2.next())
    {
        myId = query2.value(0).toInt();
    }

    QString data = QString("insert into friend(id, friendId) values(%1,%2), (%3,%4)").arg(perId).arg(myId).arg(myId).arg(perId);
    QSqlQuery query;
    return query.exec(data);
}

QStringList OpeDB::handleFlushFriend(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear();
    if (NULL == name)
    {
        return strFriendList;
    }
    QString data = QString("select friendId from friend where id = (select id from userInfo where name='%1')").arg(name);
    QSqlQuery query;
    query.exec(data);

    while (query.next())
    {
        int ret = query.value(0).toInt();
        QString data1 = QString("select name from userInfo where online=1 and id=%1").arg(ret);
        QSqlQuery query1;
        query1.exec(data1);
        if (query1.next())
        {
            QString strTmp = query1.value(0).toString();
            strFriendList.append(strTmp);
            qDebug() << strTmp;
        }
    }
    return strFriendList;
}

bool OpeDB::handleDeleteFriend(const char *name, const char *friendName)
{
    if (NULL == name || NULL == friendName)
    {
        return false;
    }

    QString data1 = QString("select id from userInfo where name='%1'").arg(friendName);
    QString data2 = QString("select id from userInfo where name='%1'").arg(name);
    QSqlQuery query1, query2;
    query1.exec(data1);
    query2.exec(data2);
    int friendId = 0, selfId = 0;
    if (query1.next())
    {
        friendId = query1.value(0).toInt();
    }
    if (query2.next())
    {
        selfId = query2.value(0).toInt();
    }

    QString data = QString("delete from friend where id=%1 and friendId=%2").arg(selfId).arg(friendId);
    QSqlQuery query;
    query.exec(data);

    data = QString("delete from friend where id=%1 and friendId=%2").arg(friendId).arg(selfId);
    query.exec(data);

    return true;

}

QStringList OpeDB::handleAllOnline()
{
    QString data = QString("select name from userInfo where online=1");
    QSqlQuery query;
    query.exec(data);

    QStringList result;
    result.clear();
    while (query.next())
    {
        result.append(query.value(0).toString());
    }
    return result;
}

