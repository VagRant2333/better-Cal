#include "option.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <ctime>

void Option::parseArgs(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "A:B:d:r:m:")) != -1)
    {
        switch (opt)
        {
        case 'A':
            after = std::max(0, std::stoi(optarg));
            break;
        case 'B':
            before = std::max(0, std::stoi(optarg));
            break;
        case 'd':
        {
            // break down yyyy-mm
            std::string s(optarg);
            auto pos = s.find('-');
            if (pos == std::string::npos)
            {
                std::cerr << "Invalid -d format, expeceted yyyy-mm" << std::endl;
                std::exit(1);
            }
            year = std::stoi(s.substr(0, pos));
            month = std::stoi(s.substr(pos + 1));
            break;
        }
        case 'r':
            rowNum = std::max(1, std::stoi(optarg));
            break;
        case 'm':
            month = std::stoi(optarg);
            break;
        default:
            std::cerr << "Usage: cal [-A months] [-B months] [-d yyyy-mm] [-r rownum] [-m month] [yyyy]" << std::endl;
            std::exit(1);
        }
    }

    // if there is a remaining number in optarg, see it as year
    if (optind < argc)
    {
        year = std::stoi(argv[optind++]);
        month = 0;
    }

    if (year <= 0)
    {
        std::time_t t = std::time(nullptr);
        std::tm *now = std::localtime(&t);
        year = now->tm_year + 1900;
        if (month == 0)
        {
            month = now->tm_mon + 1;
        }
    }
    if (month < 0 || month > 12)
    {
        std::cerr << "Month must be in [1, 12]" << std::endl;
        std::exit(1);
    }
    if (month == 0 && before + after > 0)
    {
        // for the whole year, don't support before/after
        before = after = 0;
    }
}