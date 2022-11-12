#ifndef MYCLASS_H
#define MYCLASS_H
#include <QString>
#include <QVector>
#include "number.h"

class parcel
{
public:
    virtual ~parcel();
    const QString getSender();
    void setSender(QString sender_toset);
    const QString getReceiver();
    void setReceiver(QString receiver_toset);
    const QString getSendTime();
    void setSendTime(QString time_toset);
    const QString getReceiveTime();
    void setRecieveTime(QString time_toset);
    const QString getDescription();
    void setDescription(QString description_toset);
    const QString getParcelID();
    void setParcelID(QString id_toset);
    int getState();
    void setState(int state_toset);
    void setType(int type_toset);
    int getType();
    double getNum();
    void setNum(double num_toset);
    void setCourier(QString courier_toset);
    const QString getCourier();
    virtual double getPrice();
private:
    QString parcel_id;//八位，从00000001开始逐渐增加
    QString sender;
    QString receiver;
    QString send_time;
    QString receive_time;
    QString description;
    QString courier;
    int state;// 0表示待揽收 1表示待签收 2表示已签收
    int type;//1普通快递 2书 3易碎品
    double num;//本书/kg
};

class user
{
public:
    user();
    ~user();
    const QString getUserName();
    void setUserName(QString name_toset);
    const QString getName();
    void setName(QString name);
    const QString getPhoneNum();
    void setPhoneNum(QString phone);
    const QString getPassword();
    void setPassword(QString password_toset);
    QVector<QString>& getAddrss();
    void addAddress(QString address_toset);
    void removeAddress();//
    double getBalance();
    void setBalance(double balance_toset);
    void addParcel(parcel* parcel_toadd);
    QVector<parcel*>& getParcel();//查询快递
private:
    QString user_name;
    QString name;
    QString phone_num;
    QString password;
    QVector<QString> address;
    QVector<parcel*> my_parcel;
    double balance;
};

class courier
{
public:
    courier();
    ~courier();
    const QString getName();
    void setName(QString name);
    const QString getPhoneNum();
    void setPhoneNum(QString phone);
    const QString getPassword();
    void setPassword(QString password_toset);
     double getBalance();
    void setBalance(double balance_toset);
    void addParcel(parcel* parcel_toadd);
    QVector<parcel*>& getParcel();
private:
    QString name;
    QString password;
    QString phone_num;
    QVector<parcel*> my_parcel;
    double balance;
};

class admin//查询所有快递，按某个信息查询快递， 查询用户信息（利用login获取用户信息）
{
public:
    admin();
    ~admin();
     void getAllParcelInfo();
    QVector<parcel*>& getAllparcel();
private:
    QString user_name;
    QString name;
    QString password;
    QVector<parcel*> allParcel;//读入所有快递信息
    double balance;
};

#endif // MYCLASS_H
