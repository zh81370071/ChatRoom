#include "widget.h"
#include "ui_widget.h"
#include<QPainter>
#include <QUdpSocket>
#include <QHostInfo>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QNetworkInterface>
#include <QProcess>
#include<Qstring>
#include <QFileDialog>
#include <QColorDialog>
#include"tcpserver.h"
#include"tcpclient.h"
#include"databaselink.h"
#include"historywidget.h"





Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setFixedSize(1200,800);


    udpSocket=new QUdpSocket(this);  //创建UDP对象
    port=8888;
    //绑定端口，允许多个服务连接到此端口
    udpSocket->bind(port,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    //当有数据报时，就触发readyread信号，接收数据函数处理发来的数据
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    sendMessageFunc(newUser);//向外发射本地信息，让其他在线用户得到通知



    server=new TcpServer(this); //创建TCP服务器
    //服务器触发发送文件名信号，槽函数接收文件名
    connect(server, SIGNAL(sendFileName(QString)), this, SLOT(getFileName(QString)));




}

void Widget::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/bluebackground.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);



}



Widget::~Widget()
{
    delete ui;
}
//用户加入
void Widget::newUserFunc(QString userName, QString localHostName, QString ipAddress)
{
    //从用户列表中按主机名查找
    bool isEmpty  = ui->userTableWidget->findItems(localHostName, Qt::MatchExactly).isEmpty();
    if (isEmpty ) {

        QTableWidgetItem *user = new QTableWidgetItem(userName);
        QTableWidgetItem *host = new QTableWidgetItem(localHostName);
        QTableWidgetItem *ip = new QTableWidgetItem(ipAddress);

        ui->userTableWidget->insertRow(0);
        ui->userTableWidget->setItem(0,0,user);
        ui->userTableWidget->setItem(0,1,host);
        ui->userTableWidget->setItem(0,2,ip);
        ui->messageBrowser->setTextColor(Qt::green);
        ui->messageBrowser->setCurrentFont(QFont("Times New Roman",10));

        ui->messageBrowser->append(tr("%1 在线！").arg(userName));
        ui->userlabel->setText(tr("在线人数：%1").arg(ui->userTableWidget->rowCount()));

        sendMessageFunc(newUser);//该句的功能是让新来的用户也能收到其它在线用户的信息，可拥于更新自己的好友列表
    }



}
//用户离开处理函数
void Widget::userLeftFunc(QString userName, QString localHostName, QString time)
{

    int rowNum = ui->userTableWidget->findItems(localHostName, Qt::MatchExactly).first()->row();
    ui->userTableWidget->removeRow(rowNum);    //移除离开的用户
    ui->messageBrowser->setTextColor(Qt::gray);//设置文本颜色为灰色
    ui->messageBrowser->setCurrentFont(QFont("Times New Roman", 10));
    ui->messageBrowser->append(tr("%1 在 %2 离线了！").arg(userName).arg(time));
    ui->userlabel->setText(tr("在线人数：%1").arg(ui->userTableWidget->rowCount()));

}

//UDP发送数据报
void Widget::sendMessageFunc(MessageType type, QString serverAddress)
{
    QByteArray data;    //字节数组
    QDataStream out(&data, QIODevice::WriteOnly); //数据流中写入数据，即到data中

    QString localHostName = QHostInfo::localHostName();//返回主机名
    QString userName=getUserName();
    QString address = getIP();    //获得ip地址
    //将消息类型，用户名，主机名输入字节数组
    out << type <<userName<< localHostName;

    switch (type)
    {

    case Message :
        //判断输入文本是否为空，是，就给出提醒
        if (ui->messageTextEdit->toPlainText() == "") {
            QMessageBox::warning(0,tr("警告"),tr("发送内容不能为空"),QMessageBox::Ok);
            return;
        }
        out << address << getMessage();//将IP和数据报发送到字节数组；
        ui->messageBrowser->verticalScrollBar()->setValue(ui->messageBrowser->verticalScrollBar()->maximum());//设置垂直滑动条的最大值，至于顶端
        break;
    case newUser:
        out<<address;
        break;


    case FileName:
    {
        int row = ui->userTableWidget->currentRow(); //返回当前选中行
        QString clientAddress = ui->userTableWidget->item(row, 2)->text(); //选中行的用户地址
        out << address << clientAddress << fileName;
        break;
    }

    case Refuse:
        out << serverAddress;
        break;
    }
    //发送数据，QHostAddress::Broadcast为本机所在地址的广播组内所有机器，广播地址
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast,port);

}

QString Widget::getIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();//获得的地址包含ipv4和ipv6的地址

    foreach (QHostAddress address, list) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            return address.toString();
    }
    return 0;


}

QString Widget::getUserName()
{
    QStringList envVariables;  //envVariables环境变量

    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";

    QStringList environment = QProcess::systemEnvironment();

    foreach (QString string, envVariables) {

        //indexOf为返回第一个匹配list的索引,QRegExp类是用规则表达式进行模式匹配
        int index = environment.indexOf(QRegExp(string));
        if (index != -1) {
            //匹配到的索引根据=分割
            QStringList stringList = environment.at(index).split('=');
            if (stringList.size() == 2) {
                return stringList.at(1);//at(0)为文字"USERNAME."，at(1)为用户名
                break;
            }
        }
    }
    return "NULL";


}
//发送文本框中获得发送的文本
QString Widget::getMessage()
{
    QString str=ui->messageTextEdit->toHtml();
    ui->messageTextEdit->clear();
    ui->messageTextEdit->setFocus();//重新设置光标焦点
    return str;


}


//接受数据信息函数
void Widget::processPendingDatagrams()
{
    //判断是否有数据接收，数据报大小为接收数据大小
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        //将读取到的不大于datagram.size()大小数据输入到datagram.data()中
        udpSocket->readDatagram(datagram.data(), datagram.size());

        QDataStream in(&datagram, QIODevice::ReadOnly);
        int messageType;

        //获取数据类型
        in >> messageType;

        QString userName,localHostName,ipAddress,message;

        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        switch(messageType)
        {
        case Message:

            in >> userName >> localHostName >> ipAddress >> message;

            //设置文本颜色，文字大小以及输出的格式
            ui->messageBrowser->setTextColor(Qt::red);
            ui->messageBrowser->setCurrentFont(QFont("Times New Roman",12));
            ui->messageBrowser->append("( " +userName+" ) "+  time);
            ui->messageBrowser->append(message);//消息输出
            break;

        case newUser:
            in >>userName >>localHostName >>ipAddress;
            //用户加入处理函数
            newUserFunc(userName,localHostName,ipAddress);
            break;

        case userLeft:
            in >>userName >>localHostName;
            //用户离开处理函数
            userLeftFunc(userName,localHostName,time);
            break;

        case FileName:
        {

            in >> userName >> localHostName >> ipAddress; //发送方(服务端)
            QString clientAddress, fileName; //客户端地址（接收方），文件名
            in >> clientAddress >> fileName;
            hasPendingFile(userName, ipAddress, clientAddress, fileName); //判断是否接收文件
            break;
        }


        case Refuse:
        {
            in >> userName >> localHostName;
            QString serverAddress;
            in >> serverAddress;  //服务器地址
            QString ipAddress = getIP();

            if(ipAddress == serverAddress)
            {
                server->refused(); //拒绝接收文件，服务器关闭

            }
            break;
        }
      }


    }

}
//判断是否接收文件
void Widget::hasPendingFile(QString userName, QString serverAddress,QString clientAddress, QString fileName)

{
    QString ipAddress = getIP();
    //判断文件发送方的地址是否为自己的地址
        if(ipAddress == clientAddress)
        {
            int flag = QMessageBox::information(this,tr("接受文件"),tr("来自%1(%2)的文件：%3,是否接收？")
                                               .arg(userName).arg(serverAddress).arg(fileName),
                                               QMessageBox::Yes,QMessageBox::No);
            if (flag == QMessageBox::Yes) {
                QString name = QFileDialog::getSaveFileName(0,tr("保存文件"),fileName);//name为另存为的文件名
                if(!name.isEmpty())
                {
                    TcpClient *client = new TcpClient(this);
                    client->setFileName(name);    //客户端设置本地文件，并命名为name文件名
                    client->setHostAddress(QHostAddress(serverAddress));    //客户端设置服务器地址，并连接
                    client->show();
                }
            } else {
                sendMessageFunc(Refuse, serverAddress);
            }
        }


}



void Widget::on_sendButton_clicked()
{
    sendMessageFunc(Message);
}

void Widget::on_exitButton_clicked()
{
    this->close();
}

void Widget::on_sendToolButton_clicked()
{
    if(ui->userTableWidget->selectedItems().isEmpty())
    {
        QMessageBox::warning(0, tr("选择用户"),
                             tr("从用户列表中选择要发送文件的用户"), QMessageBox::Ok);
        return;
    }
    server->show();
    server->initServer();


}


//获取服务器的sendfileName信号发过来的文件名
void Widget::getFileName(QString name)
{
    fileName=name;
    sendMessageFunc(FileName);


}





void Widget::on_fontComboBox_currentFontChanged(const QFont &f)
{
    ui->messageTextEdit->setCurrentFont(f); //设置文字输入字体
    ui->messageTextEdit->setFocus();
}

void Widget::on_sizeComboBox_currentIndexChanged(const QString &arg1)
{
    ui->messageTextEdit->setFontPointSize(arg1.toDouble()); //设置字号大小
    ui->messageTextEdit->setFocus();
}

void Widget::on_colorToolButton_2_clicked()
{
    color = QColorDialog::getColor(color, this); //颜色对话框选择颜色
    if (color.isValid()) {
        ui->messageTextEdit->setTextColor(color);
        ui->messageTextEdit->setFocus();
    }
}

//保持聊天记录
void Widget::on_saveToolButton_clicked()
{
    //判断消息框里的聊天记录是否为空
    if (ui->messageBrowser->document()->isEmpty()) {
        QMessageBox::warning(0, tr("警告"), tr("聊天记录为空"), QMessageBox::Ok);
    } else
    {
        //返回用户选择的文件保存路径
        QString fileName = QFileDialog::getSaveFileName(this,tr("保存聊天记录"), tr("chatHistory"), tr("文本(*.txt);;All File(*.*)"));
        if(!fileName.isEmpty())
            saveFile(fileName); //保存记录到文本中
    }


}


bool Widget::saveFile(const QString& fileName)
{
       //创建聊天记录文件
       QFile file(fileName);
        if(!file.open(QFile::WriteOnly | QFile::Text))

        {
            QMessageBox::warning(this,tr("保存文件"),tr("无法保存文件 %1:\n %2").arg(fileName).arg(file.errorString()));
            return false;
        }
        //将聊天记录输入到文件中
        QTextStream out(&file);
        out << ui->messageBrowser->toPlainText();
        //保存在数据库中
        //ui->messageBrowser->toPlainText();

        QString str=ui->messageBrowser->toPlainText();


//        //测试存在数据
//        if(str!="")
//         {
//             QMessageBox::information(NULL,"提示","有数据",QMessageBox::Ok);
//                   qDebug()
//         }

         DataBaselink* userDB=DataBaselink::getDataBase();
         bool ok=userDB->chatHistory(str);  //数据库保存历史记录

         if(ok)
         {
             QMessageBox::information(NULL,"历史聊天记录","存储成功",QMessageBox::Ok);
         }
         else{
             QMessageBox::information(NULL,"历史聊天记录","存储失败",QMessageBox::Ok);
         }
     return true;

}

//清空消息
void Widget::on_clearToolButton_clicked()
{
    ui->messageBrowser->clear();
}


//关闭程序时发送用户离开的广播，让其他端点删除该用户
void Widget::closeEvent(QCloseEvent *e)

{
    sendMessageFunc(userLeft);
    QWidget::closeEvent(e);

}



//查看历史聊天记录
void Widget::on_searchToolButton_clicked()
{
     //显示历史记录窗口
    historyWidget *historyShow=new historyWidget;
    historyShow->showMessage();

     historyShow->show();




}
