#include "gtest/gtest.h"

// Include headers of the classes to be tested
// Ensure your CMakeLists.txt is set up for correct include paths
#include "../src/option.h"
#include "../src/dateUtils.h"
#include "../src/calendar.h"

#include <vector>
#include <string>
#include <cstring>  // For strdup, strcpy, free (used in OptionTest)
#include <unistd.h> // For getopt, optind, optarg (used in OptionTest)
#include <ctime>    // For std::time, std::tm, std::localtime

// --- Tests for Option class (Argument Parsing) ---

// Helper to reset getopt's global state for each test involving parseArgs
void reset_getopt_state()
{
    optind = 1; // Reset optind for getopt. optarg is managed by getopt.
}

class OptionTest : public ::testing::Test
{
protected:
    Option opt;
    std::vector<char *> argv_vec; // To hold C-style arguments for getopt

    // Helper to create argv for parseArgs from a vector of strings
    void setup_argv(const std::vector<std::string> &args)
    {
        // Clean up previous argv_vec if any
        for (char *arg_char_ptr : argv_vec)
        {
            free(arg_char_ptr);
        }
        argv_vec.clear();

        for (const auto &arg_str : args)
        {
            // getopt might modify argv strings, so duplicate them
            argv_vec.push_back(strdup(arg_str.c_str()));
        }
    }

    void TearDown() override
    {
        for (char *arg_char_ptr : argv_vec)
        {
            free(arg_char_ptr); // Clean up duplicated strings
        }
        argv_vec.clear();
        reset_getopt_state(); // Ensure getopt is reset after each test
    }

    // Wrapper to call parseArgs more conveniently in tests
    void parse_options(const std::vector<std::string> &args_str_vec)
    {
        reset_getopt_state(); // Reset getopt before each parse operation
        setup_argv(args_str_vec);
        opt.parseArgs(static_cast<int>(argv_vec.size()), argv_vec.data());
    }
};

TEST_F(OptionTest, DefaultValues)
{
    parse_options({"cal"}); // Only program name

    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t); // [cite: 14]

    EXPECT_EQ(opt.year, now->tm_year + 1900); // Should default to current year [cite: 14]
    EXPECT_EQ(opt.month, 5);                  // Default month is 0 (indicating display whole year or current month based on other logic) [cite: 2]
    // it is May now
    EXPECT_EQ(opt.before, 0);                 // Default [cite: 2]
    EXPECT_EQ(opt.after, 0);                  // Default [cite: 2]
    EXPECT_EQ(opt.rowNum, 3);                 // Default [cite: 2]
}

TEST_F(OptionTest, ParseYearOnly)
{
    parse_options({"cal", "2023"});
    EXPECT_EQ(opt.year, 2023);
    EXPECT_EQ(opt.month, 0); // When only year is given, month is set to 0 [cite: 13]
}

TEST_F(OptionTest, ParseMonthOnlyWithM)
{
    parse_options({"cal", "-m", "5"});
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    EXPECT_EQ(opt.year, now->tm_year + 1900); // Year defaults to current
    EXPECT_EQ(opt.month, 5);                  // [cite: 10]
}

TEST_F(OptionTest, ParseDateWithD)
{
    parse_options({"cal", "-d", "2024-07"});
    EXPECT_EQ(opt.year, 2024); // [cite: 8]
    EXPECT_EQ(opt.month, 7);   // [cite: 9]
}

TEST_F(OptionTest, ParseBeforeAndAfterWithDate)
{
    parse_options({"cal", "-d", "2024-07", "-B", "2", "-A", "3"});
    EXPECT_EQ(opt.year, 2024); // [cite: 8]
    EXPECT_EQ(opt.month, 7);   // [cite: 9]
    EXPECT_EQ(opt.before, 2);  // [cite: 5]
    EXPECT_EQ(opt.after, 3);   // [cite: 4]
}

TEST_F(OptionTest, ParseRowNum)
{
    parse_options({"cal", "-r", "4"});
    EXPECT_EQ(opt.rowNum, 4); // [cite: 9]
}

TEST_F(OptionTest, BeforeAfterResetIfMonthIsZero)
{
    // Tests the behavior of: if (month == 0 && before + after > 0) { before = after = 0; } [cite: 16]
    // This happens if "cal YYYY -A 2" or "cal -A 2" (current year)
    parse_options({"cal", "-A", "2"}); // opt.month will be 0, opt.after will be 2 initially
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    EXPECT_EQ(opt.year, now->tm_year + 1900); // Defaults to current year
    EXPECT_EQ(opt.month, 5);                  // Month is now 5
    EXPECT_EQ(opt.after, 2);                  // Gets reset because month is 0 [cite: 16]
    // -A 2 means after is 2
    EXPECT_EQ(opt.before, 0);                 // Stays 0
}

TEST_F(OptionTest, NegativeValuesForBeforeAfterRowNum)
{
    // std::max(0, std::stoi(optarg)) is used for -A, -B [cite: 4, 5]
    // std::max(1, std::stoi(optarg)) is used for -r [cite: 9]
    parse_options({"cal", "-A", "-2", "-B", "-3", "-r", "-1", "-m", "6"});
    EXPECT_EQ(opt.after, 0);
    EXPECT_EQ(opt.before, 0);
    EXPECT_EQ(opt.rowNum, 1); // rowNum is max(1, ...)
    EXPECT_EQ(opt.month, 6);
}

// --- Tests for DateUtils ---

TEST(DateUtilsTest, IsLeapYear)
{
    EXPECT_TRUE(DateUtils::isLeapYear(2000));  // Divisible by 400 [cite: 18]
    EXPECT_FALSE(DateUtils::isLeapYear(1900)); // Divisible by 100 but not 400 [cite: 19]
    EXPECT_TRUE(DateUtils::isLeapYear(2024));  // Divisible by 4 but not 100 [cite: 20]
    EXPECT_FALSE(DateUtils::isLeapYear(2023)); // Not divisible by 4
}

TEST(DateUtilsTest, DaysInMonth)
{
    EXPECT_EQ(DateUtils::daysInMonth(2023, 1), 31);  // Jan [cite: 21, 22]
    EXPECT_EQ(DateUtils::daysInMonth(2023, 2), 28);  // Feb, non-leap year [cite: 21, 22]
    EXPECT_EQ(DateUtils::daysInMonth(2024, 2), 29);  // Feb, leap year [cite: 20, 21]
    EXPECT_EQ(DateUtils::daysInMonth(2023, 4), 30);  // Apr [cite: 21, 22]
    EXPECT_EQ(DateUtils::daysInMonth(2023, 12), 31); // Dec
}

TEST(DateUtilsTest, FirstWeekday)
{
    // Expected: Su=0, Mo=1, Tu=2, We=3, Th=4, Fr=5, Sa=6
    // Zeller's calculation: (h + 6) % 7 [cite: 25]
    EXPECT_EQ(DateUtils::firstWeekday(2023, 1), 0); // Jan 1, 2023 is Sunday
    EXPECT_EQ(DateUtils::firstWeekday(2023, 2), 3); // Feb 1, 2023 is Wednesday
    EXPECT_EQ(DateUtils::firstWeekday(2025, 5), 4); // May 1, 2025 is Thursday
    EXPECT_EQ(DateUtils::firstWeekday(2024, 2), 4); // Feb 1, 2024 is Thursday (leap year)
    EXPECT_EQ(DateUtils::firstWeekday(1900, 1), 1); // Jan 1, 1900 is Monday
    EXPECT_EQ(DateUtils::firstWeekday(2000, 1), 6); // Jan 1, 2000 is Saturday
}

// --- Tests for Calendar class ---

class CalendarTest : public ::testing::Test
{
protected:
    Option opt;
    Calendar *cal_ptr; // Using a pointer to recreate Calendar with different options per test

    void ReinitializeCalendar()
    {
        delete cal_ptr; // Delete previous instance if any
        cal_ptr = new Calendar(opt);
    }

    void SetUp() override
    {
        cal_ptr = nullptr; // Initialize to nullptr
        // 'opt' is default-constructed here. Specific tests will set 'opt' and call ReinitializeCalendar.
    }

    void TearDown() override
    {
        delete cal_ptr;
    }
};

TEST_F(CalendarTest, RenderSingleMonth_Jan2023)
{
    opt.year = 2023; // Not used directly by renderMonth, but good for context
    opt.month = 1;   // Not used directly by renderMonth
    ReinitializeCalendar();

    // Call the private method renderMonth via FRIEND_TEST access
    std::vector<std::string> month_lines = cal_ptr->renderMonth(2023, 1);

    ASSERT_EQ(month_lines.size(), 8U); // Title, Header, 6 lines for days (padded) [cite: 37]
    // Assuming the fix for 20-char width lines:
    EXPECT_EQ(month_lines[0], "    January 2023    "); // Centered in 20 chars [cite: 29, 30]
    EXPECT_EQ(month_lines[1], "Su Mo Tu We Th Fr Sa"); // [cite: 30]
    // Jan 1, 2023 is Sunday (firstWeekday = 0) [cite: 31]
    EXPECT_EQ(month_lines[2], " 1  2  3  4  5  6  7"); // [cite: 33]
    EXPECT_EQ(month_lines[3], " 8  9 10 11 12 13 14");
    EXPECT_EQ(month_lines[4], "15 16 17 18 19 20 21");
    EXPECT_EQ(month_lines[5], "22 23 24 25 26 27 28");
    EXPECT_EQ(month_lines[6], "29 30 31            "); // Padded to 20 chars [cite: 35, 36, 37]
    EXPECT_EQ(month_lines[7], "                    "); // Padded empty line [cite: 37]
}

TEST_F(CalendarTest, RenderSingleMonth_Feb2024_Leap)
{
    opt.year = 2024;
    opt.month = 2;
    ReinitializeCalendar();

    std::vector<std::string> month_lines = cal_ptr->renderMonth(2024, 2); // February 2024 (Leap Year)
    // Feb 1, 2024 is Thursday (firstWeekday = 4)
    // Days in month = 29
    ASSERT_EQ(month_lines.size(), 8U);
    EXPECT_EQ(month_lines[0], "   February 2024    ");
    EXPECT_EQ(month_lines[1], "Su Mo Tu We Th Fr Sa");
    EXPECT_EQ(month_lines[2], "             1  2  3"); // Th Fr Sa
    EXPECT_EQ(month_lines[3], " 4  5  6  7  8  9 10");
    EXPECT_EQ(month_lines[4], "11 12 13 14 15 16 17");
    EXPECT_EQ(month_lines[5], "18 19 20 21 22 23 24");
    EXPECT_EQ(month_lines[6], "25 26 27 28 29      "); // Padded
    EXPECT_EQ(month_lines[7], "                    ");
}

TEST_F(CalendarTest, CollectMonths_SingleSpecifiedMonth)
{
    opt.year = 2023;
    opt.month = 5; // May 2023 [cite: 40]
    opt.before = 0;
    opt.after = 0;
    ReinitializeCalendar();

    std::vector<std::vector<std::string>> collected = cal_ptr->collectMonths();
    ASSERT_EQ(collected.size(), 1U);
    ASSERT_FALSE(collected[0].empty());
    EXPECT_EQ(collected[0][0], "      May 2023      "); // Centered "May 2023"
}

TEST_F(CalendarTest, CollectMonths_FullYear)
{
    opt.year = 2023;
    opt.month = 0; // Indicates full year [cite: 38]
    ReinitializeCalendar();

    std::vector<std::vector<std::string>> collected = cal_ptr->collectMonths();
    ASSERT_EQ(collected.size(), 12U);                    // [cite: 39]
    EXPECT_EQ(collected[0][0], "    January 2023    ");  // First month
    EXPECT_EQ(collected[11][0], "   December 2023    "); // Last month
}

TEST_F(CalendarTest, CollectMonths_BeforeAndAfter)
{
    opt.year = 2023;
    opt.month = 3;  // March 2023
    opt.before = 1; // February 2023
    opt.after = 1;  // April 2023
    ReinitializeCalendar();

    std::vector<std::vector<std::string>> collected = cal_ptr->collectMonths();
    ASSERT_EQ(collected.size(), 3U); // Feb, Mar, Apr [cite: 40]
    EXPECT_EQ(collected[0][0], "   February 2023    ");
    EXPECT_EQ(collected[1][0], "     March 2023     ");
    EXPECT_EQ(collected[2][0], "     April 2023     ");
}

TEST_F(CalendarTest, CollectMonths_BeforeAndAfterCrossingYearBoundary)
{
    opt.year = 2023;
    opt.month = 1;  // January 2023
    opt.before = 1; // Should be December 2022
    opt.after = 1;  // Should be February 2023
    ReinitializeCalendar();

    std::vector<std::vector<std::string>> collected = cal_ptr->collectMonths();
    // Logic for year/month adjustment [cite: 40, 41, 42, 43]
    ASSERT_EQ(collected.size(), 3U);                    // Dec 2022, Jan 2023, Feb 2023
    EXPECT_EQ(collected[0][0], "   December 2022    "); // Title for Dec 2022
    EXPECT_EQ(collected[1][0], "    January 2023    "); // Title for Jan 2023
    EXPECT_EQ(collected[2][0], "   February 2023    "); // Title for Feb 2023
}

// Entry point for running the tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}