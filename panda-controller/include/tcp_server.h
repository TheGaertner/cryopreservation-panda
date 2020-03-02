#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "task_handler.h"

std::string make_daytime_string();

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

//

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::asio::io_service& io_service, task_handler *task_handler)
    {
        return pointer(new tcp_connection(io_service, task_handler));
    }

    tcp::socket& socket();

    void start();


    tcp_connection(boost::asio::io_service& io_service, task_handler *task_handler);

private:
    void handle_write();
    void handle_read(const boost::system::error_code& ec, std::size_t nbytes);
    void start_read();

    tcp::socket socket_;

    std::array<char, 256> input2;
    char input[256];
    task_handler *task_handler_;

};

class tcp_server
{
public:
    tcp_server(boost::asio::io_service& io_service, int port);
    tcp_server(boost::asio::io_service& io_service, int port,task_handler *task_handler);

private:
    void start_accept();

    void handle_accept(tcp_connection::pointer new_connection,
                       const boost::system::error_code& error);

    tcp::acceptor acceptor_;
    task_handler *task_handler_;
};


#endif // TCP_SERVER_H
