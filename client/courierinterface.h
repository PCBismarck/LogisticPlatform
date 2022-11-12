#ifndef COURIERINTERFACE_H
#define COURIERINTERFACE_H

#include <QWidget>
#include "myclass.h"
#include <QTcpSocket>
#include <QMessageBox>

namespace Ui {
class courierInterface;
}

class courierInterface : public QWidget
{
    Q_OBJECT

public:
    explicit courierInterface(QWidget *parent = nullptr);
    ~courierInterface();

    void refresh_inquiry_parcel_page();//刷新信息查询页

    void refresh_main_page();//刷新主页

    void refresh_person_info();//刷新个人信息

    void recv_parcel_data();//获取快递员快递信息

    void show_parcel_result(QVector<parcel *> result);//显示快递查询结果

private slots:
    void recv_show(QString rbuf);//显示本窗口

    void on_Button_inquiry_clicked();//点击查询

    void on_pushButton_by_keyword_clicked();//点击按条件查询

    void on_pushButton_all_clicked();//点击显示全部快递

    void on_pushButton_pickup_all_clicked();//点击显示揽收的全部快递

    void on_Button_pick_up_clicked();//点击切换到查询页

    void on_pushButton_pickup_clicked();//点击切换到

    void on_Button_log_out_clicked();//点击登出

signals:
    void back_to_login();//发出信号回到登录界面

private:
    Ui::courierInterface *ui;//ui
    courier c;              //用于存储快递员信息的courier类
    QTcpSocket *client; //用于tcp连接
};

#endif // COURIERINTERFACE_H
