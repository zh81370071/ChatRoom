#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include<QSqlDatabase>
#include"register.h"

namespace Ui {
class mainWidget;
}

class mainWidget : public QWidget
{
   Q_OBJECT

public:
    explicit mainWidget(QWidget *parent = 0);
     void paintEvent(QPaintEvent *);

    ~mainWidget();

private slots:
     void on_exitBtn_clicked();

     void on_registerBtn_clicked();

     void on_loginBtn_clicked();

private:
    Ui::mainWidget *ui;
    Register *reg; //注册
};

#endif // MAINWIDGET_H
