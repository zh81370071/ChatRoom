#include "historywidget.h"
#include "ui_historywidget.h"
#include"databaselink.h"
#include"QDebug"
#include<QMessageBox>

historyWidget::historyWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::historyWidget)
{
    ui->setupUi(this);
    this->setFixedSize(600,700);




}

void historyWidget::showMessage()
{
    DataBaselink* userDB=DataBaselink::getDataBase();
   QString chatData= userDB->queryChatMessage();

   if(chatData==" ")
   {
     QMessageBox::information(NULL,"提示","数据为空",QMessageBox::Ok);

   }

   ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
   ui->textBrowser->append(chatData);//消息输出

}




historyWidget::~historyWidget()
{
    delete ui;
}

void historyWidget::on_returnButton_clicked()
{
    this->close();
}
