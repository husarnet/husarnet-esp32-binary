#pragma once
#include "port.h"
#include "util.h"

inline int readAll(int fd, void* buffer, int len) {
    int offset = 0;
    while (offset < len) {
        int ret = SOCKFUNC(read)(fd, ((char*)buffer) + offset, len - offset);
        if (ret == 0) break;
        if (ret == -1) return -1;
        offset += ret;
    }
    return offset;
}

inline int32_t readInt(int fd) {
    int32_t ret;
    if (readAll(fd, &ret, 4) != 4) return -1;
    return ret;
}

inline bool writeInt(int fd, int32_t val) {
    return SOCKFUNC(write)(fd, &val, 4) == 4;
}

inline bool writePrefixed(int fd, std::string v) {
    if (!writeInt(fd, v.size())) return false;
    if (SOCKFUNC(write)(fd, &v[0], v.size()) != v.size()) return false;
    return true;
}

inline bool writeAttrs(int fd, std::vector<std::string> v) {
    int32_t totalLength = 0;

    for (std::string s : v) {
        totalLength += 4 + s.size();
    }

    if (!writeInt(fd, totalLength)) return false;
    for (std::string s : v)
        writePrefixed(fd, s);

    return true;
}

inline std::pair<bool, std::string> readPrefixed(int fd) {
    int32_t len = readInt(fd);
    std::string buffer;
    buffer.resize(len);
    if (readAll(fd, &buffer[0], len) != len)
        return {false, ""};

    return {true, buffer};
}

inline std::vector<std::string> readAttrs(int fd) {
    std::vector<std::string> ret;
    int32_t totalLength = readInt(fd);
    if (totalLength == -1) {
        LOG("readAttrs: incomplete read (1)");
        return {};
    }
    while (totalLength > 0) {
        auto res = readPrefixed(fd);
        std::string s = res.second;
        totalLength -= s.length() + 4;
        if (!res.first) {
            LOG("readAttrs: incomplete read (2)");
            return {};
        }
        ret.push_back(std::move(s));
    }
    return ret;
}
