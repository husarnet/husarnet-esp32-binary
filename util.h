// Copyright (c) 2017 Husarion Sp. z o.o.
// author: Michał Zieliński (zielmicha)
#ifndef UTIL_H_
#define UTIL_H_
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include "fstring.h"
#include "string_view.h"
//#include <experimental/string_view>
//using std::experimental::string_view;

int64_t currentTime();

#ifdef __ANDROID__
#include <android/log.h>

#define LOG(msg, x...) __android_log_print(ANDROID_LOG_INFO, "husarnet", "[%ld] " msg "\n", (long int)currentTime(), ##x)
#define LOGV(msg, x...) LOG(msg, ##x)

#else
#define LOG(msg, x...) fprintf(stderr, "[%ld] " msg "\n", (long int)currentTime(), ##x)
#define LOGV(msg, x...) (getenv("HUSARNET_VERBOSE")?LOG(msg, ##x):0)
#endif

#define LOG_DEBUG(msg, x...)

#ifdef ESP_PLATFORM
inline const char* memmem(const char* stack, int slen, const char* needle, int nlen) {
    if (nlen > slen) return nullptr;

    for (int i=0; i <= slen - nlen; i ++) {
        if (memcmp(stack + i, needle, nlen) == 0)
            return stack + i;
    }
    return nullptr;
}
#endif

template <typename Vec, typename T>
bool insertIfNotPresent(Vec& v, const T& t) {
    if (std::find(v.begin(), v.end(), t) == v.end()) {
        v.push_back(t);
        return true;
    } else {
        return false;
    }
}

template <typename T>
std::string pack(T t) {
    std::string s;
    s.resize(sizeof(T));
    memcpy(&s[0], &t, sizeof(T));
    return s;
}

template <typename T>
void packTo(T t, void* dst) {
    memcpy(dst, &t, sizeof(T));
}

template <typename T>
T unpack(std::string s) {
    if(s.size() != sizeof(T)) abort();
    T r;
    memcpy(&r, s.data(), sizeof(T));
    return r;
}

template <typename T>
T unpack(fstring<sizeof(T)> s) {
    T r;
    memcpy(&r, s.data(), sizeof(T));
    return r;
}

static const char* hexLetters = "0123456789abcdef";

inline std::string encodeHex(std::string s) {
    std::string ret;
    for (unsigned char ch : s) {
        ret.push_back(hexLetters[(ch >> 4) & 0xF]);
        ret.push_back(hexLetters[ch & 0xF]);
    }
    return ret;
}

inline std::string decodeHex(std::string s) {
    if (s.size() % 2 != 0) return "";


    std::string ret;
    for (int i=0; i + 1 < (int)s.size(); i += 2) {
        int a = (int)(std::find(hexLetters, hexLetters + 16, s[i]) - hexLetters);
        int b = (int)(std::find(hexLetters, hexLetters + 16, s[i+1]) - hexLetters);
        if (a == 16 || b == 16) return "";
        ret.push_back((char)((a << 4) | b));
    }
    return ret;
}

template <typename A, typename B>
struct pair_hash {
    unsigned operator()(const std::pair<A, B>& a) const {
        return std::hash<A>()(a.first) + std::hash<B>()(a.second) * 23456213;
    }
};

#endif
