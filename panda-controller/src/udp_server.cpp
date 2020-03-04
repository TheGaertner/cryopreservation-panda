#include "udp_server.h"

udp_server::udp_server(boost::asio::io_service& io_service): socket_(io_service) //, udp::endpoint(udp::v4(), port)
{
    YAML::Node config = ConfigHandler::getConfig();

    io_service_ = &io_service;
    socket_.open(udp::v4());

    socket_.set_option(udp::socket::reuse_address(true));
    socket_.set_option(boost::asio::socket_base::broadcast(true));

    local_endpoint_ = boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::any(), config["nuc"]["udp_port"].as<int>());

    std::vector<std::string> broadcast_adresse_parts = cpp_utils::split_string(cpp_utils::get_ip_by_hostname(config["nuc"]["hostname"].as<std::string>()),".");
    std::string broadcast_adresse_merged = broadcast_adresse_parts[0]+"."+broadcast_adresse_parts[1]+"."+broadcast_adresse_parts[2]+".255";


    remote_endpoint0_ = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(broadcast_adresse_merged), config["client"]["udp_port"].as<int>());
    // remote_endpoint1_ = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("10.42.0.255"), config["client"]["udp_port"].as<int>());

    socket_.bind(local_endpoint_);
}


void udp_server::start_send(boost::asio::streambuf *buffer)
{
    socket_.send_to(buffer->data(), remote_endpoint0_);
    // socket_.send_to(buffer->data(), remote_endpoint1_);
}
