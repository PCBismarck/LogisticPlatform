#include "myclass.h"

parcel::~parcel()
{

}

const QString parcel::getSender(){
    return sender;
}

void parcel::setSender(QString sender_toset){
    sender = sender_toset;
}

const QString parcel::getReceiver(){
    return receiver;
}

void parcel::setReceiver(QString receiver_toset){
    receiver = receiver_toset;
}

const QString parcel::getSendTime(){
return send_time;
}

void parcel::setSendTime(QString time_toset){
    send_time = time_toset;
}

const QString parcel::getReceiveTime(){
    return receive_time ;
}

void parcel::setRecieveTime(QString time_toset){
    receive_time = time_toset;
}

const QString parcel::getDescription(){
    return description ;
}

void parcel::setDescription(QString description_toset){
    description = description_toset;
}

const QString parcel::getParcelID(){
    return parcel_id;
}

void parcel::setParcelID(QString id_toset){
    parcel_id = id_toset;
}

int parcel::getState(){
    return state;
}

void parcel::setState(int state_toset){
    state = state_toset;
}

void parcel::setType(int type_toset){
    type = type_toset;
}

int parcel::getType(){
    return type;
}

double parcel::getNum(){
    return num;
}

void parcel::setNum(double num_toset){
    num = num_toset;
}

void parcel::setCourier(QString courier_toset){
    courier = courier_toset;
}

const QString parcel::getCourier(){
    return courier;
}

double parcel::getPrice(){
    long num_int = getNum();
    int times;
    if(!(std::abs(getNum() - num_int) < 1e-15))//如果不为整数,向上取整
        ++num_int;
    switch(type)
    {
        case 1: times = 5;break;
        case 2: times = 2;break;
        case 3: times = 8;break;
        default: times = 0;break;
    }
    return times*num_int;
}


user::user(){
    user_name = '\0';
    name = '\0';
    phone_num = '\0';
    password = '\0';
    balance = 0;
}

user::~user(){
    address.clear();
    for(QVector<parcel*>::iterator it = my_parcel.begin() ; it != my_parcel.end() ; it++)
    {
        delete *it;
        *it = NULL;
    }
}

const QString user::getUserName(){
    return user_name;
}

void user::setUserName(QString name_toset){
    user_name = name_toset;
}

const QString user::getName(){
    return name;
}

void user::setName(QString name){
    this->name = name;
}

const QString user::getPhoneNum(){
    return phone_num;
}

void user::setPhoneNum(QString phone){
    phone_num = phone;
}

const QString user::getPassword(){
    return password;
}

void user::setPassword(QString password_toset){
    password = password_toset;
}

QVector<QString>& user::getAddrss(){
    return address;
}

void user::addAddress(QString address_toset){
    address.push_back(address_toset);
}

double user::getBalance(){
    return balance;
}

void user::setBalance(double balance_toset){
    balance = balance_toset;
}

void user::addParcel(parcel* parcel_toadd){
    my_parcel.push_back(parcel_toadd);
}

QVector<parcel*>& user::getParcel(){
    return my_parcel;
}//查询快递



courier::courier(){
    name = '\0';
    password = '\0';
    phone_num = '\0';
    balance = 0;
}

courier::~courier(){
    for(QVector<parcel*>::iterator it = my_parcel.begin() ; it != my_parcel.end() ; it++)
    {
        delete *it;
        *it = NULL;
    }
}

const QString courier::getName(){
    return name;
}

void courier::setName(QString name){
    this->name = name;
}

const QString courier::getPhoneNum(){
    return phone_num;
}

void courier::setPhoneNum(QString phone){
    phone_num = phone;
}

const QString courier::getPassword(){
    return password;
}

void courier::setPassword(QString password_toset){
    password = password_toset;
}

double courier::getBalance(){
    return balance;
}

void courier::setBalance(double balance_toset){
    balance = balance_toset;
}

void courier::addParcel(parcel* parcel_toadd){
    my_parcel.push_back(parcel_toadd);
}

QVector<parcel*>& courier::getParcel(){
    return my_parcel;
}


admin::admin(){

}

admin::~admin(){
    for(QVector<parcel*>::iterator it = allParcel.begin() ; it != allParcel.end() ; it++)
    {
        delete *it;
        *it = NULL;
    }
}

QVector<parcel*>& admin::getAllparcel() {
    return allParcel;
}
/*
client::client()
{
    //初始化DLL
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    //创建套接字
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    //向服务器发起请求
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);
    connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

}

client::~client()
{
    //关闭套接字
    closesocket(sock);
    //终止使用 DLL
    WSACleanup();
}

void client::set_identity(int id)
{
    this->identity = id;
}

SOCKET client::getSocket()
{
    return this->sock;
}
*/














































