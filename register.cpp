#include "register.h"
#include "ui_register.h"
#include"Qpainter"
#include"mainwidget.h"
#include <QMessageBox>
#include <QSqlQuery>
#include<QDebug>
#include <QtSql>
#include<QSqlDatabase>
#include <QString>
#include <QDebug>
#include"databaselink.h"




Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    this->setFixedSize(560,400);

}
//绘制背景图
void Register::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/bluebackground.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);



}


Register::~Register()
{
    delete ui;
}

//返回登录界面
void Register::on_backBtn_clicked()
{
    this->close();
    mainWidget * mainlogin=new mainWidget;
    mainlogin->show();


}





//注册账号信息
void Register::on_registerBtn_clicked()
{
    QString reg_userName=ui->userNameEdit->text();
    QString reg_password=ui->passwordEdit->text();
    QString verifyPassword=ui->verifyPasswordEdit->text();

    DataBaselink *userDB=DataBaselink::getDataBase();


    //判断账号和密码是否为空
    if((reg_userName!="")&&(reg_password!="")&&(verifyPassword!=""))
    {

        //判断用户名在数据库中是否存在
        if(userDB->queryusername(reg_userName))
        {
            QMessageBox::information(NULL,"注册","用户名已注册",QMessageBox::Ok);
            return;
        }

        else
        {


            if(reg_password!=verifyPassword)
            {
                ui->passwordEdit->setStyleSheet("color: rgb(255, 0, 0)");
                QMessageBox::warning(0,tr("错误"),tr("两次密码不一致"), QMessageBox::Ok);
            }

            else if(reg_password==verifyPassword)
            {

                //打开数据库
                bool ok = userDB->addRegiserUser(reg_userName,reg_password);
                if(!ok){
                    QMessageBox::information(NULL,"注册","注册失败",QMessageBox::Ok);
                }else{
                    QMessageBox::information(NULL,"注册","注册成功",QMessageBox::Ok);
                      this->close();
                    mainWidget * mainlogin=new mainWidget;
                    mainlogin->show();

                }

            }

        }
    }
    else if(reg_userName==" ")
    {
        QMessageBox::warning(0,tr("错误"),tr("账号不能为空"), QMessageBox::Ok);
    }
    else if(reg_password==" ")
    {
        QMessageBox::warning(0,tr("错误"),tr("密码不能为空"), QMessageBox::Ok);
    }


}
