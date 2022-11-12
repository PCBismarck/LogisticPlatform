#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QWidget>
#include "myclass.h"
#include <QTcpSocket>
#include "number.h"
#include <QMessageBox>
#include <QTime>

namespace Ui {
class userInterface;
}

class userInterface : public QWidget
{
    Q_OBJECT

public:
    explicit userInterface(QWidget *parent = nullptr);
    ~userInterface();

    void refresh_main_page();//刷新主页面

    void refresh_inquiry_parcel_page();//刷新快递查询页面

    void refresh_send_parcel_page();//刷新快递发送页面

    void refresh_person_info_page();//刷新个人信息页面

    void refresh_balance();//刷新余额显示

    double get_parcel_cost(int type , double num);//获取当前输入下的快递价格

    void change_cost_show();//改变在快递价格显示框中显示的价格

    void show_parcel_result(QVector<parcel*> result);//显示快递查询的结果

private slots:
    void recv_show(QString rbuf);//显示本窗口

    void recv_message();//接收用户快递数据信息

    void on_pushButton_recv_clicked();//点击接收

    void on_Button_main_clicked();//点击切换主页面

    void on_Button_info_clicked();//点击切换个人信息页

    void on_Button_send_parcel_clicked();//点击切换到快递发送页

    void on_Button_inquiry_clicked();//点击切换到查询页

    void on_Button_log_out_clicked();//点击登出

    void on_pushButton_ensure_clicked();//点击确认发送快递

    void on_comboBox_select_parcel_type_currentIndexChanged(int index);//下拉栏状态改变

    void on_lineEdit_parcel_num_editingFinished();//待发送快递数量输入完毕

    void on_pushButton_charge_balance_clicked();//点击充值余额

    void on_pushButton_cahnge_pwd_clicked();//点击改变密码

    void on_pushButton_parcel_all_clicked();//点击查询我的所有快递

    void on_pushButton_parcel_send_clicked();//点击查询我发送的快递

    void on_pushButton_parcel_recv_clicked();//点击查询我接收的快递

    void on_pushButton_by_keyword_clicked();//点击按条件查询

signals:
    void back_to_login();

private:
    Ui::userInterface *ui;//ui
    user u;             //user用户数据存储类
    QTcpSocket *client;//用于tcp连接
};

#endif // USERINTERFACE_H
