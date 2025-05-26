#pragma once
#include <vector>
#include <string>
#include "option.h"

class Calendar
{
    // this class is to generate and print cal
public:
    explicit Calendar(const Option &opt);

    void print() const;

private:
    Option opts_;

    std::vector<std::string> renderMonth(int year, int month) const;

    std::vector<std::vector<std::string>> collectMonths() const;
};