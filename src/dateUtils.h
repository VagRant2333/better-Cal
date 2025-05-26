#pragma once
#include <array>
#include <string>

namespace DateUtils
{
    bool isLeapYear(int year) noexcept;
    int daysInMonth(int year, int month) noexcept;
    int firstWeekday(int year, int month) noexcept;

    inline const std::array<std::string, 12> monthNames = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"};
}