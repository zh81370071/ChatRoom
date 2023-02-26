#include "mainwidget.h"
#include "ui_mainwidget.h"
#include"Qpainter"
#include"register.h"
#include<QSqlQuery>
#include"widget.h"
#include<QMessageBox>
#include<QSqlDatabase>
#include"databaselink.h"




mainWidget::mainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainWidget)
{
    ui->setupUi(this);
    this->setFixedSize(560,400);


}

void mainWidget::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/bluebackground.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);



}

mainWidget::~mainWidget()
{
    delete ui;
}

void mainWidget::on_exitBtn_clicked()
{
    this->close();
}
//启动注册界面
void mainWidget::on_registerBtn_clicked()
{

    reg=new Register;
    this->hide();
    reg->show();

}

//登录
void mainWidget::on_loginBtn_clicked()
{
    bool flag;
    //获取输入的用户名和密码
    QString userName=ui->username->text();
    QString passwd=ui->password->text();
    DataBaselink *userDB=DataBaselink::getDataBase();
    flag=userDB->querypassword(userName,passwd);
    qDebug()<<flag;

    //检测用户名和密码

    if(userDB->queryusername(userName))
    {
            if(flag){
                this->hide();
                Widget *w=new Widget;
               w->show();
            }
            else
            {
            QMessageBox::information(this,("提示"),("密码错误"),QMessageBox::Ok);
            return;
            }

    }
    else
    {
           QMessageBox::information(this,("提示"),("用户名错误或不存在"),QMessageBox::Ok);
           return;
    }

}
