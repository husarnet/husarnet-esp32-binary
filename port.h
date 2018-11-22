// Copyright (c) 2017 Husarion Sp. z o.o.
// author: Michał Zieliński (zielmicha)
#ifndef PORT_H_
#define PORT_H_

#ifdef ESP_PLATFORM
#include "port_esp32.h"
#endif

#include <string>
#include <cstdint>
#include <vector>
#include <sodium.h>
#include <functional>

#ifdef __linux__
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef USE_HUSARNET_SOCKETS
#include "husarnet_socket.h"
#define SOCKFUNC(name) ::husarnet_##name
#else
#define SOCKFUNC(name) ::name
#endif
#define ENABLE_IPV6
#endif

#include "ipaddress.h"

// performance profiler

#ifdef ESP_PLATFORM
//#define ENABLE_HPREF
#endif

#ifdef ENABLE_HPREF
#define HPERF_RECORD(name) hperf.name = esp_timer_get_time();

struct hperf_t {
    int64_t start;
    // 290 ms
    int64_t secure_enter;
    // 1500 us
    int64_t secure_exit;
    // 80 us
    int64_t husarnet_enter;
    // 180 us
    int64_t husarnet_exit;
    // 430 us
    int64_t udpsend1;
    // 786 us
    int64_t udpsend2;
    // 159 us
    int64_t exit;
};

extern hperf_t hperf;
void hperf_compute();
#else

#define HPERF_RECORD(name)
__attribute__((weak)) void hperf_compute() {}
#endif
// ---

int64_t currentTime();
void startThread(std::function<void()> func, const char* name, int stack);

inline std::string randBytes(int count) {
    std::string s;
    s.resize(count);
    randombytes_buf(&s[0], count);
    return s;
}

void beforeDropCap();
std::vector<IpAddress> getLocalAddresses();

sockaddr_storage resolve(const std::string& hostname, const std::string& port);
#endif
