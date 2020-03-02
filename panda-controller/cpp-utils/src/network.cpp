#include "cpp_utils/network.hpp"

#include "cpp_utils/files.hpp"
#include "cpp_utils/output.hpp"

namespace cpp_utils{

int ping(const char *ipaddr) {
    char *command = NULL;
    FILE *fp;
    int stat = 0;
    //  asprintf (&command, "%s %s -q 2>&1", "fping", ipaddr);
    int rtn = asprintf (&command, "%s %s -q -r 0 -t 50 2>&1", "fping", ipaddr);
    //    printf("%s\n",command);
    fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to execute fping command\n");
        free(command);
        return -1;
    }
    stat = pclose(fp);
    free(command);
    return WEXITSTATUS(stat);
}

/*  Check if an ip address is valid */
bool is_valid_ip_address(const char *ipaddr)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipaddr, &(sa.sin_addr));
    return result != 0;
}

std::string get_own_ip(const std::string &iface){
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    std::string ip="none";
    std::string interface;

    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            interface=std::string(ifa->ifa_name);
            if(interface==iface){
                ip=std::string(addressBuffer);
            }
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            //            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            interface=std::string(ifa->ifa_name);
            //            if(interface==iface){
            //                ip=std::string(addressBuffer);
            //            }
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return ip;
}

std::string get_ip_by_hostname(const std::string &hostname){
    hostent * record = gethostbyname(hostname.c_str());
    if(record == NULL)
    {
        print_error(hostname+" is unavailable.");
        return "";
    }
    in_addr * address = (in_addr * )record->h_addr;
    std::string ip_address = inet_ntoa(* address);
    return ip_address;
}

std::map<std::string,std::string> get_subnets(){
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    std::string ip;
    std::string interface;

    std::map<std::string,std::string> ifaces;

    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

            interface=std::string(ifa->ifa_name);
            ip=std::string(addressBuffer);
            std::string ip_tmp=convert_ip_to_default_format(ip);
            ip_tmp=ip_tmp.substr(0,ip_tmp.size()-3);
            ip=convert_ip_from_default_format(ip_tmp);
            ifaces.insert(std::pair<std::string,std::string>(std::string(ifa->ifa_name),ip));
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            //            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return ifaces;
}

std::vector<std::string> get_ifaces(){
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    std::vector<std::string> ifaces;

    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            ifaces.push_back(std::string(ifa->ifa_name));
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return ifaces;
}

bool rpc_call(const std::string &address, unsigned port, const std::string &method, const jsonrpccxx::positional_parameter& request, nlohmann::json& response, long timeout, unsigned id){
    try{
        CppHttpLibClientConnector httpClient(address, port);
        jsonrpccxx::JsonRpcClient client(httpClient, jsonrpccxx::version::v2);
        response = client.CallMethod<nlohmann::json>(1,method,request);
        return true;
    }catch(const nlohmann::json::exception& e){
        std::cout<<e.what()<<std::endl;
        return false;
    }catch(const jsonrpccxx::JsonRpcException& e){
        std::cout<<e.what()<<std::endl;
        return false;
    }
}

bool rpc_call_thr(const std::string &address, unsigned port, const std::string &method, nlohmann::json request, nlohmann::json& response, long timeout, unsigned id){
    try{
        CppHttpLibClientConnector httpClient(address, port);
        jsonrpccxx::JsonRpcClient client(httpClient, jsonrpccxx::version::v2);
        response = client.CallMethod<nlohmann::json>(1,method,{request});
        return true;
    }catch(const nlohmann::json::exception& e){
        std::cout<<e.what()<<std::endl;
        return false;
    }catch(const jsonrpccxx::JsonRpcException& e){
        std::cout<<e.what()<<std::endl;
        return false;
    }
}


std::string convert_ip_to_default_format(std::string ip){
    std::vector<std::string> ip_parts = split_string(ip,".");
    std::string ip_in_format="";
    for(unsigned i=0;i<ip_parts.size();i++){
        if(ip_parts[i].size()==1){
            ip_parts[i].insert(0,"xx");
        }
        if(ip_parts[i].size()==2){
            ip_parts[i].insert(0,"x");
        }
        ip_in_format+=ip_parts[i]+".";
    }
    ip_in_format.erase(ip_in_format.size()-1);
    return ip_in_format;
}

std::string convert_ip_from_default_format(std::string ip){
    std::vector<std::string> ip_parts = split_string(ip,".");
    std::string ip_in_format="";
    for(unsigned i=0;i<ip_parts.size();i++){
        if(ip_parts[i][0]=='x' && ip_parts[i][1]=='x'){
            ip_parts[i].erase(0,2);
        }else if(ip_parts[i][0]=='x'){
            ip_parts[i].erase(0,1);
        }
        ip_in_format+=ip_parts[i]+".";
    }
    ip_in_format.erase(ip_in_format.size()-1);
    return ip_in_format;
}

bool check_if_valid_ip(std::string ip){
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
    return result != 0;

}

CppHttpLibServerConnector::CppHttpLibServerConnector(jsonrpccxx::JsonRpcServer &server, std::string address, int port) : _server(server), _address(address), _port(port) {
    this->_httpServer.Post("/", [&](const httplib::Request &req, httplib::Response &res) {
        res.status = 200;
//        std::cout<<"REQUEST: "<<req.body<<std::endl;
        res.set_content(server.HandleRequest(req.body), "application/json");
//        std::cout<<"RESPONSE: "<<res.body<<std::endl;
    });
}

CppHttpLibServerConnector::~CppHttpLibServerConnector(){
    this->stop_listening();
}

bool CppHttpLibServerConnector::start_listening(){
    if (this->_httpServer.is_running())
        return false;
    this->_thread = std::thread([this]() { this->_httpServer.listen(this->_address.c_str(), this->_port); });
    return true;
}

void CppHttpLibServerConnector::stop_listening(){
    if (this->_httpServer.is_running()) {
        this->_httpServer.stop();
        this->_thread.join();
    }
}

CppHttpLibClientConnector::CppHttpLibClientConnector(const std::string &host, int port, double timeout) : _httpClient(host.c_str(), port), _timeout(timeout) {

}

std::string CppHttpLibClientConnector::Send(const std::string &request){
    this->_httpClient.set_timeout_sec(this->_timeout);
    auto res = this->_httpClient.Post("/", request, "application/json");
    if (!res || res->status != 200) {
        throw jsonrpccxx::JsonRpcException(-32003, "client connector error, received status != 200");
    }
    return res->body;
}

UDP::UDP(){

}

UDP::~UDP(){

}

bool UDP::initialize_sender(){
    if ((this->_s_send=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        print_error("Could not initialize socket.");
        std::cout<<std::strerror<<std::endl;
        return false;
    }

    int broadcastEnable=this->_broadcast;
    int ret=setsockopt(this->_s_send, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if(ret<0){
        print_error("Could not set socket options.");
        std::cout<<std::strerror<<std::endl;
        return false;
    }


    memset((char *) &this->_si_other, 0, sizeof(this->_si_other));
    this->_si_other.sin_family = AF_INET;
    this->_si_other.sin_port = htons(this->_port_send);

    inet_aton(this->_ip_send.c_str() , &this->_si_other.sin_addr);
    return true;
}

bool UDP::initialize_receiver(){
    return true;
}

bool UDP::send_message(const char *msg){
    return true;
}

bool UDP::receive_message(char *msg){
    return true;
}

}
