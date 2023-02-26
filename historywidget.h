#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QDialog>

namespace Ui {
class historyWidget;
}

class historyWidget : public QDialog
{
    Q_OBJECT

public:
    explicit historyWidget(QWidget *parent = 0);

    void showMessage();

    ~historyWidget();

private slots:
    void on_returnButton_clicked();

private:
    Ui::historyWidget *ui;
};

#endif // HISTORYWIDGET_H
