#include "tcpclient.h"
#include "ui_tcpclient.h"
#include<Qpainter>
#include <QTcpSocket>
#include <QDebug>
#include <QMessageBox>
#include<QTime>



TcpClient::TcpClient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    this->setFixedSize(500,300);
    TotalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;

    tcpClient = new QTcpSocket(this);  //创建客户端套接字
    tcpPort = 6688;
    //当数据达到套接字时，触发读取的信号进行读数据
    connect(tcpClient, SIGNAL(readyRead()), this, SLOT(readMessage()));

    //当发生错误时，触发error信号
    connect(tcpClient, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SLOT(displayError(QAbstractSocket::SocketError)));



}


void TcpClient::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/bluebackground.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);


}

//设置与服务器的新连接
void TcpClient::newConnect()
{
    blockSize = 0;
    tcpClient->abort();  //终止已经有的连接
    tcpClient->connectToHost(hostAddress, tcpPort);  //连接到指定ip地址和端口的主机
    time.start();
}

//设置指定IP地址，服务器IP
void TcpClient::setHostAddress(QHostAddress address)
{
       hostAddress = address;
       newConnect(); //与指定IP地址连接

}

//读取信息
void TcpClient::readMessage()
{
    QDataStream in(tcpClient);  //从TCP套接字接口读数据
    in.setVersion(QDataStream::Qt_4_6);

    float useTime = time.elapsed(); //消耗的时间


      //判断收到的数据是否为文件头结构
    if (bytesReceived <= sizeof(qint64)*2)
    {
           //判断tcpClient->bytesAvailable()返回等待读取的传入字节数是否大于文件头结构
        if ((tcpClient->bytesAvailable()
             >= sizeof(qint64)*2) && (fileNameSize == 0))
        {
            in>>TotalBytes>>fileNameSize;
            bytesReceived += sizeof(qint64)*2;
        }
        //判断待读取的字节数是否大于文件名大小，即得到文件名
        if((tcpClient->bytesAvailable() >= fileNameSize) && (fileNameSize != 0))
        {
            in>>fileName;
            bytesReceived +=fileNameSize;

            if(!localFile->open(QFile::WriteOnly)){
                QMessageBox::warning(this,tr("警告"),tr("无法读取 %1:\n%2.")
                                     .arg(fileName).arg(localFile->errorString()));
                return;
            }
        }
        else
        {
            return;
        }
    }
    //已经收到了文件头结构，判断是否收到全部数据
    if (bytesReceived < TotalBytes) {
        bytesReceived += tcpClient->bytesAvailable();
        inBlock = tcpClient->readAll(); //读取剩余的数据到缓冲区
        localFile->write(inBlock);  //写入文件中
        inBlock.resize(0);
    }

    ui->progressBar->setMaximum(TotalBytes);
    ui->progressBar->setValue(bytesReceived);

      //接收速度
    double speed = bytesReceived / useTime;

    ui->tcpClientStatusLabel->setText(tr("已接收到 %1MB (%2MB/s) "
                                         "\n共%3MB 已经用时:%4秒\n剩余时间%5秒")
                                      .arg(bytesReceived / (1024*1024))
                                      .arg(speed*1000/(1024*1024),0,'f',2)
                                      .arg(TotalBytes / (1024 * 1024))
                                      .arg(useTime/1000,0,'f',0)
                                      .arg(TotalBytes/speed/1000 - useTime/1000,0,'f',0));
     //当接收完数据后
    if(bytesReceived == TotalBytes)
    {
        //关闭文件，关闭套接字接口
        localFile->close();
        tcpClient->close();
        ui->tcpClientStatusLabel->setText(tr("接收到 %1 成功").arg(fileName));
    }



}

//错误处理
void TcpClient::displayError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
        {
        //RemoteHostClosedError为远程主机关闭了连接时发出的错误信号
        case QAbstractSocket::RemoteHostClosedError : break;

         default : qDebug() << tcpClient->errorString();
        }
}



TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::closeEvent(QEvent *e)
{
    on_tcpClientCloseBtn_clicked();
}

void TcpClient::on_tcpClientCancelBtn_clicked()
{
           tcpClient->abort();  //终止TCP连接
           //关闭文件
           if (localFile->isOpen())
            localFile->close();

}

void TcpClient::on_tcpClientCloseBtn_clicked()
{
           tcpClient->abort();
        if (localFile->isOpen())
            localFile->close();
        close();
}
