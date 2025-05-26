#pragma once
#include <string>

class Option
{
public:
    int year{0};
    int month{0};
    int before{0};
    int after{0};
    int rowNum{3};

    void parseArgs(int argc, char *argv[]);
};