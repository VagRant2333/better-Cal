
#include <gtest/gtest.h>
#include "../src/option.h"
#include "../src/dateUtils.h"
#include <ctime>

// 测试 Option 类的 parseArgs 函数
TEST(OptionTest, ParseArgsWithOptions) {
    Option opt;
    const char* argv[] = {"cal", "-A", "2", "-B", "1", "-d", "2025-05"};
    int argc = 7;
    opt.parseArgs(argc, const_cast<char**>(argv));  // 由于 parseArgs 期望 char*[]
    EXPECT_EQ(opt.year, 2025);
    EXPECT_EQ(opt.month, 5);
    EXPECT_EQ(opt.before, 1);
    EXPECT_EQ(opt.after, 2);
    EXPECT_EQ(opt.rowNum, 3);
}

// 测试默认参数（当前年月）
TEST(OptionTest, ParseArgsDefault) {
    Option opt;
    const char* argv[] = {"cal"};
    int argc = 1;
    opt.parseArgs(argc, const_cast<char**>(argv));

    // 获取当前年月
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    int current_year = now->tm_year + 1900;
    int current_month = now->tm_mon + 1;

    EXPECT_EQ(opt.year, current_year);
    EXPECT_EQ(opt.month, current_month);
    EXPECT_EQ(opt.before, 0);
    EXPECT_EQ(opt.after, 0);
    EXPECT_EQ(opt.rowNum, 3);
}

// 测试整年日历
TEST(OptionTest, ParseArgsFullYear) {
    Option opt;
    const char* argv[] = {"cal", "2025"};
    int argc = 2;
    opt.parseArgs(argc, const_cast<char**>(argv));
    EXPECT_EQ(opt.year, 2025);
    EXPECT_EQ(opt.month, 0);  // month=0 表示整年
    EXPECT_EQ(opt.before, 0);
    EXPECT_EQ(opt.after, 0);
    EXPECT_EQ(opt.rowNum, 3);
}

// 测试 DateUtils::isLeapYear 函数
TEST(DateUtilsTest, IsLeapYear) {
    EXPECT_TRUE(DateUtils::isLeapYear(2000));  // 世纪闰年
    EXPECT_FALSE(DateUtils::isLeapYear(1900)); // 非闰年
    EXPECT_TRUE(DateUtils::isLeapYear(2024));  // 普通闰年
    EXPECT_FALSE(DateUtils::isLeapYear(2023)); // 非闰年
}

// 测试 DateUtils::daysInMonth 函数
TEST(DateUtilsTest, DaysInMonth) {
    EXPECT_EQ(DateUtils::daysInMonth(2025, 1), 31); // 1月31天
    EXPECT_EQ(DateUtils::daysInMonth(2025, 2), 28); // 2025年2月28天（非闰年）
    EXPECT_EQ(DateUtils::daysInMonth(2024, 2), 29); // 2024年2月29天（闰年）
    EXPECT_EQ(DateUtils::daysInMonth(2025, 4), 30); // 4月30天
}

// 测试 DateUtils::firstWeekday 函数
TEST(DateUtilsTest, FirstWeekday) {
    // 2025年1月1日是星期三（3）
    EXPECT_EQ(DateUtils::firstWeekday(2025, 1), 3);
    // 2024年2月1日是星期四（4）
    EXPECT_EQ(DateUtils::firstWeekday(2024, 2), 4);
}

// 测试无效月份
TEST(OptionTest, InvalidMonth) {
    Option opt;
    const char* argv[] = {"cal", "-m", "13"};
    int argc = 3;
    testing::internal::CaptureStderr();
    opt.parseArgs(argc, const_cast<char**>(argv));
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_TRUE(output.find("Month must be in [1, 12]") != std::string::npos);
}

// 测试无效 -d 格式
TEST(OptionTest, InvalidDFormat) {
    Option opt;
    const char* argv[] = {"cal", "-d", "2025"};
    int argc = 3;
    testing::internal::CaptureStderr();
    opt.parseArgs(argc, const_cast<char**>(argv));
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_TRUE(output.find("Invalid -d format, expected yyyy-mm") != std::string::npos);
}