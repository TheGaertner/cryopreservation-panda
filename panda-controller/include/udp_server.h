#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include "cpp_utils/network.hpp"
#include "cpp_utils/files.hpp"

#include "config_handler.h"

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class udp_server
{
public:
    udp_server(boost::asio::io_service& io_service);

    void start_send(boost::asio::streambuf *buffer);

private:

    boost::asio::io_service *io_service_;
    udp::socket socket_;
    udp::endpoint local_endpoint_;
    udp::endpoint remote_endpoint0_;
    udp::endpoint remote_endpoint1_;
    boost::array<char, 1> recv_buffer_;

};
#endif // UDP_SERVER_H
