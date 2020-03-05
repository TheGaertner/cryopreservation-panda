#ifndef TCP_COMMAND_H
#define TCP_COMMAND_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>

#include <boost/algorithm/string.hpp>


class Tcp_command : public QObject
{
    Q_OBJECT
public:
    Tcp_command();

public slots:
    void init();
    bool connect();
    void new_adresse(QString adresse);
    void write_command(QString command);
private:
    QTcpSocket *socket_tcp_ = nullptr;
    QString adresse_;
    QString ip_;
    int port_;
    bool adresse_changed_=false;
};

#endif // TCP_COMMAND_H
