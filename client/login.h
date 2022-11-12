#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "mainwindow.h"
//#include "myclass.h"
#include <QtDebug>
#include <QMessageBox>
#include <QtNetwork/qtcpsocket.h>
#include "number.h"


namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

    void clear_all();//清空界面

private slots:
    void on_pushButton_login_clicked();//点击登录按钮

    void on_pushButton_user_clicked();//点击选择用户身份

    void on_pushButton_courier_clicked();//点击选择快递员身份

    void on_pushButton_admin_clicked();//点击选择管理员身份

    void recv_message();//接收到信息

    void on_pushButton_create_account_clicked();//点击注册

    void recv_show();//接到显示页面请求

signals:
    void show_user_interface(QString rbuf);//显示用户页面
    void show_courier_interface(QString rbuf);//显示快递员界面
    void show_admin_interface(QString rbuf);//显示管理员界面
    void show_create_account_interface();//显示注册界面

private:
    Ui::login *ui;  //ui类
    QTcpSocket *client;//用于tcp连接
};

#endif // LOGIN_H
