#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QDialog>
#include<QTcpSocket>
#include<QTime>
#include <QHostAddress>
#include <QFile>

namespace Ui {
class TcpClient;
}

class TcpClient : public QDialog
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);

    void setHostAddress(QHostAddress address);  //设置指定IP地址，服务器IP，并连接服务器
    //设置文件名
    void setFileName(QString fileName){localFile = new QFile(fileName);} //创建本地文件，并命名为收到的文件名


    ~TcpClient();

protected:
    void closeEvent(QEvent *e);

private slots:
    void on_tcpClientCancelBtn_clicked();

    void on_tcpClientCloseBtn_clicked();

    void newConnect(); //信号连接

    void readMessage(); //读取信息

    void displayError(QAbstractSocket::SocketError);

private:
    Ui::TcpClient *ui;
    QTcpSocket *tcpClient; //tcp套接字对象
    quint16 blockSize;  //存放数据的大小
    QHostAddress hostAddress;  //主机地址
    qint16 tcpPort;  //端口号


    qint64 TotalBytes;  //数据总大小
    qint64 bytesReceived;  //接收到的数据大小
    qint64 bytesToReceive;  //待接收的数据大小
    qint64 fileNameSize;   //文件名大小
    QString fileName;      //文件名
    QFile *localFile;    //本地文件
    QByteArray inBlock;   //数据缓冲区
    QTime time;
};

#endif // TCPCLIENT_H
