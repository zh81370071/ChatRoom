#ifndef DATABASELINK_H
#define DATABASELINK_H

#include <QObject>

class DataBaselink : public QObject
{
    Q_OBJECT
public:
      //单例模式，设置一个数据库连接
    static DataBaselink* getDataBase();
    bool addRegiserUser(const QString &username,const QString &password); //数据库记录注册的账号信息
    bool queryusername(const QString &username);
    bool querypassword(const QString &username,const QString &password);
    bool chatHistory(const QString  &chathistory);  //数据库保存历史记录
    QString  queryChatMessage(); //查找聊天记录

signals:

public slots:

    //构造函数私有化
private:
    explicit DataBaselink(QObject *parent = nullptr);
     static DataBaselink* userDB;

};



#endif // DATABASELINK_H
