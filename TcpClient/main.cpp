#include "tcpclient.h"
//#include "opewidget.h"
//#include "online.h"
#include "friend.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    TcpClient w;
//    w.show();
//    OpeWidget w;
//    w.show();
//    Online w;
//    w.show();
    Friend w;
    w.show();
    return a.exec();
}
