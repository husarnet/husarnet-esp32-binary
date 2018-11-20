#ifndef XMLRPC_H_
#define XMLRPC_H_
#include <memory>
#include <vector>
#include <string>
#include "util.h"
#include "port.h"
#include "ipaddress.h"

struct RpcValue {
    enum Kind {
        array,
        string,
        failure,
        integer
    };

    virtual std::string repr() = 0;
    virtual Kind kind() = 0;

    const std::vector<std::unique_ptr<RpcValue> >& getArray();
    const std::string& getString();
};

using RpcValuePtr = std::unique_ptr<RpcValue>;

struct RpcArray : RpcValue {
    std::vector<RpcValuePtr> arr;

    RpcArray() {}
    RpcArray(std::vector<RpcValuePtr> arr): arr(std::move(arr)) {}

    Kind kind() {
        return array;
    }

    std::string repr() {
        std::string s = "[";
        for (auto& item : arr)
            s += item->repr() + ", ";
        s += "]";
        return s;
    }
};

inline const std::vector<RpcValuePtr>& RpcValue::getArray() {
    static std::vector<RpcValuePtr> emptyArr;
    if (kind() != array) return emptyArr;

    return ((RpcArray*)this)->arr;
}

struct RpcString : RpcValue {
    RpcString(const std::string& s): str(s) {}
    RpcString() {}

    Kind kind() {
        return string;
    }

    std::string repr() {
        return "\"" + str + "\"";
    }

    std::string str;
};

inline const std::string& RpcValue::getString() {
    static std::string emptyString;
    if (kind() != string) return emptyString;

    return ((RpcString*)this)->str;
}
struct RpcInteger : RpcValue {
    RpcInteger(int64_t v): val(v) {}
    RpcInteger() {}

    Kind kind() {
        return integer;
    }

    std::string repr() {
        return std::to_string(val);
    }

    int64_t val = 0;
};


struct RpcFailure : RpcValue {
    RpcFailure(const std::string& msg): message(msg) {}

    Kind kind() {
        return failure;
    }

    std::string repr() {
        return "(failure: " + message + ")";
    }

    std::string message;
};

inline RpcValuePtr rpcFailure(std::string message) {
    return std::unique_ptr<RpcFailure>(new RpcFailure(message));
}

inline RpcValuePtr rpcString(std::string s) {
    return std::unique_ptr<RpcString>(new RpcString(s));
}

inline RpcValuePtr rpcInteger(int64_t v) {
    return std::unique_ptr<RpcInteger>(new RpcInteger(v));
}

inline void fillVec(std::vector<RpcValuePtr>& vec) {}

template <typename... T>
void fillVec(std::vector<RpcValuePtr>& vec, RpcValuePtr&& item, T... args) {
    vec.push_back(std::move(item));
    fillVec(vec, std::move(args)...);
}

template <typename... T>
inline std::vector<RpcValuePtr> rpcVec(T&&... rest) {
    std::vector<RpcValuePtr> vec;
    fillVec(vec, std::move(rest)...);
    return vec;
}

template <typename... T>
inline RpcValuePtr rpcArray(T&&... rest) {
    std::vector<RpcValuePtr> vec;
    fillVec(vec, std::move(rest)...);
    return std::unique_ptr<RpcValue>(new RpcArray(std::move(vec)));
}

using RpcCallback = std::function<RpcValuePtr(std::string name, std::vector<RpcValuePtr>&)>;

RpcValuePtr rpcCall(std::string url, std::string method, const std::vector<RpcValuePtr>& args);
void startRpcServer(int port, RpcCallback callback);
void startTcpServer(int port, std::function<void(int)> callback);
void runTcpServer(int port, std::function<void(int)> callback);
#endif
