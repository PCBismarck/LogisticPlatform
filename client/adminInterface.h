#ifndef ADMININTERFACE_H
#define ADMININTERFACE_H

#include <QWidget>
#include "myclass.h"
#include <QMessageBox>
#include <QTcpSocket>

namespace Ui {
class adminInterface;
}

class adminInterface : public QWidget
{
    Q_OBJECT

public:
    explicit adminInterface(QWidget *parent = nullptr);
    ~adminInterface();

    void clear_current_parcel_info();//清理当前admin类内的快递信息

    void recv_parcel_data();//接收快递信息

    void recv_person_data();//接收个人数据

    void show_parcel_result(QVector<parcel *> result);//显示快递查询结果

    void refresh_combobox_courier();//刷新分配快递页面的快递员选择下拉栏

    void refresh_assign_page();//刷新分配快递页面

private slots:
    void recv_show(QString rbuf);//显示本窗口

    void on_pushButton_show_all_clicked();//查询全部快递

    void on_pushButton_by_keyword_clicked();//点击按条件查询快递

    void on_checkBox_by_user_stateChanged(int arg1);//查询个人信息点击选择用户

    void on_checkBox_by_courier_stateChanged(int arg1);//查询个人信息点击选择快递员

    void on_pushButton_inquiry_person_info_clicked();//点击查询个人信息

    void on_pushButton_reset_input_clicked();//点击查询页面的重置输入按钮

    void on_Button_log_out_clicked();//点击登出

    void on_Button_assign_parcel_clicked();//转到快递分配页

    void on_Button_info_inquiry_clicked();//转到信息查询页

    void on_Button_courier_manage_clicked();//转到快递员管理页

    void on_pushButton_create_account_clicked();//点击创建快递员账号

    void on_pushButton_cancel_clicked();//点击禁用快递员账号

    void on_pushButton_assign_clicked();//点击分配快递

signals:
    void back_to_login();//返回登录页面

private:
    Ui::adminInterface *ui;//ui
    admin a;        //管理员数据存储类admin
    QTcpSocket *client;//用于tcp连接
};

#endif // ADMININTERFACE_H
