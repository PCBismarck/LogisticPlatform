#ifndef CREATEACCOUNT_H
#define CREATEACCOUNT_H

#include <QWidget>
#include <QMessageBox>
#include <QTcpSocket>
#include "number.h"

namespace Ui {
class createAccount;
}

class createAccount : public QWidget
{
    Q_OBJECT

public:
    explicit createAccount(QWidget *parent = nullptr);
    ~createAccount();

private slots:
    void on_pushButton_go_back_clicked();//点击返回

    void on_pushButton_create_clicked();//点击创建

    void recv_show();//接到信息显示窗口

    void recv_message();//接到通信信息

signals:
    void back_to_login();//发出信号返回登录页面

private:
    Ui::createAccount *ui;//ui
    QTcpSocket *client;//用于tcp连接
};

#endif // CREATEACCOUNT_H
