#ifndef ROSSERIAL_H_
#define ROSSERIAL_H_
#include "string_view.h"
#include <functional>

struct Parser {
    Parser();
    void feed(string_view data);
    bool parse();
    std::function<void(int,string_view)> callback;

private:
    std::string buffer;
};

#endif
