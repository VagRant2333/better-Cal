#include "calendar.h"
#include "dateUtils.h"
#include <format>
#include <iostream>

Calendar::Calendar(const Option &opt) : opts_{opt} {}

std::vector<std::string> Calendar::renderMonth(int y, int m) const
{
    std::vector<std::string> lines;
    // std::format doesnot support formatting tm
    // the following is illegal!
    // auto title = std::format("{:^20}", std::format("{} {}", std::format("{:%B}", std::tm{0, 0, 0, 1, m - 1, y - 1900}), y));
    auto title = DateUtils::monthNames[m - 1] + " " + std::to_string(y);
    lines.push_back(std::format("{:^20}", title));
    lines.push_back("Su Mo Tu We Th Fr Sa");

    int first = DateUtils::firstWeekday(y, m);
    int dim = DateUtils::daysInMonth(y, m);

    std::string week;
    for (int i = 0; i < first; ++i)
        week += "   ";
    for (int d = 1; d <= dim; ++d)
    {
        week += std::format("{:>2} ", d);
        if ((first + d) % 7 == 0 || d == dim)
        {
            lines.push_back(week);
            week.clear();
        }
    }
    while (lines.size() < 8)
        lines.emplace_back("");
    return lines;
}

std::vector<std::vector<std::string>> Calendar::collectMonths() const
{
    std::vector<std::vector<std::string>> all;
    if (opts_.month == 0)
    {
        for (int m = 1; m <= 12; ++m)
        {
            all.push_back(renderMonth(opts_.year, m));
        }
    }
    else
    {
        int start = opts_.month - opts_.before;
        int end = opts_.month + opts_.after;
        for (int mm = start; mm <= end; ++mm)
        {
            // if mm <= 0, then cal crosses 2 year
            int y = opts_.year + (mm - 1) / 12;
            int m = (mm - 1) % 12 + 1;
            if (m <= 0)
            {
                y -= 1;
                m += 12;
            }
            all.push_back(renderMonth(y, m));
        }
    }
    return all;
}

void Calendar::print() const
{
    auto months = collectMonths();
    int total = static_cast<int>(months.size());
    for (int i = 0; i < total; i += opts_.rowNum)
    {
        int end = std::min(i + opts_.rowNum, total);
        for (int row = 0; row < 8; ++row)
        {
            for (int j = i; j < end; ++j)
            {
                std::cout << months[j][row];
                if (j < end - 1)
                {
                    std::cout << "  ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}