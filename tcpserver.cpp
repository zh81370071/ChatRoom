#include "tcpserver.h"
#include "ui_tcpserver.h"
#include<Qpainter>
#include <QTcpSocket>
#include <QFileDialog>
#include <QMessageBox>

//using namespace std::tr1;




TcpServer::TcpServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    this->setFixedSize(500,300);
    tcpServer=new QTcpServer(this);

    tcpPort=6688;
    //connect(TcpServer,&TcpServer::newConnection,this,&TcpServer::sendMessage);

    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(sendMessage())); //当有客服端连接时，触发连接信号，启动发送数据函数
    initServer(); //初始化服务器
}



void TcpServer::closeEvent(QCloseEvent *)
{
    on_serverCloseBtn_clicked();
}

//绘制背景图
void TcpServer::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/bluebackground.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);



}

void TcpServer::refused()
{
       //当对方拒绝接收文件，关闭服务器
        tcpServer->close();
        ui->serverStatusLabel->setText(tr("对方拒绝接收文件"));
}

//服务器初始化
void TcpServer::initServer()
{
    loadSize=64*1024;
    TotalBytes=0;
    bytesWritten=0;
    bytesToWrite=0;
    ui->serverStatusLabel->setText("选择待发送的文件");
    ui->progressBar->reset();
    ui->serverOpenBtn->setEnabled(true);
    ui->serverSendBtn->setEnabled(false);
    tcpServer->close();


}

void TcpServer::updateClientProgress(qint64 numBytes)

{
    qApp->processEvents(); //防止传输文件时，界面冻结
    if(bytesToWrite > 0){
        outBlock = localFile->read(qMin(bytesToWrite,loadSize));  //数据流读入一次传输的文件数据
        bytesToWrite -= (int)clientConnection->write(outBlock);  //套接字写数据流中的缓存，bytesToWrite计算剩余需要发送的数据
        outBlock.resize(0);
    }
    else{
        localFile->close();
    }
    ui->progressBar->setMaximum(TotalBytes);
    ui->progressBar->setValue(bytesWritten);


    float useTime = time.elapsed();  //elapsed获得耗费的时间ms
    double speed = bytesWritten / useTime;
    ui->serverStatusLabel->setText(tr("已发送 %1MB (%2MB/s) \n共%3MB 已用时:%4秒\n估计剩余时间：%5秒")
                                   .arg(bytesWritten / (1024*1024))//已发送
                                   .arg(speed*1000/(1024*1024),0,'f',2)//速度，毫秒换成秒
                                   .arg(TotalBytes / (1024 * 1024))//总大小
                                   .arg(useTime/1000,0,'f',0)//用时
                                   .arg(TotalBytes/speed/1000 - useTime/1000,0,'f',0));//剩余时间

    //判断文件是否全部发送完
    if(bytesWritten == TotalBytes)
        ui->serverStatusLabel->setText(tr("传送文件 %1 成功").arg(theFileName));


}




TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::on_serverOpenBtn_clicked()
{
       fileName = QFileDialog::getOpenFileName(this);  //获得文件名（含文件地址信息）
        if(!fileName.isEmpty())
        {
            theFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1); //获取文件地址后的文件名字
            ui->serverStatusLabel->setText(tr("要传送的文件为：%1 ").arg(theFileName));
            ui->serverSendBtn->setEnabled(true);
            ui->serverOpenBtn->setEnabled(false);
        }
}

void TcpServer::on_serverSendBtn_clicked()
{
    if(!tcpServer->listen(QHostAddress::Any,tcpPort))//开始监听，//任意地址
       {
           qDebug() << tcpServer->errorString();
           close();
           return;
       }

       ui->serverStatusLabel->setText(tr("等待对方接收中"));
       emit sendFileName(theFileName);
}

void TcpServer::on_serverCloseBtn_clicked()
{
    //关闭监听，关闭服务器，终止连接和套接字,关闭对话框
    if(tcpServer->isListening())
        {
            tcpServer->close();
            clientConnection->abort();
        }
        this->close();
}

//发送数据文件头结构
void TcpServer::sendMessage()
{
    ui->serverSendBtn->setEnabled(false);
    clientConnection=tcpServer->nextPendingConnection(); //获取连接过来的套接字
    //一旦有已发送的数据信号，就触发更新进度条的函数
    connect(clientConnection,SIGNAL(bytesWritten(qint64)),this,SLOT(updateClientProgress(qint64)));
            ui->serverStatusLabel->setText(tr("开始传送文件 %1 ！").arg(theFileName));
    localFile=new QFile(fileName); //创建待发送的文件
    if(!localFile->open((QFile::ReadOnly)))
    {
        QMessageBox::warning(this,tr("提醒"),tr("无法读取文件 %1:\n%2").arg(fileName).arg(localFile->errorString()));
        return;
    }
    TotalBytes = localFile->size(); //文件大小

    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);

    sendOut.setVersion(QDataStream::Qt_4_6);  //设置数据流版本，cs需要相同
    time.start();  //开始计时
    QString currentFile = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1); //返回最后“/”后的字符串，文件名
    sendOut<<qint64(0)<<qint64(0)<<currentFile; //数据大小，文件名大小，文件名
    TotalBytes += outBlock.size(); //总大小=总大小信息+文件名大小信息+文件名+文件大小
    sendOut.device()->seek(0);
    sendOut<<TotalBytes<<qint64((outBlock.size()-sizeof(qint64)*2));
    bytesToWrite = TotalBytes - clientConnection->write(outBlock); //发送文件头结构后的剩余数据大小
    outBlock.resize(0);

}
