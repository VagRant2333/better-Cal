#include "calendar.h"
#include "dateUtils.h"
#include <format>
#include <iostream>

Calendar::Calendar(const Option &opt) : opts_{opt} {}

std::vector<std::string> Calendar::renderMonth(int y, int m) const
{
    std::vector<std::string> lines;
    // auto title = std::format("{:^20}", std::format("{} {}", DateUtils::monthNames[m - 1], y)); // had formatting issues with tm
    auto month_name_str = DateUtils::monthNames[m - 1];
    auto title_str = month_name_str + " " + std::to_string(y);
    lines.push_back(std::format("{:^20}", title_str));

    lines.push_back("Su Mo Tu We Th Fr Sa");

    int first_day_weekday = DateUtils::firstWeekday(y, m); // 0=Sun, 1=Mon, ..., 6=Sat
    int days_in_month_count = DateUtils::daysInMonth(y, m);

    std::string current_week_line;
    // Add leading spaces for the first week
    // Each day slot is 2 chars, plus 1 space separator (except for the last day in the line)
    for (int i = 0; i < first_day_weekday; ++i)
    {
        current_week_line += "  ";
        if (i < 6)
        {
            current_week_line += " ";
        }
    }

    for (int day = 1; day <= days_in_month_count; ++day)
    {
        current_week_line += std::format("{:>2}", day);

        int weekday_for_current_day = (first_day_weekday + day - 1) % 7; // 0=Sun, ..., 6=Sat

        if (weekday_for_current_day < 6)
        {
            current_week_line += " ";
        }

        if (weekday_for_current_day == 6 || day == days_in_month_count)
        {
            if (day == days_in_month_count && weekday_for_current_day < 6)
            {
                for (int i = weekday_for_current_day + 1; i < 7; ++i)
                {
                    current_week_line += "  ";
                    if (i < 6)
                    {
                        current_week_line += " ";
                    }
                }
            }
            lines.push_back(current_week_line);
            current_week_line.clear();
        }
    }

    while (lines.size() < 8)
    {
        lines.emplace_back(std::string(20, ' '));
    }
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