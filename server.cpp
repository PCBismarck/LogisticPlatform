#include<iostream>
#include<fstream>
#include<istream>
#include<string>
#include<vector>
#include<set>
#include<algorithm>
#include<io.h>
#include<time.h>
#include<direct.h>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#define MAXLENGTH 4096
using namespace std;

/*

*/

class parcel
{
public:
    const string getSender() { return sender; }
    void setSender(string sender_toset) { sender = sender_toset; }
    const string getReceiver() { return receiver; }
    void setReceiver(string receiver_toset) { receiver = receiver_toset; }
    const string getSendTime() { return send_time; }
    void setSendTime(string time_toset) { send_time = time_toset; }
    const string getReceiveTime() { return receive_time ;}
    void setRecieveTime(string time_toset) {receive_time = time_toset; }
    const string getDescription() { return description ;}
    void setDescription(string description_toset) { description = description_toset; } 
    const string getParcelID() { return parcel_id; }
    void setParcelID(string id_toset) { parcel_id = id_toset; }
    const int getState() { return state; }
    void setState(int state_toset) { state = state_toset; }
    void setType(int type_toset) { type = type_toset; }
    const int getType() { return type; }
    const double getNum() { return num; }
    void setNum(double num_toset) { num = num_toset; }
    void setCourier(string courier_toset) { courier = courier_toset; }
    const string getCourier() { return courier; }
    const virtual double getPrice() 
    { 
        long num_int = getNum();
        int times;
        if(!(abs(getNum() - num_int) < 1e-15))//如果不为整数,向上取整
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
private:
    string parcel_id;//八位，从00000001开始逐渐增加
    string sender;
    string receiver;    
    string send_time;
    string receive_time;
    string description;
    string courier;
    int state;// 0表示待揽收 1表示待签收 2表示已签收
    int type;//1普通快递 2书 3易碎品
    double num;//本书/kg
};

class book : public parcel
{
public:
    book() { setType(1); }
    const double getPrice() 
    { 
        long num_int = getNum();
        if(!(abs(getNum() - num_int) < 1e-15))//如果不为整数,向上取整
            ++num_int;
        return 2*num_int; 
    }
};

class fragileProduct : public parcel
{
public:
    fragileProduct() { setType(2); }
    const double gerPrice() 
    { 
        long num_int = getNum();
        if(!(abs(getNum() - num_int) < 1e-15))//如果不为整数,向上取整
            ++num_int;
        return 8*num_int; 
    }
};

class normalProduct : public parcel
{
public:
    normalProduct() { setType(0); }
    const double getPrice()
    { 
        long num_int = getNum();
        if(!(abs(getNum() - num_int) < 1e-15))//如果不为整数,向上取整
            ++num_int;
        return 5*num_int; 
    }
};

class user
{
public:
    user()
    {
        user_name = '\0';
        name = '\0';
        phone_num = '\0';
        password = '\0';
        balance = 0;
    }
    ~user()
    {
        address.clear();
        for(vector<parcel*>::iterator it = my_parcel.begin() ; it != my_parcel.end() ; it++)
        {
            delete *it;
            *it = NULL;
        }
    }
    const string getUserName() { return user_name; }
    void setUserName(string name_toset) { user_name = name_toset; }
    const string getName() { return name; }
    void setName(string name) { this->name = name;}
    const string getPhoneNum() { return phone_num; }
    void setPhoneNum(string phone) { phone_num = phone; }
    const string getPassword() { return password; }
    void setPassword(string password_toset) { password = password_toset; }
    vector<string>& getAddrss() { return address; }
    void addAddress(string address_toset) { address.push_back(address_toset); }
    void removeAddress();//
    const double getBalance() { return balance; }    
    void setBalance(double balance_toset) { balance = balance_toset; }
    void addParcel(parcel* parcel_toadd) { my_parcel.push_back(parcel_toadd); }
    vector<parcel*>& getParcel() { return my_parcel; }//查询快递
    void setDefaultAddress(string address_default)
    {
        ofstream ofs("data\\user\\" + user_name + "\\defaultAddress.dic" , ios::out);
        ofs << address_default << endl;
        ofs.close();
    }    
private:
    string user_name;
    string name;
    string phone_num;
    string password;
    vector<string> address;
    vector<parcel*> my_parcel;
    double balance;
};

class courier
{
public:
    courier()
    {
        name = '\0';
        password = '\0';
        phone_num = '\0';
        balance = 0;
    }
    ~courier()
    {
        for(vector<parcel*>::iterator it = my_parcel.begin() ; it != my_parcel.end() ; it++)
        {
            delete *it;
            *it = NULL;
        }        
    }
    const string getName() { return name; }
    void setName(string name) { this->name = name;}
    const string getPhoneNum() { return phone_num; }
    void setPhoneNum(string phone) { phone_num = phone; }
    const string getPassword() { return password; }
    void setPassword(string password_toset) { password = password_toset; }
    const double getBalance() { return balance; }    
    void setBalance(double balance_toset) { balance = balance_toset; }
    void addParcel(parcel* parcel_toadd) { my_parcel.push_back(parcel_toadd); }
    vector<parcel*>& getParcel() { return my_parcel; }    
private:
    string name;
    string password;
    string phone_num;
    vector<parcel*> my_parcel;
    double balance;
};

class admin//查询所有快递，按某个信息查询快递， 查询用户信息（利用login获取用户信息）
{
public:
    admin()
    {
        ifstream ifs("data\\admin\\balance.dic" , ios::in);
        ifs >> balance;
        ifs.close();
        ifs.open("data\\admin\\adminInfo.dic" , ios::in);
        ifs >> user_name >> name >> password ;
        ifs.close();
    }
    ~admin()
    {
        for(vector<parcel*>::iterator it = allParcel.begin() ; it != allParcel.end() ; it++)
        {
            delete *it;
            *it = NULL;
        }        
    }
    const void getAllParcelInfo();
    vector<parcel*>& getAllparcel() { return allParcel; }
    const double getBalance() { return balance; } 
    const string getName() { return name; }
    const string getPassword() { return password; }
    const string getUserName() { return user_name; }
private:
    string user_name;
    string name;
    string password;
    vector<parcel*> allParcel;//读入所有快递信息
    double balance;
};

class logisticPlatform
{
public:
    logisticPlatform()
    {
        WSAStartup( MAKEWORD(2, 2), &wsaData);
        //创建套接字
        servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        //绑定套接字
        memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
        sockAddr.sin_family = PF_INET;  //使用IPv4地址
        sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
        sockAddr.sin_port = htons(1234);  //端口
        bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
        //进入监听状态
        listen(servSock, 20);
        //接收客户端请求
        nSize = sizeof(SOCKADDR);

        ifstream ifs("data\\platformInfo.dic" , ios::in);
        ifs >> parcel_num;
        ifs.close();
    }
    ~logisticPlatform()
    {
        //关闭套接字
        //closesocket(clntSock);
        closesocket(servSock);
        //终止 DLL 的使用
        WSACleanup();
    }

    //一些辅助函数
    const bool isUseableName(string user_name); //检验用户名是否可用
    const bool isExisted(string user_name , string type);     //检验账号是否存在,type为"user" "courier"
    const bool verifyAccount(string name , string password  , int key_type);    //验证账号key_type=1(user)  key_type=2(courier) 
    const string nextParcelID();                //用于计算下一个发出的快递ID
    const double getParcelNum() { return parcel_num; }
    const string getSystemTime();               //获取当前系统时间，返回一个string
    const double isMyParcel(vector<parcel*> my_parcel , string ID);//判断是否是自己的快递,是则返回快递费用，不是返回0
    const bool illegalID(string ID);            //判断快递id是否违法
    const bool illegalDes(string des);          //判断描述是否含有<>
    const void getUserData(user& user_info , string user_name);
    const void getCourierData(courier& courier_info , string name);


    void login_socket(string msg);          //接收到登录请求
    void create_account_socket(string msg); //接收到创建账号请求（包括用户和快递员）
    void cancel_courier_socket(string msg); //接收到禁用快递员请求
    void send_parcel_socket(string msg);    //接收到用户发送快递请求
    void assign_parcel_socket(string msg);  //接收到管理员分配快递请求
    void pick_up_parcel_socket(string msg); //接收到快递员揽收快递请求
    void receive_parcel_socket(string msg); //接收到用户签收快递请求
    void change_parcel_info_socket(string msg);//接收到修改快递信息的请求
    void change_password_socket(string msg);//接收到用户修改密码的请求
    void inquiry_info_socket(string msg);   //接收到查询信息的请求
    void inquiry_parcel_by_keyword(string msg);//条件查询快递
    void balance_to_charge_socket(string msg); //充值余额
    void get_person_index_socket(string msg);
    void get_parcel_toassign_socket();

    void user_info_inquiry_reply(string user_name);
    void courier_info_inquiry_reply(string courier);
    void user_login_reply(string user_name);
    void courier_login_reply(string user_name);
    void admin_login_reply();
    bool verifyAdmin(string admin_name , string admin_pwd);
    void login_fault(int type);
    void send_parcel_info_socket(vector<parcel*> parcel_tosend);

    //socket
    WSADATA wsaData;
    SOCKET servSock;
    SOCKET clntSock;    
    struct sockaddr_in sockAddr;
    SOCKADDR clntAddr;
    int nSize;

private:
    long parcel_num;//记录目前的包裹数，同时也用于计算下一个包裹

};

int main(void)
{   
    logisticPlatform p;
    char buf[MAXLENGTH];
    for(;;)
    {
        p.clntSock = accept(p.servSock, (SOCKADDR*)&p.clntAddr, &p.nSize);
        int strLen = recv(p.clntSock, buf, MAXLENGTH, 0);  //接收客户端发来的数据
        int type = buf[0] - '0';
        cout << buf << endl;
        switch (type)
        {
        case 1://请求登录
            p.login_socket(buf);
            break;
        case 2://请求注册
            p.create_account_socket(buf);
            break;
        case 3://查询（管理员用）
            p.inquiry_info_socket(buf);break;
        case 4://禁用快递员账号
            p.cancel_courier_socket(buf);
            break;
        case 5://修改快递信息(发送 分配 揽收 签收)
            p.change_parcel_info_socket(buf);
            break;
        case 6://修改用户密码
            p.change_password_socket(buf);
            break;
        case 7://用户余额充值
            p.balance_to_charge_socket(buf);
            break;
        case 8://获取人员索引
            p.get_person_index_socket(buf);
            break;
        case 9:
            p.get_parcel_toassign_socket();
            break;
        default:
            break;
        }
        closesocket(p.clntSock);  //关闭套接字
        memset(buf, 0, MAXLENGTH);  //重置缓冲区        
    }
    return 0;
}

void logisticPlatform::get_person_index_socket(string msg)
{
    string key;
    switch(msg[1] - '0')
    {
        case 1: key = "user";break;
        case 2: key = "courier";break;
        default: break; 
    }
    vector<string> index;
    string buf;
    ifstream ifs("data\\admin\\" + key +"Index.dic" , ios::in);
    while (!ifs.eof())
    {
        getline(ifs , buf , '\n');
        if((msg[1] - '0') == 2)//查询快递员,还需要验证快递员是否在工作
        {
            if(!access(("data\\courier\\" + buf + "\\working.dic").c_str() , 0))
                index.push_back(buf);            
        }
        else
            index.push_back(buf);
        if(ifs.peek() == '\n') break;
    }
    ifs.close();
    string reply;
    reply.append("81").append(1 , msg[1]).append(to_string(index.size()));
    for(int i = 0 ; i < index.size() ; ++i)
        reply.append("\n").append(index[i]);
    send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);
}

void logisticPlatform::inquiry_parcel_by_keyword(string msg)
{
    // | type(3) | item(0) | func(1) | key_num(1-6) | key_type1 | key1 | key_type2 |----------
    vector<int> key_type;
    vector<string> key_word;
    int key_num = msg[3] - '0';
    int pos = 4;
    //解析发送快递报文
    for(int i = 0 ; i < key_num ; ++i)
    {
        string buf;
        key_type.push_back(msg[pos] - '0');//存入key_type
        for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)//读取keyword
            buf.append(1 , msg[pos]);
        key_word.push_back(buf);//存入keyword
        ++pos;//移动到下一个key_type
    }
    admin ad;
    ad.getAllParcelInfo();
    vector<parcel*> result = ad.getAllparcel();
    for(int i = 0 ; i < key_num ; i++)//循环多次把不符合条件的去除
    {
        for(vector<parcel*>::iterator it = result.begin() ; it != result.end() ; it++)
            switch(key_type[i])
            {
                case 1: if((*it)->getSender() != key_word[i] && (*it)->getReceiver() != key_word[i] ) { result.erase(it); --it;} break;
                case 2: if((*it)->getParcelID() != key_word[i] ) { result.erase(it); --it; } break;
                case 3: if((*it)->getSendTime() != key_word[i] && (*it)->getReceiveTime() != key_word[i] ) { result.erase(it); --it;} break;
                case 4: if((*it)->getSender() != key_word[i] ) { result.erase(it); --it;} break;
                case 5: if((*it)->getReceiver() != key_word[i] ) { result.erase(it); --it;} break;
                case 6: if((*it)->getCourier() != key_word[i] ) { result.erase(it); --it;} break;
                default: break;
            }
    }
    cout << "byKeyword:" << endl;
    send_parcel_info_socket(result);//发送筛选后的结果
}

void logisticPlatform::user_info_inquiry_reply(string user_name)
{
    // 回复|type(3)|flag(1)|identity(0)|account\n|name\n|password\n|phone\n|balance\n|address_num\n|address1\n|--------
    user data;
    string reply;    
    if(isExisted(user_name , "user"))//账号存在
    {

        getUserData(data , user_name);
        reply.append("310").append(data.getUserName()).append("\n").append(data.getName()).append("\n")\
             .append(data.getPassword()).append("\n").append(data.getPhoneNum()).append("\n")\
             .append(to_string(data.getBalance())).append("\n").append(to_string(data.getAddrss().size()))\
             .append("\n");                      
        for(vector<string>::iterator it = data.getAddrss().begin() ; it != data.getAddrss().end() ; it++)
            reply.append(*it).append("\n");
        
    }
    else//账号不存在
        reply.append("300");
    send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);
}

void logisticPlatform::courier_info_inquiry_reply(string account)
{
    courier data;
    string reply;
    if(isExisted(account , "courier") && !access(("data\\courier\\" + account + "\\working.dic").c_str() , 0))//账号存在
    {
        getCourierData(data , account);
        reply.append("311").append(data.getName()).append("\n")\
             .append(data.getPassword()).append("\n").append(data.getPhoneNum()).append("\n")\
             .append(to_string(data.getBalance())).append("\n"); 
    }
    else//账号不存在
        reply.append("301");
    send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);        
}

void logisticPlatform::balance_to_charge_socket(string msg)
{
    string user_name;
    string buf;
    double to_charge;
    //解析报文
    int pos;
    for(pos = 2 ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        user_name.append(1 , msg[pos]);
    for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        buf.append(1 , msg[pos]);
    to_charge = atof(buf.c_str());
    user user_info;
    getUserData(user_info , user_name);
    ifstream ifs("data\\user\\" + user_info.getUserName() + "\\account.dic" , ios::in);
    ofstream ofs("data\\user\\" + user_info.getUserName() + "\\temp.dic" , ios::out);
    for(int i = 0 ; i < 4 ; i++)
    {
        ifs >> buf;
        ofs << buf << endl;
    }
    user_info.setBalance(user_info.getBalance() + to_charge);
    ofs << user_info.getBalance();
    ofs.close();
    ifs.close();
    string reply;
    //如果失败    
    if(remove(("data\\user\\" + user_info.getUserName() + "\\account.dic").c_str()) \
    || rename(("data\\user\\" + user_info.getUserName() + "\\temp.dic").c_str() ,( "data\\user\\" + user_info.getUserName() + "\\account.dic").c_str()))
        reply.append("700");
    else         
        reply.append("710");

    send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);          
}

void logisticPlatform::inquiry_info_socket(string msg)
{
    int item = msg[1] - '0' ;
    int func = msg[2] - '0' ;
    if(item == 0)//快递查询
    {
        switch (func)
        {
        case 0://查询全部快递
        {
            admin a;
            a.getAllParcelInfo();
            send_parcel_info_socket(a.getAllparcel());
        }break;

        case 1://条件查询快递
            inquiry_parcel_by_keyword(msg);
            break;

        default:
            break;
        }
    }
    else if(item == 1)//人员信息查询
    {
        switch (func)
        {
        case 0://查询用户信息
            user_info_inquiry_reply(msg.substr(3));//从3开始为用户名字段
            break;

        case 1://查询快递员信息
            courier_info_inquiry_reply(msg.substr(3));
            break;

        default:
            break;
        }
    }

}

void logisticPlatform::change_password_socket(string msg)
{
    string user_name , name , phone_num , new_pwd , buf;
    double balance;
    int pos;
    //解析发送快递报文
    for(pos = 2 ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        user_name.append(1 , msg[pos]);
    for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        new_pwd.append( 1 , msg[pos]);
    ifstream ifs("data\\user\\" + user_name + "\\account.dic" , ios::in);
    ifs >> buf >> name >> phone_num >> buf >> balance;
    ifs.close(); 
    ofstream ofs("data\\user\\" + user_name + "\\account.dic" , ios::out); 
    ofs << user_name << endl << name << endl << phone_num << endl;
    ofs << new_pwd << endl << balance << endl;
    ofs.close();
    cout << "The password has changed " << endl;      
    
    string reply = "61";
    reply.append(1, msg[1]);
    send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);
}

void logisticPlatform::change_parcel_info_socket(string msg)
{
    int type = msg[1] - '0';
    switch (type)
    {
    case 0:
        send_parcel_socket(msg);
        break;
    case 1:
        assign_parcel_socket(msg);
        break;
    case 2:
        pick_up_parcel_socket(msg);
        break;
    case 3:
        receive_parcel_socket(msg);
        break;
    default:
        break;
    }
}

void logisticPlatform::receive_parcel_socket(string msg)
{
    set<string> id_torecv;
    string  parcel_num_buf;
    int num;
    int pos;
    //解析分配快递报文
    for(pos = 2 ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        parcel_num_buf.append( 1 , msg[pos]);
    num = atoi(parcel_num_buf.c_str());
    for(int i = 0 ; i < num ; ++i)//获取每个待分配快递id
    {
        string id_buf;
        for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
            id_buf.append( 1 , msg[pos]);
        id_torecv.insert(id_buf);
    }
    string time_recv = getSystemTime();
    //修改数据库文件
    ifstream ifs("data\\parcelInfo.dic" , ios::in);
    ofstream ofs("data\\tempInfo.dic" , ios::out);    
    string buf;
    for(set<string>::iterator it = id_torecv.begin() ; it != id_torecv.end() ; it++)
    {
        ifs >> buf;
        while(*it != buf)
        {
            ofs << buf;
            getline(ifs , buf , '\n');
            ofs << buf << endl;
            ifs >> buf;
        }
        //找到目标行
        ofs << buf + " " << 2 << " ";
        ifs >> buf;//读取掉原先的state
        //存入后5个状态(读到description的<)
        for(int i = 0 ; i < 5 ; i++)
        {
            ifs >> buf;
            ofs << buf << " ";
        }
        getline(ifs , buf , '>');//存入des
        ofs << buf << "> ";
        for(int i = 0 ; i < 2 ; i++)//读入courier和sendtime
        {
            ifs >> buf;
            ofs << buf << " ";
        }
        ofs << time_recv << endl; //存入接收时间
        getline(ifs , buf , '\n');//读取掉原来的NULL
    }
    //继续复制剩余内容
    while(!ifs.eof())
    {
        getline(ifs , buf , '\n');
        if(ifs.peek() == '\n' || buf == "") break;        
        ofs << buf << endl;
    }
    ifs.close();
    ofs.close();
    if(remove("data\\parcelInfo.dic") || rename("data\\tempInfo.dic" , "data\\parcelInfo.dic"))//remove 和 rename 成功返回0
        cout << "Fali " << endl;
    else
        cout << "Success " << endl;
        
    string reply = "51" ;
    reply.append(1, msg[1]).append(time_recv);
    send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);                  
}

void logisticPlatform::pick_up_parcel_socket(string msg)
{
    set<string> id_topick;
    string courier , parcel_num_buf , reward_buf;
    int num;
    double reward;
    int pos;
    //解析分配快递报文
    for(pos = 2 ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        courier.append(1 , msg[pos]);
    for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)    
        reward_buf.append(1 , msg[pos]);
    for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        parcel_num_buf.append(1 , msg[pos]);
    reward = atof(reward_buf.c_str());
    num = atoi(parcel_num_buf.c_str());
    for(int i = 0 ; i < num ; ++i)//获取每个待分配快递id
    {
        string id_buf;
        for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
            id_buf.append( 1 , msg[pos]);
        id_topick.insert(id_buf);
    }  
    //修改数据库文件
    ifstream ifs("data\\parcelInfo.dic" , ios::in);
    ofstream ofs("data\\tempInfo.dic" , ios::out);
    string buf;    
    for(set<string>::iterator it = id_topick.begin() ; it != id_topick.end() ; it++)
    {
        ifs >> buf;
        while(*it != buf)
        {
            ofs << buf;
            getline(ifs , buf , '\n');
            ofs << buf << endl;
            ifs >> buf;
        }
        //找到目标行
        ofs << buf + " " << 1;
        ifs >> buf;//读取掉原先的state
        getline(ifs , buf , '\n');//读取后面所有数据
        ofs << buf << endl;
    }
    //继续复制剩余内容
    while(!ifs.eof())
    {
        getline(ifs , buf , '\n');
        ofs << buf << endl;
        if(ifs.peek() == '\n') break;
    }
    ifs.close();
    ofs.close();
    //给快递员发钱
    ifs.open("data\\courier" + courier + "\\balance.dic" , ios::in);
    double balance;
    ifs >> balance; 
    ifs.close();   
    balance += reward/2;
    ofs.open("data\\courier\\" + courier + "\\balance.dic" , ios::out);
    ofs << balance;
    ifs.close();
    //管理员扣钱
    ifs.open("data\\admin\\balance.dic" , ios::in);
    ifs >> balance;
    ifs.close();
    ofs.open("data\\admin\\balance.dic" , ios::out);
    ofs << (balance - reward/2) << endl;
    ofs.close();

    if(remove("data\\parcelInfo.dic") || rename("data\\tempInfo.dic" , "data\\parcelInfo.dic"))//remove 和 rename 成功返回0
        cout << "Fali " << endl;
    else
        cout << "Success " << endl;

    string reply = "51";
    reply.append(1 , msg[1]);
    send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);
}

void logisticPlatform::assign_parcel_socket(string msg)
{
    set<string> id_toassign;
    set<string> all_parcel;
    string courier , parcel_num_buf;
    int num;
    int pos;
    //解析分配快递报文
    for(pos = 2 ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        courier.append(1 , msg[pos]);
    for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        parcel_num_buf.append( 1 , msg[pos]);
    num = atoi(parcel_num_buf.c_str());
    for(int i = 0 ; i < num ; ++i)//获取每个待分配快递id
    {
        string id_buf;
        for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
            id_buf.append( 1 , msg[pos]);
        id_toassign.insert(id_buf);
        all_parcel.insert(id_buf);
    }    
    //修改数据库文件            
    ifstream ifs("data\\parcelInfo.dic" , ios::in);
    ofstream ofs("data\\tempInfo.dic" , ios::out);
    ifstream ifs_courier("data\\courier\\" + courier + "\\parcel.dic" , ios::in);
    string buf;    
    while(ifs_courier >> buf)
        all_parcel.insert(buf);
    ifs_courier.close();
    ofstream ofs_courier("data\\courier\\" + courier + "\\parcel.dic" , ios::out);
    for(set<string>::iterator it = id_toassign.begin() ; it != id_toassign.end() ; it++)
    {
        ifs >> buf;
        while(*it != buf)
        {
            ofs << buf;
            getline(ifs , buf , '\n');
            ofs << buf << endl;
            ifs >> buf;
        }
        //找到目标行
        ofs << buf + " ";//存入id
        //存入后6个状态(到courier前)
        for(int i = 0 ; i < 6 ; i++)
        {
            ifs >> buf;
            ofs << buf << " ";
        }
        //第六个buf读入<
        getline(ifs , buf , '>');
        ofs << buf << "> ";
        ofs << courier; //存入快递员
        ifs >> buf;//读取掉原来的快递员(NULL)
        getline(ifs , buf , '\n');//读取发送时间和接收时间
        ofs << buf << endl;//存入信息
    
        //快递id加入快递员快递信息中  
        //ofs_courier << *it << endl;
    }
    //继续复制剩余内容
    while(!ifs.eof())
    {
        getline(ifs , buf , '\n');
        ofs << buf << endl;
        if(ifs.peek() == '\n') break;
    }
    ifs.close();
    ofs.close();
    //重置快递员快递信息
    for(set<string>::iterator it = all_parcel.begin() ; it != all_parcel.end() ; it++)
        ofs_courier << *it << endl;
    ofs_courier.close();                 
    if(remove("data\\parcelInfo.dic") || rename("data\\tempInfo.dic" , "data\\parcelInfo.dic"))//remove 和 rename 成功返回0
        cout << "Fali " << endl;
    else
        cout << "Success " << endl;
    
    string reply = "51";
    reply.append(1 , msg[1]);
    send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);
}

void logisticPlatform::send_parcel_socket(string msg)//用户发送快递
{
    string user_name , parcel_type , num , receiver , description ;
    string parcel_id = nextParcelID() , cost_buf; 
    
    double cost;
    int pos;
    //解析发送快递报文
    for(pos = 2 ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        cost_buf.append(1 , msg[pos]);
    for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        user_name.append( 1 , msg[pos]);
    for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        parcel_type.append( 1 , msg[pos]);
    for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        num.append( 1 , msg[pos]);
    for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        receiver.append( 1 , msg[pos]);
    for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        description.append( 1 , msg[pos]);
    cost = atof(cost_buf.c_str());

    ofstream ofs("data\\user\\" + user_name + "\\parcel.dic" , ios::app);//修改发送者数据文件
    ofs << parcel_id << endl;
    ofs.close();
    ofs.open("data\\user\\" + receiver + "\\parcel.dic" , ios::app);//修改接受者数据
    ofs << parcel_id << endl;
    ofs.close();    
    //修改平台数据库
    ofs.open("data\\parcelInfo.dic" , ios::app);
    ofs << parcel_id << " " << 0 << " " << parcel_type << " " << num << " " << user_name << " " << receiver ;
    ofs << " < " << description << "> " << "NULL " << getSystemTime() << " NULL" << endl;
    ofs.close();
    ofs.open("data\\platformInfo.dic" , ios::out);//存入新的包裹数
    ofs << (parcel_num + 1);
    ofs.close();
    parcel_num++;
    //扣除费用
    user user_info;
    getUserData(user_info, user_name);    
    user_info.setBalance(user_info.getBalance() - cost);
    ofs.open("data\\user\\" + user_info.getUserName() + "\\account.dic" , ios::out);
    ofs << user_info.getUserName() << endl << user_info.getName() << endl << user_info.getPhoneNum() << endl << user_info.getPassword() << endl << user_info.getBalance() << endl;
    ofs.close();
    //费用转入管理员账户
    ifstream ifs("data\\admin\\balance.dic" , ios::in);
    double balance;
    ifs >> balance;
    ifs.close();
    ofs.open("data\\admin\\balance.dic" , ios::out);
    ofs << (balance + cost) << endl;
    ofs.close();
    cout << "Your parcel " << parcel_id << " is preparing for sending! " << endl;
    string reply = "51" ;
    reply.append(1 ,msg[1]).append(parcel_id).append("\n").append(getSystemTime());
    send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);
}

void logisticPlatform::cancel_courier_socket(string msg)
{
    string name;
    for(int pos = 2 ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        name.append( 1 , msg[pos]);
    string path = "data\\courier\\";
    if(isExisted(name , "courier"))
    {
        if(access((path + name + "\\working.dic").c_str() , 0))//access访问到返回0
        {
            cout << "The courier has canceled " << endl;
            string reply = "40";
            reply.append(1 , msg[1]);
            send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);
            return;            
        }
        string date;
        ifstream ifs(path + name + "\\working.dic" , ios::in);
        ofstream ofs(path + name + "\\cancel.dic" , ios::out);
        ifs >> date;
        ofs << date << " " << getSystemTime() << endl;
        ofs.close();
        ifs.close();
        if(remove((path + name + "\\working.dic").c_str()))
        {
            cout << "Fali to cancel " << name << endl;            
            string reply = "40";
            reply.append(1 , msg[1]);
            send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);
        }
        else
        {
            cout << "Success to cancel " << name << endl;            
            string reply = "41";
            reply.append(1 , msg[1]);
            send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);            
        }

    }
    else
    {
        cout << "Courier dose not exist " << endl;
        string reply = "40";
        reply.append(1 , msg[1]);
        send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);        
    }
}

void logisticPlatform::create_account_socket(string msg)
{
    int type = msg[1] - '0';
    switch (type)
    {
    case 1://用户注册
    {
        string user_name , password , name , phone , address;
        int pos;
        //解析注册请求报文
        int identity = msg[1] - '0';//获取身份
        for(pos = 2 ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
            user_name.append( 1 , msg[pos]);
        for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
            password.append( 1 , msg[pos]);
        for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
            name.append( 1 , msg[pos]);
        for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
            phone.append( 1 , msg[pos]);
        for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
            address.append( 1 , msg[pos]);
        //修改数据库文件
        string userDir = "data\\user\\" + user_name ;
        mkdir(userDir.c_str());//创建用户名文件夹
        //创建默认文件
        ofstream ofs;
        //创建account.txt
        ofs.open(userDir + "\\account.dic" , ios::out );
        ofs << user_name << endl << name << endl << phone << endl << password << endl << 0 << endl;
        ofs.close();
        //创建address.txt
        ofs.open(userDir + "\\address.dic" , ios::out );
        ofs << "1" << endl << address << endl;
        ofs.close();
        //创建defaultAddress.dic
        ofs.open(userDir + "\\defaultAddress.dic" , ios::out );
        ofs << address << endl;
        ofs.close();
        //创建parcel.txt
        ofs.open(userDir + "\\parcel.dic" , ios::out );
        ofs.close();
        //将新建用户名加入用户索引中
        ofs.open("data\\admin\\userIndex.dic" , ios::app);
        ofs << user_name << endl;
        ofs.close();
        //cout << "Account has created! " << endl;
        string reply = "21" ;
        reply.append(1 , msg[1]);
        cout << msg[1] << endl;
        //cout << reply << endl;
        send(clntSock ,  reply.c_str() , reply.size() + sizeof(char) , 0);
    }break;
    
    case 2://快递员注册
    {
        string name , password , phone_num;
        int pos;
        //解析注册请求报文
        int identity = msg[1] - '0';//获取身份
        for(pos = 2 ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
            name.append( 1 , msg[pos]);
        for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
            password.append( 1 , msg[pos]);
        for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
            phone_num.append( 1 , msg[pos]);
        //修改数据库文件
        string path = "data\\courier\\" + name;
        mkdir(path.c_str());//创建快递员文件夹
        //创建默认文件
        ofstream ofs;
        //account.dic
        ofs.open(path + "\\account.dic" , ios::out);
        ofs << name << endl << phone_num << endl << password << endl; 
        ofs.close();
        //parcel.dic
        ofs.open(path + "\\parcel.dic" , ios::out);
        ofs.close();
        //balance.dic
        ofs.open(path + "\\balance.dic" , ios::out);
        ofs << 0 << endl;
        ofs.close();
        //working.dic
        ofs.open(path + "\\working.dic" , ios::out);
        ofs << getSystemTime() << endl;
        ofs.close();
        //将新建courier加入快递员索引
        ofs.open("data\\admin\\courierIndex.dic" , ios::app);
        ofs << name << endl;
        ofs.close();
        cout << "Courier account has created! " << endl;
        string reply = "21";
        reply.append(1 , msg[1]);
        cout << reply << endl;
        send(clntSock , reply.c_str() , reply.size() + sizeof(char) , 0);
    }break;

    default:
        break;
    }
}

void logisticPlatform::login_socket(string msg)
{
    string account , password;
    int pos;
    //解析登录请求报文
    int identity = msg[1] - '0';//获取身份

    for(pos = 2 ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        account.append( 1 , msg[pos]);
    for(++pos ; pos < msg.size() && msg[pos] != '\n' ; ++pos)
        password.append( 1 , msg[pos]);

    switch (identity)
    {
    case 1://用户
        if(verifyAccount(account , password , 1))//身份验证成功
            user_login_reply(account);        
        else
            login_fault(1);        
        break;

    case 2://快递员
        if(verifyAccount(account , password , 2))
            if(!access(("data\\courier\\" + account + "\\working.dic").c_str() , 0))//账号未禁用
                courier_login_reply(account);
        else
            login_fault(2);
        break;

    case 3://管理员
        if(verifyAdmin(account , password))
            admin_login_reply();
        else
            login_fault(3);
        break;
    default:
        break;
    }

}

bool logisticPlatform::verifyAdmin(string admin_name , string admin_pwd)
{
    admin a;
    if(admin_name == a.getUserName() && admin_pwd == a.getPassword())
        return true;
    return false;
}

void logisticPlatform::login_fault(int type)
{
    string reply;
    reply.append("10").append(to_string(type));
    send(clntSock , reply.c_str() , reply.size() + sizeof(char), 0);
}

void logisticPlatform::user_login_reply(string user_name)
{
    user u_info;
    getUserData(u_info , user_name);
    string reply;
    //前三个数字为 type flag identity
    reply.append("111").append(u_info.getUserName()).append("\n").append(u_info.getName()).append("\n")\
         .append(u_info.getPhoneNum()).append("\n").append(u_info.getPassword()).append("\n")\
         .append(to_string(u_info.getBalance())).append("\n").append(u_info.getAddrss()[0]);
    send(clntSock , reply.c_str() , reply.size() + sizeof(char), 0);
    cout << reply << endl;
    //closesocket(clntSock);
    //clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
    //send_parcel_info_socket(u_info.getParcel());
}

void logisticPlatform::courier_login_reply(string user_name)
{
    courier c_info;
    getCourierData(c_info , user_name);
    string reply;
    //前三个数字为 type flag identity
    reply.append("112").append(c_info.getName()).append("\n")\
         .append(c_info.getPhoneNum()).append("\n").append(c_info.getPassword()).append("\n")\
         .append(to_string(c_info.getBalance()));
    send(clntSock , reply.c_str() , reply.size() + sizeof(char), 0);
    //send_parcel_info_socket(c_info.getParcel());
}

void logisticPlatform::admin_login_reply()
{
    admin a;
    string reply;
    //前三个数字为 type flag identity
    reply.append("113").append(a.getUserName()).append("\n")\
         .append(a.getName()).append("\n").append(a.getPassword()).append("\n")\
         .append(to_string(a.getBalance()));
    send(clntSock , reply.c_str() , reply.size() + sizeof(char), 0);
}

void logisticPlatform::send_parcel_info_socket(vector<parcel*> parcel_tosend)
{
    long parcel_num = parcel_tosend.size();
    string num_info;
    num_info.append("30").append(to_string(parcel_num));
    cout << num_info << endl;
    send(clntSock , num_info.c_str() , num_info.size() + sizeof(char) , 0);
    for(vector<parcel*>::iterator it = parcel_tosend.begin() ; it != parcel_tosend.end() ; ++it)
    {
        string msg;
        msg.append("31"). append((*it)->getParcelID()).append("\n").append((*it)->getSender()).append("\n")\
           .append((*it)->getReceiver()).append("\n").append((*it)->getSendTime()).append("\n")\
           .append((*it)->getReceiveTime()).append("\n").append((*it)->getDescription()).append("\n")\
           .append((*it)->getCourier()).append("\n").append(to_string((*it)->getState())).append("\n")\
           .append(to_string((*it)->getType())).append("\n").append(to_string((*it)->getNum()));
        cout << msg << endl;
        send(clntSock , msg.c_str() , msg.size() + sizeof(char) , 0);
    }
}

void logisticPlatform::get_parcel_toassign_socket()
{
    admin ad;
    ad.getAllParcelInfo();
    vector<parcel*> result;
    for(vector<parcel*>::iterator it = ad.getAllparcel().begin() ; it != ad.getAllparcel().end() ; it++)
        if((*it)->getCourier() == "NULL")
            result.push_back(*it);
    send_parcel_info_socket(result);
}

const void admin::getAllParcelInfo()
{
    string buf;
    ifstream ifs("data\\parcelInfo.dic" , ios::in);
    while(!ifs.eof())
    {
        parcel* newParcel = new parcel;
        ifs >> buf;        
        newParcel->setParcelID(buf);
        ifs >> buf;
        newParcel->setState(stoi(buf)); 
        ifs >> buf;
        newParcel->setType(stoi(buf));
        ifs >> buf;
        newParcel->setNum(stod(buf));
        ifs >> buf;
        newParcel->setSender(buf);
        ifs >> buf;
        newParcel->setReceiver(buf);
        ifs >> buf;
        getline(ifs , buf , '>');
        newParcel->setDescription(buf);
        ifs >> buf;
        newParcel->setCourier(buf);
        ifs >> buf;
        newParcel->setSendTime(buf);       
        ifs >> buf;
        newParcel->setRecieveTime(buf);            
        allParcel.push_back(newParcel);
        getline(ifs , buf , '\n');     
        if(ifs.peek() == '\n') break;
    }
    ifs.close();
}

const bool logisticPlatform::isUseableName(string name) //用户名是否合法，合法则返回1，否则返回0
{
    for(int i = 0 ; i < name.size() ; i++)
    {
        if(!((name[i] >= '0' && name[i] <= '9') || (name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z')))//判断用户名是否合法
        {
            cout << "Invalid USERNAME!"  << endl;
            cout << i+1 << " " << name[i] <<endl;
            return false;            
        }
    }
    return true;
}

const bool logisticPlatform::isExisted(string name , string type)  //用户名是否存在，存在则返回1，否则返回0
{
    string path = "data\\" + type + "\\" + name;
    int i = access(path.c_str() , 0);
    if(access(path.c_str() , 0))//access() 判断路径是否存在，存在则返回0，不存在则返回-1
        return false;
    return true;
}

const bool logisticPlatform::verifyAccount(string name , string password , int key_type)//验证密码输入，验证成功返回1，否则返回0
{
    string type;
    if(key_type == 1)
        type = "user";
    else if(key_type == 2)
        type = "courier";
    if(!isExisted(name , type))
        return 0;
    string key;
    ifstream ifs;
    ifs.open("data\\" + type + "\\" + name + "\\account.dic" , ios::in);
    getline(ifs , key , '\n');//读取用户名
    //如果是验证用户密码，则再读一行
    if(key_type == 1)
        getline(ifs , key , '\n');//名称
    getline(ifs , key , '\n');//电话号码
    getline(ifs , key , '\n');//读取密码
    ifs.close();    
    if(password == key)
        return true;        
    return false;
}

const void logisticPlatform::getUserData(user& user_info , string user_name)
{
    string path = "data\\user\\" + user_name + "\\";
    string buf;
    double balance;
    ifstream ifs(path + "account.dic" , ios::in);
    ifs >> buf;
    user_info.setUserName(buf);
    ifs >> buf;
    user_info.setName(buf);
    ifs >> buf;
    user_info.setPhoneNum(buf);
    ifs >> buf;
    user_info.setPassword(buf);
    ifs >> balance;
    user_info.setBalance(balance);
    ifs.close();
    ifs.open(path + "address.dic" , ios::in);
    int num;
    ifs >> num;
    getline(ifs , buf , '\n');
    for(int i = 0 ; i < num ; i++)
    {
        getline(ifs , buf , '\n');//ifs >> buf;
        user_info.getAddrss().push_back(buf);
    }
    ifs.close();
    ifs.open(path + "parcel.dic" , ios::in);
    while(!ifs.eof())
    {
        getline(ifs , buf , '\n');
        if(ifs.peek() == '\n' || buf == "") break;        
        parcel *newparcel = new parcel;
        newparcel->setParcelID(buf);
        user_info.getParcel().push_back(newparcel);
    }
    ifs.close();
    ifs.open("data\\parcelInfo.dic");
    for(int i = 0 ; i < user_info.getParcel().size() ; i++)//每次读取一个自己的包裹信息
    {
        ifs >> buf;
        while(user_info.getParcel()[i]->getParcelID() != buf && user_info.getParcel().size() != 0 )//循环直到找到目标快递, 此时buf中为快递id
        {
            getline(ifs , buf , '\n');
            ifs >> buf;
        }
        ifs >> buf;
        user_info.getParcel()[i]->setState(stoi(buf));//将buf转为数字
        ifs >> buf;
        user_info.getParcel()[i]->setType(stoi(buf));
        ifs >> buf;
        user_info.getParcel()[i]->setNum(stod(buf));
        ifs >> buf;
        user_info.getParcel()[i]->setSender(buf);
        ifs >> buf;
        user_info.getParcel()[i]->setReceiver(buf);                
        ifs >> buf;//读入<
        getline(ifs , buf , '>');
        user_info.getParcel()[i]->setDescription(buf);
        ifs >> buf;
        user_info.getParcel()[i]->setCourier(buf);
        ifs >> buf;
        user_info.getParcel()[i]->setSendTime(buf);
        ifs >> buf;
        user_info.getParcel()[i]->setRecieveTime(buf);            
    }
    ifs.close();
}

const bool logisticPlatform::illegalID(string id)//违法id返回TRUE
{
    if(id.size() != 8)
        return true;
    for(string::iterator it = id.begin() ; it != id.end() ; it++)
    {
        if(!(*it >= '0' && *it <= '9'))
            return true;
    }
    return false;
}

const bool logisticPlatform::illegalDes(string des)
{
    for(string::iterator it = des.begin() ; it != des.end() ; it++)
        if(*it == '<' || *it == '>')
            return true;
    return false;
}

const double logisticPlatform::isMyParcel(vector<parcel*> my_parcel , string ID)
{
    for(vector<parcel*>::iterator it = my_parcel.begin() ; it != my_parcel.end() ; it++)
        if((*it)->getParcelID() == ID)
            return (*it)->getPrice();            
    return 0;
}

const string logisticPlatform::nextParcelID()
{
    string nextID = to_string(parcel_num + 1);
    nextID.insert(0 , 8 - nextID.size()  ,'0');//填补到八位
    return nextID;
}

const string logisticPlatform::getSystemTime()
{
    time_t nowtime;
    char Time[255];
	time(&nowtime);	
    struct tm* p = localtime(&nowtime);    
    strftime(Time , 255 , "%Y%m%d%H%M%S" , p );
    string key = Time;
    key.insert(12 , "-");
    key.insert(10 , "-" );
    key.insert(8 , "--");
    key.insert(6 , "-");
    key.insert(4 , "-");
    return key;
}

const void logisticPlatform::getCourierData(courier& courier_info , string name)
{
    string path = "data\\courier\\" + name + "\\";
    string buf;
    double balance;
    ifstream ifs(path + "account.dic" , ios::in);
    ifs >> buf;
    courier_info.setName(buf);
    ifs >> buf;
    courier_info.setPhoneNum(buf);
    ifs >> buf;
    courier_info.setPassword(buf);
    ifs.close();
    ifs.open(path + "balance.dic" , ios::in);
    ifs >> balance;
    courier_info.setBalance(balance);
    ifs.close();
    ifs.open(path + "parcel.dic" , ios::in);
    while(!ifs.eof())
    {
        getline(ifs , buf , '\n');
        if(ifs.peek() == '\n' || buf == "") break;        
        parcel *newparcel = new parcel;
        newparcel->setParcelID(buf);
        courier_info.getParcel().push_back(newparcel);
    }
    ifs.close();
    ifs.open("data\\parcelInfo.dic");
    for(int i = 0 ; i < courier_info.getParcel().size() ; i++)//每次读取一个自己的包裹信息
    {
        ifs >> buf;
        while(courier_info.getParcel()[i]->getParcelID() != buf && courier_info.getParcel().size() != 0 )//循环直到找到目标快递, 此时buf中为快递id
        {
            getline(ifs , buf , '\n');
            ifs >> buf;
        }
        ifs >> buf;
        courier_info.getParcel()[i]->setState(stoi(buf));//将buf转为数字
        ifs >> buf;
        courier_info.getParcel()[i]->setType(stoi(buf));
        ifs >> buf;
        courier_info.getParcel()[i]->setNum(stod(buf));
        ifs >> buf;
        courier_info.getParcel()[i]->setSender(buf);
        ifs >> buf;
        courier_info.getParcel()[i]->setReceiver(buf);                
        ifs >> buf;//读入<
        getline(ifs , buf , '>');
        courier_info.getParcel()[i]->setDescription(buf);
        ifs >> buf;
        courier_info.getParcel()[i]->setCourier(buf);
        ifs >> buf;
        courier_info.getParcel()[i]->setSendTime(buf);
        ifs >> buf;
        courier_info.getParcel()[i]->setRecieveTime(buf);            
    }
    ifs.close();    
}

