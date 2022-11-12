#include "adminInterface.h"
#include "ui_adminInterface.h"

adminInterface::adminInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::adminInterface)
{
    ui->setupUi(this);
    client = new QTcpSocket;
    ui->stackedWidget->setCurrentIndex(0);
}

adminInterface::~adminInterface()
{
    delete ui;
}

void adminInterface::recv_show(QString recv_msg)
{    int type = 0;
     if(recv_msg[0] == '1')
         type = 1;
     else if(recv_msg[0] == '2')
         type = 2;
     else if(recv_msg[0] == '3')
         type = 3;
     else if(recv_msg[0] == '4')
         type = 4;
     else if(recv_msg[0] == '5')
         type = 5;

     switch (type) {
     case 1:
     {
         QString user_name , name , password , bal_buf;
         int pos = 3;
         for(; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//user_name
             user_name.append(recv_msg[pos]);
         for(++pos ; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//name
             name.append(recv_msg[pos]);
         for(++pos ; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//password
             password.append(recv_msg[pos]);
         for(++pos ; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//balance
             bal_buf.append(recv_msg[pos]);
         ui->label_show_balance->setText(QString::number(bal_buf.toDouble() , 'f' , 2));
         ui->label_welcome->setText("欢迎使用, " + user_name + "! ");
         client->close();
         refresh_combobox_courier();
         refresh_assign_page();
     }break;

     default:
         break;
     }

    this->show();
}


void adminInterface::on_pushButton_show_all_clicked()//查询全部快递
{
    clear_current_parcel_info();
    client->abort();
    client->connectToHost("127.0.0.1" , 1234);
    QString msg;
    msg.append("300");
    client->write(msg.toLatin1() , msg.size() + sizeof(char));
    recv_parcel_data();
    show_parcel_result(a.getAllparcel());
}


void adminInterface::clear_current_parcel_info()
{
    for(QVector<parcel*>::iterator it = a.getAllparcel().begin() ; it != a.getAllparcel().end() ; ++it)
    {
        delete *it;
        *it = NULL;
    }
    a.getAllparcel().clear();
}


void adminInterface::recv_parcel_data()//接收快递数据
{
    char recvbuf[MAXLENGTH];
    client->waitForReadyRead();
    client->read(recvbuf , MAXLENGTH);
    qDebug() << "recv:" << recvbuf;
    QString msg = QString(QLatin1String(recvbuf));
    int num = msg.right(msg.size() - 2).toInt();
    qDebug() << num;
    if(msg[0] == '3')//接收快递包数据
    {
        for(int i = 0 ; i < num ; i++)
        {
            client->waitForReadyRead();
            client->read(recvbuf , MAXLENGTH);
            msg = QString(QLatin1String(recvbuf));
            qDebug() << msg;
            QStringList recv_list = msg.right(msg.size() -2).split('\n');
            QStringList::iterator it = recv_list.begin();
            parcel *new_parcel = new parcel;
            new_parcel->setParcelID(*(it++));
            new_parcel->setSender(*(it++));
            new_parcel->setReceiver(*(it++));
            new_parcel->setSendTime(*(it++));
            new_parcel->setRecieveTime(*(it++));
            new_parcel->setDescription(*(it++));
            new_parcel->setCourier(*(it++));
            new_parcel->setState((*(it++)).toInt());
            new_parcel->setType((*(it++)).toInt());
            new_parcel->setNum((*(it)).toDouble());
            a.getAllparcel().push_back(new_parcel);
        }
    }
}


void adminInterface::recv_person_data()//接收个人信息数据
{
    char recvbuf[MAXLENGTH];
    client->waitForReadyRead();
    client->read(recvbuf , MAXLENGTH);
    QString msg = QString(QLatin1String(recvbuf));
    if(recvbuf[0] == '3' && recvbuf[1] == '1' && recvbuf[2] == '0')//用户信息
    {
        ui->listWidget_show_person_result->clear();
        QStringList user_info = msg.right(msg.size() - 3).split('\n');
        QStringList items;
        QStringList::iterator it = user_info.begin();
        items << tr("用户名：").append(*(it++)) << tr("姓名：").append(*(it++)) << tr("密码：").append(*(it++))\
              << tr("电话：").append(*(it++)) << tr("余额：").append(QString::number((*(it++)).toDouble()))\
              << tr("地址：").append(*(++it));
        ui->listWidget_show_person_result->addItems(items);
    }
    else if(recvbuf[0] == '3' && recvbuf[1] == '1' && recvbuf[2] == '1')//快递员信息
    {
        ui->listWidget_show_person_result->clear();
        QStringList courier_info = msg.right(msg.size() - 3).split('\n');
        QStringList items;
        QStringList::iterator it = courier_info.begin();
        items << tr("用户名：").append(*(it)) << tr("姓名：").append(*(it++)) << tr("密码：").append(*(it++))\
              << tr("电话：").append(*(it++)) << tr("余额：").append(QString::number((*(it++)).toDouble()));
        ui->listWidget_show_person_result->addItems(items);
    }
    else//不存在
    {
        QMessageBox::warning(this, tr("查询失败"),
                              tr("该用户不存在"),
                              QMessageBox::Yes);
    }
}


void adminInterface::on_pushButton_by_keyword_clicked()//按条件查询
{
    QVector<QString> key_type;
    QVector<QString> key_word;
    if((ui->checkBox_user_name->checkState() == Qt::Checked && ui->lineEdit_user_name->text().size() == 0) ||\
       (ui->checkBox_sender->checkState() == Qt::Checked && ui->lineEdit_sender->text().size() == 0) || \
       (ui->checkBox_receiver->checkState() == Qt::Checked && ui->lineEdit_receiver->text().size() == 0) || \
       (ui->checkBox_parcel_id->checkState() == Qt::Checked && ui->lineEdit_parcel_id->text().size() == 0) || \
       (ui->checkBox_time->checkState() == Qt::Checked && ui->lineEdit_time->text().size() == 0) || \
       (ui->checkBox_courier->checkState() == Qt::Checked && ui->lineEdit_courier->text().size() == 0))
    {
        QMessageBox::warning(this, tr("错误"),
                              tr("条件不能为空"),
                              QMessageBox::Yes);
        return ;
    }
    if(ui->checkBox_user_name->checkState() == Qt::Checked)
    {
        key_type.push_back("1");
        key_word.push_back(ui->lineEdit_user_name->text());
    }
    if(ui->checkBox_parcel_id->checkState() == Qt::Checked)
    {
        key_type.push_back("2");
        key_word.push_back(ui->lineEdit_parcel_id->text());
    }
    if(ui->checkBox_time->checkState() == Qt::Checked)
    {
        key_type.push_back("3");
        key_word.push_back(ui->lineEdit_time->text());
    }
    if(ui->checkBox_sender->checkState() == Qt::Checked)
    {
        key_type.push_back("4");
        key_word.push_back(ui->lineEdit_sender->text());
    }
    if(ui->checkBox_receiver->checkState() == Qt::Checked)
    {
        key_type.push_back("5");
        key_word.push_back(ui->lineEdit_receiver->text());
    }
    if(ui->checkBox_courier->checkState() == Qt::Checked)
    {
        key_type.push_back("6");
        key_word.push_back(ui->lineEdit_courier->text());
    }
    clear_current_parcel_info();
    client->connectToHost("127.0.0.1" , 1234);
    QString msg;
    msg.append("301").append(QString::number(key_type.size()));//添加关键字数量
    for(int i = 0 ; i < key_type.size() ; ++i)//添加关键字种类和关键字
        msg.append(key_type[i]).append(key_word[i]).append("\n");
    client->write(msg.toLatin1() , msg.size() + sizeof (char));//发送请求
    recv_parcel_data();
    show_parcel_result(a.getAllparcel());
}


void adminInterface::on_checkBox_by_user_stateChanged(int arg1)//个人信息查询页面选择
{
    if(ui->checkBox_by_user->checkState() == Qt::Checked)
        ui->checkBox_by_courier->setCheckState(Qt::Unchecked);
}


void adminInterface::on_checkBox_by_courier_stateChanged(int arg1)
{
    if(ui->checkBox_by_courier->checkState() == Qt::Checked)
        ui->checkBox_by_user->setCheckState(Qt::Unchecked);
}


void adminInterface::on_pushButton_inquiry_person_info_clicked()//点击查询个人信息
{
    QString account , msg;
    if(ui->checkBox_by_user->checkState() == Qt::Checked)
    {
        account = ui->lineEdit_person_user_name->text();
        msg.append("310");
    }
    else if(ui->checkBox_by_courier->checkState() == Qt::Checked)
    {
        account = ui->lineEdit_person_courier->text();
        msg.append("311");
    }
    else
    {
        QMessageBox::warning(this, tr("错误"),
                              tr("请选择查询对象身份"),
                              QMessageBox::Yes);
        return ;
    }
    if(account.size() == 0)
    {
        QMessageBox::warning(this, tr("错误"),
                              tr("条件不能为空"),
                              QMessageBox::Yes);
        return ;
    }
    clear_current_parcel_info();
    client->connectToHost("127.0.0.1" , 1234);
    msg.append(account);
    client->write(msg.toLatin1() , msg.size() + sizeof(char));
    recv_person_data();
    ui->lineEdit_person_user_name->clear();
    ui->lineEdit_person_courier->clear();
}


void adminInterface::show_parcel_result(QVector<parcel *> result)
{
    ui->tableWidget_show_parcel_result->clearContents();
    QStringList header;
    header << "ID" << tr("发送人") << tr("接收人") << tr("快递描述")\
           << tr("发送时间") << tr("接收时间") << tr("快递员");
    ui->tableWidget_show_parcel_result->setColumnCount(7);
    ui->tableWidget_show_parcel_result->setRowCount(result.size());
    ui->tableWidget_show_parcel_result->setHorizontalHeaderLabels(header);
    ui->tableWidget_show_parcel_result->setColumnWidth(0 , 60);
    ui->tableWidget_show_parcel_result->setColumnWidth(1 , 60);
    ui->tableWidget_show_parcel_result->setColumnWidth(2 , 60);
    ui->tableWidget_show_parcel_result->setColumnWidth(6 , 60);
    //ui->tableWidget_show_myparcel->horizontalHeader()->setSectionResizeMode(0 , QHeaderView::Fixed);
    for(int i = 3 ; i < 6 ; ++i)//为后几个项目设置宽度方式
        ui->tableWidget_show_parcel_result->horizontalHeader()->setSectionResizeMode(i , QHeaderView::Stretch);
    for(int row = 0 ; row < result.size() ; ++row)//每行添加item
    {
        ui->tableWidget_show_parcel_result->setItem(row , 0 ,new QTableWidgetItem(result[row]->getParcelID()));
        ui->tableWidget_show_parcel_result->setItem(row , 1 ,new QTableWidgetItem(result[row]->getSender()));
        ui->tableWidget_show_parcel_result->setItem(row , 2 ,new QTableWidgetItem(result[row]->getReceiver()));
        ui->tableWidget_show_parcel_result->setItem(row , 3 ,new QTableWidgetItem(result[row]->getDescription()));
        ui->tableWidget_show_parcel_result->setItem(row , 4 ,new QTableWidgetItem(result[row]->getSendTime()));
        ui->tableWidget_show_parcel_result->setItem(row , 5 ,new QTableWidgetItem(result[row]->getReceiveTime()));
        ui->tableWidget_show_parcel_result->setItem(row , 6 ,new QTableWidgetItem(result[row]->getCourier()));
        for(int col = 0 ; col < 7 ; ++col)
        {
            ui->tableWidget_show_parcel_result->item(row , col)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_show_parcel_result->item(row , col)->setFlags(Qt::ItemIsEnabled);
        }
    }
}


void adminInterface::on_pushButton_reset_input_clicked()
{
    ui->lineEdit_user_name->clear();
    ui->lineEdit_courier->clear();
    ui->lineEdit_time->clear();
    ui->lineEdit_sender->clear();
    ui->lineEdit_receiver->clear();
    ui->lineEdit_parcel_id->clear();
}


void adminInterface::on_Button_log_out_clicked()//登出
{
    this->hide();
    clear_current_parcel_info();
    //clear_all();
    client->close();
    emit back_to_login();
}


void adminInterface::on_Button_assign_parcel_clicked()//转到快递分配页
{
    refresh_assign_page();
    ui->stackedWidget->setCurrentIndex(0);
}


void adminInterface::on_Button_info_inquiry_clicked()//转到信息查询页
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->listWidget_show_person_result->clear();
}


void adminInterface::on_Button_courier_manage_clicked()//转到快递员管理页
{
    ui->stackedWidget->setCurrentIndex(2);
}


void adminInterface::on_pushButton_create_account_clicked()//点击创建快递员账号
{
    QString name , pwd , phone;
    name = ui->lineEdit_account_create->text();
    pwd = ui->lineEdit_password_create->text();
    phone = ui->lineEdit_phone_create->text();
    if(name.size() == 0 || pwd.size() == 0 || phone.size() == 0)
    {
        QMessageBox::warning(this, tr("错误"),
                             tr("缺少参数"),
                             QMessageBox::Yes);
        return ;
    }
    QString msg;
    msg.append("22").append(name).append("\n").append(pwd).append("\n").append(phone);
    client->abort();
    client->connectToHost("127.0.0.1" , 1234);
    client->write(msg.toLatin1() , msg.size() + sizeof(char));//发送注册信息
    //等待反馈
    char recvbuf[MAXLENGTH];
    client->waitForReadyRead();
    client->read(recvbuf , MAXLENGTH);
    if(recvbuf[0] == '2' && recvbuf[1] == '1' && recvbuf[2] == '2')//注册成功
    {
        QMessageBox::information(this, tr("成功"),
                             tr("已成功创建快递员账号"),
                             QMessageBox::Yes);
        ui->lineEdit_account_create->clear();
        ui->lineEdit_password_create->clear();
        ui->lineEdit_phone_create->clear();
        refresh_combobox_courier();
    }
    else
    {
        QMessageBox::warning(this, tr("错误"),
                             tr("创建快递员账号失败"),
                             QMessageBox::Yes);
    }
}


void adminInterface::on_pushButton_cancel_clicked()//点击禁用快递员账号
{
    QString account = ui->lineEdit_account_cancel->text();
    if(account.size() == 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("缺少参数"), QMessageBox::Yes);
        return ;
    }
    QString msg;
    msg.append("43").append(account);
    client->abort();
    client->connectToHost("127.0.0.1" , 1234);
    client->write(msg.toLatin1() , msg.size() + sizeof(char));//发送禁用信息
    //等待反馈
    char recvbuf[MAXLENGTH];
    client->waitForReadyRead();
    client->read(recvbuf , MAXLENGTH);
    if(recvbuf[0] == '4' && recvbuf[1] == '1' && recvbuf[2] == '3')//禁用成功
    {
        QMessageBox::information(this, tr("成功"),
                             tr("快递员账号已禁用"),
                             QMessageBox::Yes);
        ui->lineEdit_account_cancel->clear();
        refresh_combobox_courier();
    }
    else
    {
        QMessageBox::warning(this, tr("错误"),
                             tr("禁用快递员账号失败"),
                             QMessageBox::Yes);
    }
}

void adminInterface::refresh_combobox_courier()
{
    ui->comboBox_select_courier->clear();
    client->abort();
    client->connectToHost("127.0.0.1" , 1234);
    QString msg;
    msg.append("82");
    client->write(msg.toLatin1() , msg.size() + sizeof(char));//发送快递员索引请求
    //等待反馈
    char recvbuf[MAXLENGTH];
    client->waitForReadyRead();
    client->read(recvbuf , MAXLENGTH);
    if(recvbuf[0] == '8' && recvbuf[1] == '1' && recvbuf[2] == '2')//请求成功，更新索引信息
    {
        QString recv_msg = &(recvbuf[3]);
        QStringList courier_index = recv_msg.split("\n");
        courier_index.removeAt(0);
        ui->comboBox_select_courier->addItems(courier_index);
    }
    client->close();
}

void adminInterface::on_pushButton_assign_clicked()//点击分配快递
{
    QString courier = ui->comboBox_select_courier->currentText();
    int row = ui->tableWidget_parcel_assign->rowCount();
    QVector<QString> to_recv;
    for(int i = 0 ; i < row ; ++i)//获取所有要分配快递id
    {
        if(ui->tableWidget_parcel_assign->item(i , 0)->checkState() == Qt::Checked)
            to_recv.push_back(ui->tableWidget_parcel_assign->item(i , 1)->text());
    }
    if(to_recv.size() == 0 )
    {
        QMessageBox::warning(this, tr("错误"),
                              tr("请选择快递后再点击分配"),
                              QMessageBox::Yes);
        return ;
    }
    QString msg = "51";
    msg.append(courier).append("\n").append(QString::number(to_recv.size())).append("\n");//加入分配快递数
    for(QVector<QString>::iterator it = to_recv.begin() ; it != to_recv.end() ; ++it)
        msg.append(*it).append("\n");
    client->abort();
    client->connectToHost("127.0.0.1" , 1234);
    client->write(msg.toLatin1() , msg.size() + sizeof (char));
    char recvbuf[MAXLENGTH];
    memset(recvbuf , 0 , MAXLENGTH);
    client->waitForReadyRead();
    client->read(recvbuf , MAXLENGTH);
    if(recvbuf[0] == '5' && recvbuf[1] == '1' && recvbuf[2] == '1')
    {
        QMessageBox::warning(this, tr("分配成功"),
                              tr("成功分配快递"),
                              QMessageBox::Yes);
        refresh_assign_page();
    }
    else
    {
        QMessageBox::warning(this, tr("分配失败"),
                              tr("未能分配快递"),
                              QMessageBox::Yes);
    }
}

void adminInterface::refresh_assign_page()
{
    client->abort();
    client->connectToHost("127.0.0.1" , 1234);
    QString msg;
    msg.append("93");
    clear_current_parcel_info();
    client->write(msg.toLatin1() , msg.size() + sizeof(char));//发送待发送快递请求
    recv_parcel_data();
    ui->tableWidget_parcel_assign->clearContents();

    int row = a.getAllparcel().size();
    QStringList header;
    header << " " << tr("ID") << tr("发送人") << tr("发送时间") << tr("快递描述") << tr("快递员");
    ui->tableWidget_parcel_assign->setRowCount(row);
    ui->tableWidget_parcel_assign->setColumnCount(6);
    ui->tableWidget_parcel_assign->setHorizontalHeaderLabels(header);
    ui->tableWidget_parcel_assign->setColumnWidth(0 , 5);
    ui->tableWidget_parcel_assign->setColumnWidth(1 , 80);
    ui->tableWidget_parcel_assign->horizontalHeader()->setSectionResizeMode(0 , QHeaderView::Fixed);
    ui->tableWidget_parcel_assign->horizontalHeader()->setSectionResizeMode(1 , QHeaderView::Fixed);
    for(int i = 2 ; i < 6 ; ++i)//为后几个项
        ui->tableWidget_parcel_assign->horizontalHeader()->setSectionResizeMode(i , QHeaderView::Stretch);
    for(int r = 0 ; r < row ; ++r)//给每列添加item
    {
        QTableWidgetItem * check_box = new QTableWidgetItem();
        check_box->setCheckState(Qt::Unchecked);
        check_box->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_parcel_assign->setItem(r , 0 , check_box);
        for(int i = 1 ; i < 6 ; ++i)
        {
            QTableWidgetItem *item;
            switch (i) {
            case 1:
                item = new QTableWidgetItem(a.getAllparcel()[r]->getParcelID());
                break;
            case 2:
                item = new QTableWidgetItem(a.getAllparcel()[r]->getSender());
                break;
            case 3:
                item = new QTableWidgetItem(a.getAllparcel()[r]->getSendTime());
                break;
            case 4:
                item = new QTableWidgetItem(a.getAllparcel()[r]->getDescription());
                break;
            case 5:
                item = new QTableWidgetItem(a.getAllparcel()[r]->getCourier());
                break;
            default:
                item = new QTableWidgetItem("Error");
                        break;
            }
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item->setFlags(Qt::ItemIsEnabled);
            ui->tableWidget_parcel_assign->setItem(r , i , item);
            qDebug() << "item" << item->text() ;
        }
    }
}


