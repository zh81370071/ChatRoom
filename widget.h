#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QString>
#include<QUdpSocket>
#include<QTcpServer>
#include"tcpserver.h"
#include <QTextCharFormat>

namespace Ui {
class Widget;
}

// 枚举变量标志信息的类型，分别为消息，新用户加入，用户退出，文件名，拒绝接受文件
enum MessageType{Message, newUser, userLeft, FileName, Refuse};


class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    ~Widget();
protected:
    //新用户加入处理函数，用户离开处理函数，发生数据信息函数
    void newUserFunc(QString userName,QString localHostName, QString ipAddress);
    void userLeftFunc(QString userName,QString localHostName, QString time);
    void sendMessageFunc(MessageType type, QString serverAddress="");

    void closeEvent(QCloseEvent *);//关闭事件

    void hasPendingFile(QString userName, QString serverAddress,
                        QString clientAddress, QString fileName); //判断是否接收文件函数

    bool saveFile(const QString& fileName); //保持聊天记录






    //获得IP，用户名，获得数据信息函数
    QString getIP();
    QString getUserName();
    QString getMessage();


private:
    Ui::Widget *ui;
    QUdpSocket *udpSocket;
    qint16 port; //端口号最大为65535
    QString fileName; //文件名
    TcpServer *server; //服务器对象
    QColor color;

private slots:


    void processPendingDatagrams();  //接收数据报
    void getFileName(QString);  //获取文件名函数
    void on_sendButton_clicked();
    void on_exitButton_clicked();
    void on_sendToolButton_clicked();
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_sizeComboBox_currentIndexChanged(const QString &arg1);
    void on_colorToolButton_2_clicked();
    void on_saveToolButton_clicked();
    void on_clearToolButton_clicked();
    void on_searchToolButton_clicked();
};

#endif // WIDGET_H
