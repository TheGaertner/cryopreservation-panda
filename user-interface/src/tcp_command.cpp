#include "tcp_command.h"

Tcp_command::Tcp_command()
{

}

void Tcp_command::init(){
    socket_tcp_ = new QTcpSocket(this);
}

void Tcp_command::go_to_init()
{
        socket_tcp_->write("GoToInit");
        socket_tcp_->waitForBytesWritten(1000);

        socket_tcp_->abort();
        socket_tcp_->connectToHost(ip_,port_);  // 10.162.115.76
        if(!socket_tcp_->waitForConnected(5000))
        {
            qDebug() << "Error: " << socket_tcp_->errorString();
        }
}

void Tcp_command::custome_command(QString command)
{
        socket_tcp_->write(command.toUtf8());
        socket_tcp_->waitForBytesWritten(1000);

        socket_tcp_->abort();
        socket_tcp_->connectToHost(ip_,port_);  // 10.162.115.76
        if(!socket_tcp_->waitForConnected(5000))
        {
            qDebug() << "Error: " << socket_tcp_->errorString();
        }
}

void Tcp_command::connect(QString hostname)
{

    std::vector<std::string> strs;
    std::string hostname2 = hostname.toStdString();

    boost::split(strs, hostname2, boost::is_any_of(":"));

    ip_ = QString::fromStdString(strs[0]);
    port_ = std::stoi(strs[1]);
    socket_tcp_->abort();
    socket_tcp_->connectToHost(ip_,port_);  // 10.162.115.76
    if(!socket_tcp_->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket_tcp_->errorString();
    }else{
        qDebug() << "Connected";
    }
}

void Tcp_command::autorecovery()
{
    socket_tcp_->write("AutoErrorRecovery");
    socket_tcp_->waitForBytesWritten(1000);

    socket_tcp_->abort();
    socket_tcp_->connectToHost(ip_,port_);  // 10.162.115.76
    if(!socket_tcp_->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket_tcp_->errorString();
    }
}

