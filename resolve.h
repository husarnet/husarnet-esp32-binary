#ifndef RESOLVE_H_
#define RESOLVE_H_

#include "port.h"
#include "ipaddress.h"

// Resolve address via DNS or network members list.
// Implemented in resolve_native.cpp for Linux and websetup.cpp for embedded targets
sockaddr_storage resolve(const std::string& hostname, const std::string& port);

#endif
