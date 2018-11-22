#ifndef WEBSETUP_H_
#define WEBSETUP_H_
#include <unordered_map>
#include <string>
#include <functional>
#include "ipaddress.h"

namespace WebSetup {
// Returns hostname of the current node (reverse lookup using host DB)
std::string getHostname();

// Resolves hostname using the hosts DB
IpAddress resolve(std::string hostname);

// Starts the Husarnet thread
void start(std::string confpath, std::string userAgent);

// Returns hosts DB
std::unordered_map<std::string, IpAddress>& getHosts();

// Called when a new host is added to the host database.
extern std::function<void()> hostsUpdateCallback;

// Husarnet IPv6 address of this node
extern IpAddress nodeAddress;

// Shared secret
extern std::string sharedSecret;
}

#endif
