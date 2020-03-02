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
    void connect(QString hostname);
    void autorecovery();
    void init();
    void go_to_init();
    void custome_command(QString command);
private:
    QTcpSocket *socket_tcp_ = nullptr;
    QString ip_;
    int port_;
};

#endif // TCP_COMMAND_H
