#include "xmlrpc.h"
#include <unordered_map>
#include <mutex>

struct PublisherDef {
    std::string topic;
    std::string type;
    std::string md5sum;
    std::function<void(int)> runFunc;
};

class RosNode {
    RpcValuePtr rpcServer(std::string methodName, const std::vector<RpcValuePtr>& args);
    void handleTcpRos(int fd);
    std::mutex mut;
    std::unordered_map<std::string, PublisherDef> publishers;
public:
    std::string ident;
    std::string hostname;
    std::string getMasterUri();
    IpAddress localAddr;
    InetAddress masterAddr;

    RpcValuePtr getParam(std::string name);
    bool publishAtMaster(PublisherDef def);
    void start();
    void startHostnameUpdater();
    bool updateHostnames();
};
