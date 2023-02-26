#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QDialog>
#include <QTcpServer>
#include <QFile>
#include <QTime>
#include<QString>

namespace Ui {
class TcpServer;
}

class TcpServer : public QDialog
{
    Q_OBJECT

public:
    explicit TcpServer(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);

    void refused();  //拒绝接收文件函数

    void initServer(); //初始化服务器函数

  signals:
    void sendFileName(QString fileName); //发送文件名信号

protected:
    void closeEvent(QCloseEvent *);



    ~TcpServer();

private slots:
    void on_serverOpenBtn_clicked();

    void on_serverSendBtn_clicked();

    void on_serverCloseBtn_clicked();

    void sendMessage(); //发送数据

    void updateClientProgress(qint64 numBytes); //更新进度条

private:
    Ui::TcpServer *ui;
    QTcpServer *tcpServer; //TCP对象
    qint16 tcpPort; //tcp的端口号
    QString fileName;
    QString theFileName;
    QFile *localFile; //待发送的文件

    qint64 TotalBytes;  //发送数据总大小
    qint64 bytesWritten;  //已发送数据大小
    qint64 bytesToWrite;  //剩余数据大小
    qint64 loadSize;  //每次发送数据的大小
    QByteArray outBlock;//缓存一次发送的数据
    QTcpSocket *clientConnection; //TCP连接套接字
    QTime time;//计时器
};

#endif // TCPSERVER_H
