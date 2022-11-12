#include "userinterface.h"
#include "ui_userinterface.h"

userInterface::userInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::userInterface)
{
    ui->setupUi(this);
    client = new QTcpSocket;
    QStringList item_list;
    item_list << tr("普通快递 5元/kg") << tr("图书 2元/本") << tr("易碎品 8元/kg");
    ui->comboBox_select_parcel_type->addItems(item_list);
    ui->stackedWidget->setCurrentIndex(0);
}

userInterface::~userInterface()
{
    delete ui;
}

void userInterface::recv_show(QString recv_msg)
{

    //根据收到的信息的类型选择合适的处理方式
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
    case 1://登录信息
    {
        QString user_name , name , phone_num , password , bal_buf , address;
        int pos = 3;
        for(; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//user_name
            user_name.append(recv_msg[pos]);
        for(++pos ; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//name
            name.append(recv_msg[pos]);
        for(++pos ; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//phone_num
            phone_num.append(recv_msg[pos]);
        for(++pos ; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//password
            password.append(recv_msg[pos]);
        for(++pos ; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//balance
            bal_buf.append(recv_msg[pos]);
        for(++pos ; pos < recv_msg.size() && recv_msg[pos] != '\n' ; ++pos)//address
            address.append(recv_msg[pos]);
        u.setUserName(user_name);
        u.setName(name);
        u.setBalance(bal_buf.toDouble());
        u.setPhoneNum(phone_num);
        u.setPassword(password);
        u.getAddrss().push_back(address);
        ui->label_show_balance->setText(QString::number(u.getBalance() , 'f' , 2));
        ui->label_welcome->setText("欢迎使用, "+ name+ "! ");
        qDebug() << "login:" << name;
        QString msg = "30111" + name;
        client->write( msg.toLatin1() , msg.size() + sizeof (char));
        recv_message();
    }break;

    default:
        break;
    }
    this->show();
}


void userInterface::recv_message()//登录后获取用户数据
{
    qDebug() << "userinterface" ;
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
            u.getParcel().push_back(new_parcel);
        }
        refresh_main_page();
    }
}


void userInterface::refresh_main_page()//刷新主页
{
    ui->tableWidget_show_parcel_recv->clearContents();
    QVector<parcel*> to_recv ;//= u.getParcel();

    for(QVector<parcel*>::iterator it = u.getParcel().begin() ; it != u.getParcel().end() ; ++it)
    {
        if((*it)->getState() == 1)//为待签收快递
            to_recv.push_back(*it);
    }
    int row = to_recv.size();
    QStringList header;
    header << " " << tr("ID") << tr("发送人") << tr("发送时间") << tr("快递描述") << tr("快递员");
    ui->tableWidget_show_parcel_recv->setRowCount(row);
    ui->tableWidget_show_parcel_recv->setColumnCount(6);
    ui->tableWidget_show_parcel_recv->setHorizontalHeaderLabels(header);
    ui->tableWidget_show_parcel_recv->setColumnWidth(0 , 5);
    ui->tableWidget_show_parcel_recv->setColumnWidth(1 , 80);
    ui->tableWidget_show_parcel_recv->horizontalHeader()->setSectionResizeMode(0 , QHeaderView::Fixed);
    ui->tableWidget_show_parcel_recv->horizontalHeader()->setSectionResizeMode(1 , QHeaderView::Fixed);
    for(int i = 2 ; i < 6 ; ++i)//为后几个项目设置宽度方式
        ui->tableWidget_show_parcel_recv->horizontalHeader()->setSectionResizeMode(i , QHeaderView::Stretch);
    for(int r = 0 ; r < row ; ++r)//给每列添加item
    {
        QTableWidgetItem * check_box = new QTableWidgetItem();
        check_box->setCheckState(Qt::Unchecked);
        check_box->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_show_parcel_recv->setItem(r , 0 , check_box);
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
            ui->tableWidget_show_parcel_recv->setItem(r , i , item);
            qDebug() << "item" << item->text() ;
        }
    }
}


void userInterface::on_pushButton_recv_clicked()//点击签收快递
{
    int row = ui->tableWidget_show_parcel_recv->rowCount();
    QVector<QString> to_recv;
    for(int i = 0 ; i < row ; ++i)//获取所有签收快递id
    {
        if(ui->tableWidget_show_parcel_recv->item(i , 0)->checkState() == Qt::Checked)
            to_recv.push_back(ui->tableWidget_show_parcel_recv->item(i , 1)->text());
    }
    if(to_recv.size() == 0 )
    {
        QMessageBox::warning(this, tr("错误"),
                              tr("请选择快递后再点击签收"),
                              QMessageBox::Yes);
        return ;
    }
    QString msg = "53";
    msg.append(QString::number(to_recv.size())).append("\n");//加入签收快递数
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
    msg = recvbuf;
    if(recvbuf[0] == '5' && recvbuf[1] == '1' && recvbuf[2] == '3')//如果成功，删除user内的数据与服务器保持一致
    {
        QString recv_time = msg.right(msg.size() - 3);
        for(QVector<QString>::iterator it_s = to_recv.begin() ; it_s != to_recv.end() ; ++it_s)
        {
            QVector<parcel*>::iterator it_p;
            for(it_p = u.getParcel().begin() ; \
                it_p != u.getParcel().end() && (*it_p)->getParcelID() != (*it_s);\
                ++it_p);
            (*it_p)->setState(2);
            (*it_p)->setRecieveTime(recv_time);
        }
        QMessageBox::information(this, tr("签收成功"),
                              tr("已签收快递"),
                              QMessageBox::Yes);
        refresh_main_page();
    }
    else
    {
        QMessageBox::warning(this, tr("签收失败"),
                              tr("未能签收快递"),
                              QMessageBox::Yes);
    }

}


void userInterface::on_Button_main_clicked()//点击切换到主页面
{
    refresh_main_page();
    ui->stackedWidget->setCurrentIndex(0);
}


void userInterface::on_Button_info_clicked()//点击切换到个人信息页
{
    refresh_person_info_page();
    ui->stackedWidget->setCurrentIndex(3);
}


void userInterface::on_Button_send_parcel_clicked()//点击切换到快递发送页
{
    refresh_send_parcel_page();
    ui->stackedWidget->setCurrentIndex(2);
}


void userInterface::on_Button_inquiry_clicked()//点击切换到查询页
{
    ui->stackedWidget->setCurrentIndex(1);
}


void userInterface::on_Button_log_out_clicked()//点击登出
{
    this->hide();
    for(QVector<parcel*>::iterator it = u.getParcel().begin() ; it != u.getParcel().end() ; ++it)
    {
        delete *it;
        *it = NULL;
    }
    u.getParcel().clear();
    u.getAddrss().clear();
    client->close();
    ui->stackedWidget->setCurrentIndex(0);
    emit back_to_login();
}


void userInterface::on_pushButton_ensure_clicked()//点击确认发送快递
{
    QString receiver , description , cost , num;
    num = ui->lineEdit_parcel_num->text();
    receiver = ui->lineEdit_receiver_sendParcel->text();
    description = ui->lineEdit_description_sendParcel->text();
    cost = ui->lineEdit_cost->text();
    if(receiver.size() == 0 || description.size() == 0 || ui->lineEdit_parcel_num->text().size() == 0)
    {
        QMessageBox::warning(this, tr("错误"),
                              tr("缺少参数"),
                              QMessageBox::Yes);
        return ;
    }
    if(u.getBalance() < ui->lineEdit_cost->text().toDouble())
    {
        QMessageBox::warning(this, tr("错误"),
                              tr("余额不足"),
                              QMessageBox::Yes);
        return ;
    }

    QString type_s;
    switch (ui->comboBox_select_parcel_type->currentIndex()) {
    case 0:
        type_s = "1";break;
    case 1:
        type_s = "2";break;
    case 2:
        type_s = "3";break;
    default:
        break;
    }

    QString msg = "50";
    msg.append(ui->lineEdit_cost->text()).append("\n").append(u.getUserName()).append("\n")\
       .append(type_s).append("\n").append(num).append("\n").append(receiver).append("\n")\
       .append(description).append("\n");
    client->abort();
    client->connectToHost("127.0.0.1" , 1234);
    client->write(msg.toLatin1() , msg.size() + sizeof (char));
    char recvbuf[MAXLENGTH];
    memset(recvbuf , 0 , MAXLENGTH);
    client->waitForReadyRead();
    client->read(recvbuf , MAXLENGTH);
    msg = recvbuf;
    if(recvbuf[0] == '5' && recvbuf[1] == '1' && recvbuf[2] == '0')//如果成功，删除user内的数据与服务器保持一致
    {
        QStringList info = msg.right(msg.size() - 3).split('\n');
        QString send_time = info.last();
        QString parcel_id = info.first();
        parcel *new_parcel = new parcel;
        new_parcel->setParcelID(parcel_id);
        new_parcel->setType(ui->comboBox_select_parcel_type->currentIndex()+1);
        new_parcel->setState(0);
        new_parcel->setSender(u.getUserName());
        new_parcel->setSendTime(send_time);
        new_parcel->setDescription(description);
        new_parcel->setNum(num.toDouble());
        new_parcel->setCourier("NULL");
        new_parcel->setReceiver(receiver);
        new_parcel->setRecieveTime("NULL");
        u.getParcel().push_back(new_parcel);
        u.setBalance(u.getBalance() - cost.toDouble());
        QMessageBox::information(this, tr("发送成功"),
                              tr("您的快递正在准备送出"),
                              QMessageBox::Yes);
        ui->label_show_balance->setText(QString::number(u.getBalance() , 'f' , 2));
        ui->lineEdit_receiver_sendParcel->clear();
        ui->lineEdit_parcel_num->clear();
        ui->lineEdit_description_sendParcel->clear();
    }
    else
    {
        QMessageBox::warning(this, tr("发送失败"),
                              tr("您的快递没有发出"),
                              QMessageBox::Yes);
    }
}


void userInterface::on_comboBox_select_parcel_type_currentIndexChanged(int index)//下拉栏改变
{
    change_cost_show();
}


void userInterface::on_lineEdit_parcel_num_editingFinished()//待发送快递数量输入完毕
{
    change_cost_show();
}


double userInterface::get_parcel_cost(int type, double num)//获取快递花费金额
{
    int num_int = num;
    double cost = 0;
    if(abs(num - num_int) > 1e-15 )
        num_int++;
    switch (type) {
    case 0://普通
        cost = 5*num_int;break;
    case 1:
        cost = 2*num_int;break;
    case 2:
        cost = 8*num_int;break;
    default:
        break;
    }
    return cost;
}


void userInterface::change_cost_show()//改变余额显示
{
    int type = ui->comboBox_select_parcel_type->currentIndex();
    double num = ui->lineEdit_parcel_num->text().toDouble();
    int cost = get_parcel_cost(type , num);
    ui->lineEdit_cost->setText(QString::number(cost));
}


void userInterface::refresh_person_info_page()//刷新个人信息页
{
    ui->listWidget_person_info->clear();
    QStringList person_info;
    person_info << tr("个人信息：") << tr("账号:") << u.getUserName() << tr("姓名：") << u.getName() \
                << tr("电话：") << u.getPhoneNum() << tr("地址：") << u.getAddrss()[0];
    ui->listWidget_person_info->addItems(person_info);
}


void userInterface::refresh_send_parcel_page()//刷新快递发送页
{
    ui->lineEdit_receiver_sendParcel->clear();
    ui->lineEdit_parcel_num->clear();
    ui->lineEdit_description_sendParcel->clear();
}


void userInterface::on_pushButton_charge_balance_clicked()//点击充值余额
{
    double balance = ui->lineEdit_balance_to_charge->text().toDouble();
    if(ui->lineEdit_balance_to_charge->text().size() == 0 || balance <= 0)
    {
        QMessageBox::warning(this, tr("错误"),
                              tr("请输入正确的金额"),
                              QMessageBox::Yes);
        return ;
    }
    QString msg = "70";
    msg.append(u.getUserName()).append("\n")\
            .append(QString::number(ui->lineEdit_balance_to_charge->text().toDouble() , 'f' , 2));
    client->abort();
    client->connectToHost("127.0.0.1" , 1234);
    client->write(msg.toLatin1() , msg.size() + sizeof (char));
    char recvbuf[MAXLENGTH];
    memset(recvbuf , 0 , MAXLENGTH);
    client->waitForReadyRead();
    client->read(recvbuf , MAXLENGTH);
    if(recvbuf[0] == '7' && recvbuf[1] == '1' && recvbuf[2] == '0')
    {
        u.setBalance(u.getBalance() + balance);
        ui->label_show_balance->setText(QString::number(u.getBalance() , 'f' , 2));
        QString to_show = tr("您已充值").append(QString::number(balance , 'f' , 2)).append(tr("元"));
        QMessageBox::information(this, tr("充值成功"),
                              to_show,
                              QMessageBox::Yes);
    }
    else
    {
        QMessageBox::warning(this, tr("充值失败"),
                              tr("您未能充值成功"),
                              QMessageBox::Yes);
    }
}


void userInterface::on_pushButton_cahnge_pwd_clicked()//点击改变密码
{
    QString new_pwd = ui->lineEdit_new_pwd->text();
    if(new_pwd.size() < 6 || new_pwd.size() > 30)
    {
        QMessageBox::warning(this, tr("失败"),
                              tr("密码长度不少于6不超过30"),
                              QMessageBox::Yes);
        return ;
    }
    if(new_pwd == u.getPassword())
    {
        QMessageBox::warning(this, tr("失败"),
                              tr("该密码与原密码相同"),
                              QMessageBox::Yes);
        return ;
    }
    QString msg = "60";
    msg.append(u.getUserName()).append("\n").append(new_pwd);
    client->abort();
    client->connectToHost("127.0.0.1" , 1234);
    client->write(msg.toLatin1() , msg.size() + sizeof (char));
    char recvbuf[MAXLENGTH];
    memset(recvbuf , 0 , MAXLENGTH);
    client->waitForReadyRead();
    client->read(recvbuf , MAXLENGTH);
    if(recvbuf[0] == '6' && recvbuf[1] == '1' && recvbuf[2] == '0')
    {
        u.setPassword(new_pwd);
        QMessageBox::information(this, tr("修改成功"),
                              tr("您的密码已经修改"),
                              QMessageBox::Yes);
        ui->lineEdit_new_pwd->clear();
    }
    else
    {
        QMessageBox::warning(this, tr("修改失败"),
                              tr("您的密码修改失败"),
                              QMessageBox::Yes);
    }
}


void userInterface::on_pushButton_parcel_all_clicked()//点击显示全部快递
{
    show_parcel_result(u.getParcel());
}


void userInterface::show_parcel_result(QVector<parcel *> result)//在查询框显示查询结果
{
    ui->tableWidget_show_myparcel->clearContents();
    QStringList header;
    header << "ID" << tr("发送人") << tr("接收人") << tr("快递描述")\
           << tr("发送时间") << tr("接收时间") << tr("快递员");
    ui->tableWidget_show_myparcel->setColumnCount(7);
    ui->tableWidget_show_myparcel->setRowCount(result.size());
    ui->tableWidget_show_myparcel->setHorizontalHeaderLabels(header);
    ui->tableWidget_show_myparcel->setColumnWidth(0 , 60);
    ui->tableWidget_show_myparcel->setColumnWidth(1 , 60);
    ui->tableWidget_show_myparcel->setColumnWidth(2 , 60);
    ui->tableWidget_show_myparcel->setColumnWidth(6 , 60);
    //ui->tableWidget_show_myparcel->horizontalHeader()->setSectionResizeMode(0 , QHeaderView::Fixed);
    for(int i = 3 ; i < 6 ; ++i)//为后几个项目设置宽度方式
        ui->tableWidget_show_myparcel->horizontalHeader()->setSectionResizeMode(i , QHeaderView::Stretch);
    for(int row = 0 ; row < result.size() ; ++row)//每行添加item
    {
        ui->tableWidget_show_myparcel->setItem(row , 0 ,new QTableWidgetItem(result[row]->getParcelID()));
        ui->tableWidget_show_myparcel->setItem(row , 1 ,new QTableWidgetItem(result[row]->getSender()));
        ui->tableWidget_show_myparcel->setItem(row , 2 ,new QTableWidgetItem(result[row]->getReceiver()));
        ui->tableWidget_show_myparcel->setItem(row , 3 ,new QTableWidgetItem(result[row]->getDescription()));
        ui->tableWidget_show_myparcel->setItem(row , 4 ,new QTableWidgetItem(result[row]->getSendTime()));
        ui->tableWidget_show_myparcel->setItem(row , 5 ,new QTableWidgetItem(result[row]->getReceiveTime()));
        ui->tableWidget_show_myparcel->setItem(row , 6 ,new QTableWidgetItem(result[row]->getCourier()));
        for(int col = 0 ; col < 7 ; ++col)
        {
            ui->tableWidget_show_myparcel->item(row , col)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_show_myparcel->item(row , col)->setFlags(Qt::ItemIsEnabled);
        }
    }
}


void userInterface::on_pushButton_parcel_send_clicked()//点击查询我发送的快递
{
    QVector<parcel*> result;
    for(QVector<parcel*>::iterator it = u.getParcel().begin() ; it != u.getParcel().end() ; ++it)
        if((*it)->getSender() == u.getUserName())
            result.push_back(*it);
    show_parcel_result(result);
}


void userInterface::on_pushButton_parcel_recv_clicked()//点击查询我接收的快递
{
    QVector<parcel*> result;
    for(QVector<parcel*>::iterator it = u.getParcel().begin() ; it != u.getParcel().end() ; ++it)
        if((*it)->getReceiver() == u.getUserName())
            result.push_back(*it);
    show_parcel_result(result);
}


void userInterface::on_pushButton_by_keyword_clicked()//点击按条件进行查询
{
    QVector<parcel*> result(u.getParcel());
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


