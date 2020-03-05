#include "tcp_command.h"

Tcp_command::Tcp_command()
{

}

void Tcp_command::new_adresse(QString adresse)
{
    adresse_ = adresse;

    std::vector<std::string> strs;
    std::string hostname2 = adresse_.toStdString();

    boost::split(strs, hostname2, boost::is_any_of(":"));

    ip_ = QString::fromStdString(strs[0]);
    port_ = std::stoi(strs[1]);

    adresse_changed_ = true;
}

void Tcp_command::init(){
    socket_tcp_ = new QTcpSocket(this);
}

void Tcp_command::write_command(QString command)
{
    if((socket_tcp_->state()== QAbstractSocket::SocketState::UnconnectedState) || adresse_changed_==true){
        connect();
    }

    if(socket_tcp_->state()== QAbstractSocket::SocketState::ConnectedState){
        socket_tcp_->write(command.toUtf8());
        socket_tcp_->waitForBytesWritten(1000);
        socket_tcp_->abort();
    }

}

bool Tcp_command::connect()
{
    adresse_changed_ = false;

    socket_tcp_->abort();
    socket_tcp_->connectToHost(ip_,port_);
    if(!socket_tcp_->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket_tcp_->errorString();
        return false;
    }

    return true;
}


