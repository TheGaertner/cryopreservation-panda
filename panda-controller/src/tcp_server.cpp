#include "tcp_server.h"

std::string make_daytime_string()
{
    std::time_t now = time(0);
    return ctime(&now);
}

////////// tcp_connection

tcp::socket& tcp_connection::socket()
{
    return socket_;
}

void tcp_connection::start()
{
    start_read();

}

tcp_connection::tcp_connection(boost::asio::io_service& io_service,task_handler *task_handler): socket_(io_service),task_handler_(task_handler)
{
}

void tcp_connection::handle_write()
{
}

void tcp_connection::start_read()
{

    boost::asio::async_read(socket_, boost::asio::buffer(input2),
            boost::bind(&tcp_connection::handle_read, shared_from_this(), _1, _2));
}


void tcp_connection::handle_read(const boost::system::error_code& ec, std::size_t nbytes)
{
    //std::cout << "Received: " << std::string(std::begin(input2),std::begin(input2)+nbytes) << std::endl;
    if(task_handler_!=NULL){
        task_handler_->add_task(std::string(std::begin(input2), std::begin(input2)+nbytes));
    }
    input2 = std::array<char, 256>();

}

////////// tcp_server

tcp_server::tcp_server(boost::asio::io_service& io_service, int port): acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
{

    start_accept();
}

tcp_server::tcp_server(boost::asio::io_service &io_service, int port, task_handler *task_handler): acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),task_handler_(task_handler)
{
    start_accept();
}

void tcp_server::start_accept()
{
    tcp_connection::pointer new_connection =
            tcp_connection::create(acceptor_.get_io_service(),task_handler_);

    acceptor_.async_accept(new_connection->socket(),
                           boost::bind(&tcp_server::handle_accept, this, new_connection,
                                       boost::asio::placeholders::error));
}

void tcp_server::handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& ec)
{

    if (!ec)
    {
        //std::cout << "Connection Accepted" << std::endl;
        new_connection->start();
        start_accept();
    }else{
        std::cout << ec.message() << std::endl;
    }
}

