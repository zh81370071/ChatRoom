#include "mainwidget.h"
#include <QApplication>
#include"register.h"
#include <QSqlDatabase>
#include <QDebug>
#include<QMessageBox>
#include"register.h"
#include <QTextCodec>





int main(int argc, char *argv[])

{


    QApplication a(argc, argv);
    mainWidget w;
    w.show();
    return a.exec();
}
