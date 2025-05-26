#include "dateUtils.h"

namespace DateUtils
{
    bool isLeapYear(int y) noexcept
    {
        if (y % 400 == 0)
            return true;
        if (y % 100 == 0)
            return false;
        return (y % 4 == 0);
    }

    int daysInMonth(int y, int m) noexcept
    {
        if (m == 2 && isLeapYear(y))
            return 29;
        static constexpr int mdays[12] = {
            31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        return mdays[m - 1];
    }

    int firstWeekday(int y, int m) noexcept
    {
        // use Zeller's congruence
        if (m < 3)
        {
            y -= 1;
            m += 12;
        }
        int K = y % 100;
        int J = y / 100;
        int h = (1 + (13 * (m + 1) / 5 + K + K / 4 + J / 4 + 5 * J)) % 7;
        return (h + 6) % 7;
    }
}