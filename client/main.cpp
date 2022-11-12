#include "mainwindow.h"
#include "login.h"
#include "userinterface.h"
#include "courierinterface.h"
#include "adminInterface.h"
#include "createaccount.h"

//#include "myclass.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    userInterface user;
    courierInterface courier;
    adminInterface admin;
    login lg;
    createAccount ca;

    QObject::connect(&lg , SIGNAL(show_create_account_interface()) , &ca , SLOT(recv_show()));
    QObject::connect(&ca , SIGNAL(back_to_login()) , &lg , SLOT(recv_show()));
    QObject::connect(&courier , SIGNAL(back_to_login()) , &lg , SLOT(recv_show()));
    QObject::connect(&admin , SIGNAL(back_to_login()) , &lg , SLOT(recv_show()));
    QObject::connect(&lg , SIGNAL(show_user_interface(QString)) , &user ,  SLOT(recv_show(QString)));
    QObject::connect(&lg , SIGNAL(show_courier_interface(QString)) , &courier , SLOT(recv_show(QString)));
    QObject::connect(&lg , SIGNAL(show_admin_interface(QString)) , &admin , SLOT(recv_show(QString)));
    QObject::connect(&user , SIGNAL(back_to_login()) , &lg , SLOT(recv_show()));
    lg.show();
    //w.show();
    return a.exec();
}
