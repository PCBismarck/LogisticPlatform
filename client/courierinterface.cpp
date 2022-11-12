#include "courierinterface.h"
#include "ui_courierinterface.h"

courierInterface::courierInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::courierInterface)
{
    ui->setupUi(this);
    client = new QTcpSocket;
    refresh_person_info();
    ui->stackedWidget->setCurrentIndex(0);
}

courierInterface::~courierInterface()
{
    delete ui;
}

void courierInterface::recv_show(QString recv_msg)//登录后解析获取快递员信息
{
    client->abort();
    client->connectToHost("127.0.0.1" , 1234);
    int type = 0;
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
        QString name , phone_num , password , bal_buf;
        int pos = 3;
        for(; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//name
            name.append(recv_msg[pos]);
        for(++pos ; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//phone_num
            phone_num.append(recv_msg[pos]);
        for(++pos ; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//password
            password.append(recv_msg[pos]);
        for(++pos ; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//balance
            bal_buf.append(recv_msg[pos]);
        c.setName(name);
        c.setPhoneNum(phone_num);
        c.setPassword(password);
        c.setBalance(bal_buf.toDouble());
        ui->label_show_balance->setText(QString::number(c.getBalance() , 'f' , 2));
        ui->label_welcome->setText("欢迎使用, "+ name+ "! ");
        QString msg = "30116";
        msg.append(name);
        client->write(msg.toLatin1() , msg.size() + sizeof (char));
        recv_parcel_data();
    }break;

    default:
        break;
    }
    this->show();
}

void courierInterface::refresh_person_info()
{
    ui->listWidget_person_info->clear();
    QStringList items;
    items << tr("姓名:") << c.getName() << tr("电话号码:") << c.getPhoneNum();
    ui->listWidget_person_info->addItems(items);
}

void courierInterface::recv_parcel_data()//获取快递员快递信息
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
            c.getParcel().push_back(new_parcel);
        }
        refresh_main_page();
        refresh_person_info();
    }
}

void courierInterface::refresh_main_page()//刷新主页（揽收页）
{
    ui->tableWidget_show_parcel_pickup->clearContents();
    QVector<parcel*> to_recv ;//= u.getParcel();
    for(QVector<parcel*>::iterator it = c.getParcel().begin() ; it != c.getParcel().end() ; ++it)
    {
        if((*it)->getState() == 0)//为待签收快递
            to_recv.push_back(*it);
    }
    int row = to_recv.size();
    QStringList header;
    header << " " << tr("ID") << tr("发送人") << tr("发送时间") << tr("快递描述") << tr("快递员");
    ui->tableWidget_show_parcel_pickup->setRowCount(row);
    ui->tableWidget_show_parcel_pickup->setColumnCount(6);
    ui->tableWidget_show_parcel_pickup->setHorizontalHeaderLabels(header);
    ui->tableWidget_show_parcel_pickup->setColumnWidth(0 , 5);
    ui->tableWidget_show_parcel_pickup->setColumnWidth(1 , 80);
    ui->tableWidget_show_parcel_pickup->horizontalHeader()->setSectionResizeMode(0 , QHeaderView::Fixed);
    ui->tableWidget_show_parcel_pickup->horizontalHeader()->setSectionResizeMode(1 , QHeaderView::Fixed);
    for(int i = 2 ; i < 6 ; ++i)//为后几个项目设置宽度方式
        ui->tableWidget_show_parcel_pickup->horizontalHeader()->setSectionResizeMode(i , QHeaderView::Stretch);
    for(int r = 0 ; r < row ; ++r)//给每列添加item
    {
        QTableWidgetItem * check_box = new QTableWidgetItem();
        check_box->setCheckState(Qt::Unchecked);
        check_box->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_show_parcel_pickup->setItem(r , 0 , check_box);
        for(int i = 1 ; i < 6 ; ++i)
        {
            QTableWidgetItem *item;
            switch (i) {
            case 1:
                item = new QTableWidgetItem(to_recv[r]->getParcelID());
                break;
            case 2:
                item = new QTableWidgetItem(to_recv[r]->getSender());
                break;
            case 3:
                item = new QTableWidgetItem(to_recv[r]->getSendTime());
                break;
            case 4:
                item = new QTableWidgetItem(to_recv[r]->getDescription());
                break;
            case 5:
                item = new QTableWidgetItem(to_recv[r]->getCourier());
                break;
            default:
                item = new QTableWidgetItem("Error");
                        break;
            }
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item->setFlags(Qt::ItemIsEnabled);
            ui->tableWidget_show_parcel_pickup->setItem(r , i , item);
            qDebug() << "item" << item->text() ;
        }
    }
}

void courierInterface::on_Button_pick_up_clicked()//点击切换到快递揽收页
{
    refresh_main_page();
    ui->stackedWidget->setCurrentIndex(0);
}

void courierInterface::on_Button_inquiry_clicked()//点击切换到快递查询页
{
    ui->stackedWidget->setCurrentIndex(1);
}

void courierInterface::show_parcel_result(QVector<parcel *> result)//显示查询结果
{
    ui->tableWidget_show_inquiry_result->clearContents();
    QStringList header;
    header << "ID" << tr("发送人") << tr("接收人") << tr("快递描述")\
           << tr("发送时间") << tr("接收时间") << tr("快递员");
    ui->tableWidget_show_inquiry_result->setColumnCount(7);
    ui->tableWidget_show_inquiry_result->setRowCount(result.size());
    ui->tableWidget_show_inquiry_result->setHorizontalHeaderLabels(header);
    ui->tableWidget_show_inquiry_result->setColumnWidth(0 , 65);
    ui->tableWidget_show_inquiry_result->setColumnWidth(1 , 60);
    ui->tableWidget_show_inquiry_result->setColumnWidth(2 , 60);
    ui->tableWidget_show_inquiry_result->setColumnWidth(6 , 60);
    //ui->tableWidget_show_myparcel->horizontalHeader()->setSectionResizeMode(0 , QHeaderView::Fixed);
    for(int i = 3 ; i < 6 ; ++i)//为后几个项目设置宽度方式
        ui->tableWidget_show_inquiry_result->horizontalHeader()->setSectionResizeMode(i , QHeaderView::Stretch);
    for(int row = 0 ; row < result.size() ; ++row)//每行添加item
    {
        ui->tableWidget_show_inquiry_result->setItem(row , 0 ,new QTableWidgetItem(result[row]->getParcelID()));
        ui->tableWidget_show_inquiry_result->setItem(row , 1 ,new QTableWidgetItem(result[row]->getSender()));
        ui->tableWidget_show_inquiry_result->setItem(row , 2 ,new QTableWidgetItem(result[row]->getReceiver()));
        ui->tableWidget_show_inquiry_result->setItem(row , 3 ,new QTableWidgetItem(result[row]->getDescription()));
        ui->tableWidget_show_inquiry_result->setItem(row , 4 ,new QTableWidgetItem(result[row]->getSendTime()));
        ui->tableWidget_show_inquiry_result->setItem(row , 5 ,new QTableWidgetItem(result[row]->getReceiveTime()));
        ui->tableWidget_show_inquiry_result->setItem(row , 6 ,new QTableWidgetItem(result[row]->getCourier()));
        for(int col = 0 ; col < 7 ; ++col)
        {
            ui->tableWidget_show_inquiry_result->item(row , col)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_show_inquiry_result->item(row , col)->setFlags(Qt::ItemIsEnabled);
        }
    }
}

void courierInterface::on_pushButton_by_keyword_clicked()//点击按条件查询
{
    QVector<parcel*> result(c.getParcel());
    QVector<int> key_type;
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
        key_type.push_back(1);
        key_word.push_back(ui->lineEdit_user_name->text());
    }
    if(ui->checkBox_parcel_id->checkState() == Qt::Checked)
    {
        key_type.push_back(2);
        key_word.push_back(ui->lineEdit_parcel_id->text());
    }
    if(ui->checkBox_time->checkState() == Qt::Checked)
    {
        key_type.push_back(3);
        key_word.push_back(ui->lineEdit_time->text());
    }
    if(ui->checkBox_sender->checkState() == Qt::Checked)
    {
        key_type.push_back(4);
        key_word.push_back(ui->lineEdit_sender->text());
    }
    if(ui->checkBox_receiver->checkState() == Qt::Checked)
    {
        key_type.push_back(5);
        key_word.push_back(ui->lineEdit_receiver->text());
    }
    if(ui->checkBox_courier->checkState() == Qt::Checked)
    {
        key_type.push_back(6);
        key_word.push_back(ui->lineEdit_courier->text());
    }
    for(int i = 0 ; i < key_type.size() ; ++i)
    {
        for(QVector<parcel*>::iterator it = result.begin() ; it != result.end() ; it++)
            switch(key_type[i])
            {
                case 1: if((*it)->getSender() != key_word[i] && (*it)->getReceiver() != key_word[i] ) { result.erase(it); --it;} break;
                case 2: if((*it)->getParcelID() != key_word[i] ) { result.erase(it); --it;} break;
                case 3: if((*it)->getSendTime() != key_word[i] && (*it)->getReceiveTime() != key_word[i] ) { result.erase(it); --it;} break;
                case 4: if((*it)->getSender() != key_word[i] ) { result.erase(it); --it;} break;
                case 5: if((*it)->getReceiver() != key_word[i] ) { result.erase(it); --it;} break;
                case 6: if((*it)->getCourier() != key_word[i] ) { result.erase(it); --it;} break;
                default: break;
            }
    }
    show_parcel_result(result);
}


void courierInterface::on_pushButton_all_clicked()//点击显示全部快递
{
    show_parcel_result(c.getParcel());
}


void courierInterface::on_pushButton_pickup_all_clicked()//点击显示揽收的快递
{
    QVector<parcel*> result;
    for(QVector<parcel*>::iterator it = c.getParcel().begin() ; it != c.getParcel().end() ; ++it)
        if((*it)->getState() == 1 || (*it)->getState() == 2)
            result.push_back(*it);
    show_parcel_result(result);
}


void courierInterface::on_pushButton_pickup_clicked()//点击揽收按钮
{
    int row = ui->tableWidget_show_parcel_pickup->rowCount();
    QVector<QString> to_recv;
    double reward = 0;
    for(int i = 0 ; i < row ; ++i)//获取所有签收快递id
    {
        if(ui->tableWidget_show_parcel_pickup->item(i , 0)->checkState() == Qt::Checked)
        {
            to_recv.push_back(ui->tableWidget_show_parcel_pickup->item(i , 1)->text());
            QVector<parcel*>::iterator it_temp;
            for(it_temp = c.getParcel().begin() ; \
                it_temp != c.getParcel().end() && \
                (*it_temp)->getParcelID() != ui->tableWidget_show_parcel_pickup->item(i ,1)->text() ;\
                ++it_temp);
            reward += (*it_temp)->getPrice();
        }
    }
    if(to_recv.size() == 0 )
    {
        QMessageBox::warning(this, tr("错误"),
                              tr("请选择快递后再点击揽收"),
                              QMessageBox::Yes);
        return ;
    }
    QString msg = "52";
    msg.append(c.getName()).append("\n").append(QString::number(reward)).append("\n")\
            .append(QString::number(to_recv.size())).append("\n");
    for(QVector<QString>::iterator it = to_recv.begin() ; it != to_recv.end() ; ++it)
        msg.append(*it).append("\n");
    qDebug() << msg;
    client->abort();
    client->connectToHost("127.0.0.1" , 1234);
    client->write(msg.toLatin1() , msg.size() + sizeof (char));
    char recvbuf[MAXLENGTH];
    memset(recvbuf , 0 , MAXLENGTH);
    client->waitForReadyRead();
    client->read(recvbuf , MAXLENGTH);
    qDebug() << recvbuf;
    if(recvbuf[0] == '5' && recvbuf[1] == '1' && recvbuf[2] == '2')//如果成功，删除user内的数据与服务器保持一致
    {
        for(QVector<QString>::iterator it_s = to_recv.begin() ; it_s != to_recv.end() ; ++it_s)
        {
            QVector<parcel*>::iterator it_p;
            for(it_p = c.getParcel().begin() ; \
                it_p != c.getParcel().end() && (*it_p)->getParcelID() != (*it_s);\
                ++it_p);
            (*it_p)->setState(1);
        }
        QString reply = tr("已揽收快递，获得").append(QString::number(reward/2 , 'f' , 2)).append(tr("元"));
        QMessageBox::information(this, tr("揽收成功"),
                              reply,
                              QMessageBox::Yes);
        refresh_main_page();
        c.setBalance(c.getBalance() + reward/2);
         ui->label_show_balance->setText(QString::number(c.getBalance() , 'f' , 2));
    }
    else
    {
        QMessageBox::warning(this, tr("揽收失败"),
                              tr("未能揽收快递"),
                              QMessageBox::Yes);
    }
}


void courierInterface::on_Button_log_out_clicked()//点击登出
{
    this->hide();
    for(QVector<parcel*>::iterator it = c.getParcel().begin() ; it != c.getParcel().end() ; ++it)
    {
        delete *it;
        *it = NULL;
    }
    c.getParcel().clear();
    ui->stackedWidget->setCurrentIndex(0);
    client->close();
    emit back_to_login();
}

