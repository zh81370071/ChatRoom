#include "databaselink.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include<QApplication>
#include<QSql>
#include<QString>
#include<QMessageBox>
#include<QSqlRecord>

DataBaselink* DataBaselink::userDB=nullptr;

//连接数据库
DataBaselink *DataBaselink::getDataBase()
{
    if(!userDB){
        userDB=new DataBaselink;
    }
    return userDB;
}

//聊天信息记录
bool DataBaselink::chatHistory(const QString &chathistory)
{
     QSqlQuery query;
     query.exec("CREATE TABLE historydb ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "historyline VARCHAR(50) , "
                        "data NTEXT)");
                        //创建一个historydb表

     //将输入chathistory的数据插入表中；
     query.prepare("INSERT INTO historydb (historyline,data) "
                   "VALUES (?, ?)");
       query.addBindValue("historyLine");
       query.addBindValue(chathistory);


     bool ok = query.exec(); //加入库中
     if(!ok){
         qDebug() << "Fail add chatHistory  : " << query.lastError().text();
     }

     return ok;


}
//查找聊天记录
QString DataBaselink::queryChatMessage()
{

    QString  strChatHistroy; //历史记录数据
    QSqlQuery query;


    query.exec("select *from historydb where historyline='historyLine' ");

    while(query.next())
    {
         if((query.value(1).toString())=="historyLine")
         {
             //QSqlRecord myrecord = query.record();
             //strChatHistroy=query.value(myrecord.indexOf("chathistory")).toString();
             strChatHistroy=query.value(2).toString();

         }
         else
         {
             strChatHistroy="NULL";
         }



    }

    qDebug() <<"数据为"<<strChatHistroy<<endl;

    if(strChatHistroy==" ")
    {
        QMessageBox::information(NULL,"提示","数据为空",QMessageBox::Ok);
         return "NULL";
    }

    return strChatHistroy;

}


//注册账号信息
bool DataBaselink::addRegiserUser(const QString &username, const QString &password)
{


    //创建一个关联的QSqlQuery对象
    QSqlQuery  query;

    query.exec("CREATE TABLE userdb ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "Username VARCHAR(50) , "
                       "Password VARCHAR(50))");
                       //创建一个userdb表
   //将输入username、password的数据插入表中；
    query.prepare("INSERT INTO userdb (Username,Password) "
                  "VALUES (?, ?)");

    query.addBindValue(username);
    query.addBindValue(password);

    bool ok = query.exec(); //加入库中；
    if(!ok){
        qDebug() << "Fail add regiser user : " << query.lastError().text();
    }

    return ok;


}
//登录的用户名检测
bool DataBaselink::queryusername(const QString &username)
{


    QSqlQuery query;
    query.prepare("select *from userdb where Username=? ");
    query.addBindValue(username);


    bool ok=query.exec();



    if(!ok){
        qDebug() << "Fail query register username"<< query.lastError().text();
        return false;

    }

    if(query.next()){
            return true;
        }
        else{
            return false;
        }


}
//登录密码检测
bool DataBaselink::querypassword(const QString &username, const QString &password)
{

    QSqlQuery query;
    query.prepare("select *from userdb where Username=? and Password=?");
    query.addBindValue(username);
    query.addBindValue(password);
     bool ok =query.exec();

    if(!ok){
        qDebug() << "Fail query register password" << query.lastError().text();
        return false;
    }



    if(query.next()){
            return true;
        }
        else{
            return false;
        }


}



DataBaselink::DataBaselink(QObject *parent) : QObject(parent)
{

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath()+"/user.dat");

    if (!db.open())
    {
        QMessageBox::warning(0, QObject::tr("Database Error"),
                             db.lastError().text());
    }
    else{
        qDebug() <<"数据库连接成功"<<endl;
    }



}
