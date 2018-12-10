#include <unordered_map>
#include <unordered_set>
#include "util.h"
#include "ipaddress.h"
#include <functional>

namespace WebSetup {
extern std::unordered_map<std::string, IpAddress> hosts;
extern std::unordered_set<std::pair<std::string, std::string>, pair_hash<std::string, std::string> > hostList;
extern std::unordered_map<std::string, int> whitelist;
extern std::string confpath;
extern std::function<void()> hostsUpdateCallback;

bool processAddHost(IpAddress ip, std::string name);
std::string readWebsetupSecret();
std::string readId();
void writeHosts();
void readHosts();
void writeHostname(std::string hostname);
void writeWebsetupSecret(std::string token);
std::string readHostname();
}
