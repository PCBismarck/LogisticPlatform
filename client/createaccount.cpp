#include "createaccount.h"
#include "ui_createaccount.h"

createAccount::createAccount(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::createAccount)
{
    ui->setupUi(this);
    client = new QTcpSocket;
    //client->connectToHost("127.0.0.1" , 1234);//链接到主机
    connect(client , &QTcpSocket::readyRead , this , &createAccount::recv_message);
}

createAccount::~createAccount()
{
    delete ui;
}

void createAccount::on_pushButton_go_back_clicked()
{
    this->hide();
    emit back_to_login();
}


void createAccount::on_pushButton_create_clicked()
{
    QString account = ui->lineEdit_account->text();
    QString pwd = ui->lineEdit_password->text();
    QString name = ui->lineEdit_name->text();
    QString phone = ui->lineEdit_phone_num->text();
    QString address = ui->lineEdit_address->text();

    if(account.size() == 0 || pwd.size() ==0 || name.size() == 0 || phone.size() == 0 || address.size() == 0)
    {
        QMessageBox::warning(this, tr("错误"),
                              tr("缺少参数"),
                              QMessageBox::Yes);
        return;
    }
    if(pwd.size() < 6 || pwd.size() > 30)
    {
        QMessageBox::warning(this, tr("错误"),
                              tr("密码长度至少为6且不超过30"),
                              QMessageBox::Yes);
        return;
    }
    if(pwd != ui->lineEdit_password_ensure->text())
    {
        QMessageBox::warning(this, tr("错误"),
                              tr("两次密码不一致"),
                              QMessageBox::Yes);
        return;
    }
    QString msg;
    msg.append("21").append(account).append("\n").append(pwd).append("\n").append(name)\
            .append("\n").append(phone).append("\n").append(address);
    client->connectToHost("127.0.0.1" , 1234);
    client->write(msg.toLatin1() , msg.size() + sizeof(char));//发送注册请求
    qDebug() << msg;

}
void createAccount::recv_show()
{
    this->show();
}

void createAccount::recv_message()
{
    char buf[MAXLENGTH];
    client->read(buf , MAXLENGTH);
    qDebug() << buf;
    if(buf[0] == '2' && buf[1] == '1' && buf[2] == '1')
    {
        QMessageBox::information(this, tr("注册成功"),
                              tr("请返回登录界面进行登录"),
                              QMessageBox::Yes);
    }
    else
    {
        QMessageBox::warning(this, tr("注册失败"),
                              tr("账号已存在或网络环境出错"),
                              QMessageBox::Yes);
    }
    ui->lineEdit_account->clear();
    ui->lineEdit_address->clear();
    ui->lineEdit_name->clear();
    ui->lineEdit_password->clear();
    ui->lineEdit_password_ensure->clear();
    ui->lineEdit_phone_num->clear();
}
