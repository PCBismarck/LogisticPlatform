#include "login.h"
#include "ui_login.h"

int identity = 0;

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    client = new QTcpSocket;
    //client->connectToHost("127.0.0.1" , 1234);//链接到主机
    connect(client , &QTcpSocket::readyRead , this , &login::recv_message);
}

login::~login()
{
    delete ui;
}


void login::on_pushButton_login_clicked()
{
    QString account = ui->lineEdit_account->text();
    QString password = ui->lineEdit_password->text();
    QString msg = '1'+QString::number(identity).append(account).append('\n').append(password);
    qDebug() << msg;
    if(client->state() != QAbstractSocket::ConnectedState)
        client->connectToHost("127.0.0.1" , 1234);
    client->write(msg.toLatin1() , msg.size() + sizeof(char));//发送登录请求
}

void login::on_pushButton_user_clicked()
{
    ui->pushButton_user->setChecked(true);
    ui->pushButton_courier->setChecked(false);
    ui->pushButton_admin->setChecked(false);
    identity = 1;
}


void login::on_pushButton_courier_clicked()
{
    ui->pushButton_user->setChecked(false);
    ui->pushButton_courier->setChecked(true);
    ui->pushButton_admin->setChecked(false);
    identity = 2;
}


void login::on_pushButton_admin_clicked()
{
    ui->pushButton_user->setChecked(false);
    ui->pushButton_courier->setChecked(false);
    ui->pushButton_admin->setChecked(true);
    identity = 3;
}


void login::on_pushButton_create_account_clicked()
{
    this->hide();
    emit show_create_account_interface();
}

void login::recv_message()
{
    char recvbuf[MAXLENGTH];
    client->read(recvbuf , MAXLENGTH);
    qDebug() << "recv " << recvbuf;
    QString recv_msg = QString(QLatin1String(recvbuf));
    if(recvbuf[1] == '1')//登陆成功页面跳转
    {
        qDebug() << "success! ";
        switch (identity) {
        case 1:
            this->hide();emit show_user_interface(recv_msg);break;
        case 2:
            this->hide();emit show_courier_interface(recv_msg);break;
        case 3:
            this->hide();emit show_admin_interface(recv_msg);break;
        default:
            QMessageBox::warning(this, tr("登陆失败"),
                                  tr("请选择身份"),
                                  QMessageBox::Yes);
            break;
        }
        clear_all();
        client->close();
    }
    else
    {
       QMessageBox::warning(this, tr("登陆失败"),
                             tr("user name or password error!"),
                             QMessageBox::Yes);
       // 清空内容并定位光标
       ui->lineEdit_account->clear();
       ui->lineEdit_password->clear();
       ui->label_account->setFocus();
    }
}

void login::recv_show()
{
    this->show();
}

void login::clear_all()
{
    ui->lineEdit_account->clear();
    ui->lineEdit_password->clear();
    ui->pushButton_user->setChecked(false);
    ui->pushButton_courier->setChecked(false);
    ui->pushButton_admin->setChecked(false);
}

