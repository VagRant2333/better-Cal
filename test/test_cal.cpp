#include <gtest/gtest.h>
#include "option.h"  // 假设这是您的选项解析类
#include <ctime>
#include <sstream>

// 重定向 std::cerr 以捕获错误消息
class CerrRedirector {
public:
    CerrRedirector() {
        std::cerr.rdbuf(buffer_.rdbuf());
    }
    std::string getOutput() {
        return buffer_.str();
    }
private:
    std::stringstream buffer_;
};

// 设置当前时间为2025年5月（模拟环境）
void setMockCurrentTime(Option& opt) {
    time_t now;
    struct tm mockTime = {0};
    mockTime.tm_year = 2025 - 1900;  // 2025年
    mockTime.tm_mon = 4;             // 5月（0-based）
    now = mktime(&mockTime);
    // 假设 Option 类有方法可以设置当前时间
    // 如果没有，您需要在 parseArgs 中处理模拟时间
}

// 测试整年日历
TEST(OptionTest, ParseArgsFullYear) {
    Option opt;
    setMockCurrentTime(opt);
    const char* argv[] = {"cal", "2025"};
    int argc = 2;
    opt.parseArgs(argc, const_cast<char**>(argv));
    EXPECT_EQ(opt.year, 2025);
    EXPECT_EQ(opt.month, 0);  // 整年
    EXPECT_EQ(opt.rowNum, 3);
}

// 测试自定义行数
TEST(OptionTest, ParseArgsCustomRowNum) {
    Option opt;
    setMockCurrentTime(opt);
    const char* argv[] = {"cal", "-r", "4", "2025"};
    int argc = 4;
    opt.parseArgs(argc, const_cast<char**>(argv));
    EXPECT_EQ(opt.year, 2025);
    EXPECT_EQ(opt.month, 0);
    EXPECT_EQ(opt.rowNum, 4);
}

// 测试指定月份
TEST(OptionTest, ParseArgsSpecifyMonth) {
    Option opt;
    setMockCurrentTime(opt);
    const char* argv[] = {"cal", "-m", "5"};
    int argc = 3;
    opt.parseArgs(argc, const_cast<char**>(argv));
    EXPECT_EQ(opt.year, 2025);
    EXPECT_EQ(opt.month, 5);
    EXPECT_EQ(opt.before, 0);
    EXPECT_EQ(opt.after, 0);
}

// 测试之后月份
TEST(OptionTest, ParseArgsAfterMonths) {
    Option opt;
    setMockCurrentTime(opt);
    const char* argv[] = {"cal", "-A", "2"};
    int argc = 3;
    opt.parseArgs(argc, const_cast<char**>(argv));
    EXPECT_EQ(opt.year, 2025);
    EXPECT_EQ(opt.month, 5);
    EXPECT_EQ(opt.after, 2);
    EXPECT_EQ(opt.before, 0);
}

// 测试之前月份
TEST(OptionTest, ParseArgsBeforeMonths) {
    Option opt;
    setMockCurrentTime(opt);
    const char* argv[] = {"cal", "-B", "2"};
    int argc = 3;
    opt.parseArgs(argc, const_cast<char**>(argv));
    EXPECT_EQ(opt.year, 2025);
    EXPECT_EQ(opt.month, 5);
    EXPECT_EQ(opt.before, 2);
    EXPECT_EQ(opt.after, 0);
}

// 测试默认行为
TEST(OptionTest, ParseArgsDefault) {
    Option opt;
    setMockCurrentTime(opt);
    const char* argv[] = {"cal"};
    int argc = 1;
    opt.parseArgs(argc, const_cast<char**>(argv));
    EXPECT_EQ(opt.year, 2025);
    EXPECT_EQ(opt.month, 5);
    EXPECT_EQ(opt.before, 0);
    EXPECT_EQ(opt.after, 0);
    EXPECT_EQ(opt.rowNum, 3);
}

// 测试指定年月
TEST(OptionTest, ParseArgsSpecifyYearMonth) {
    Option opt;
    setMockCurrentTime(opt);
    const char* argv[] = {"cal", "-d", "2025-08"};
    int argc = 3;
    opt.parseArgs(argc, const_cast<char**>(argv));
    EXPECT_EQ(opt.year, 2025);
    EXPECT_EQ(opt.month, 8);
    EXPECT_EQ(opt.before, 0);
    EXPECT_EQ(opt.after, 0);
}

// 测试无效月份
TEST(OptionTest, InvalidMonth) {
    Option opt;
    setMockCurrentTime(opt);
    const char* argv[] = {"cal", "-m", "13"};
    int argc = 3;
    CerrRedirector redirector;
    testing::internal::CaptureStderr();
    opt.parseArgs(argc, const_cast<char**>(argv));
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_TRUE(output.find("Month must be in [1, 12]") != std::string::npos);
}

// 测试无效行数
TEST(OptionTest, InvalidRowNum) {
    Option opt;
    setMockCurrentTime(opt);
    const char* argv[] = {"cal", "-r", "0", "2025"};
    int argc = 4;
    CerrRedirector redirector;
    testing::internal::CaptureStderr();
    opt.parseArgs(argc, const_cast<char**>(argv));
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_TRUE(output.find("Row number must be positive") != std::string::npos);
}

// 测试无效 -d 格式
TEST(OptionTest, InvalidDFormat) {
    Option opt;
    setMockCurrentTime(opt);
    const char* argv[] = {"cal", "-d", "2025"};
    int argc = 3;
    CerrRedirector redirector;
    testing::internal::CaptureStderr();
    opt.parseArgs(argc, const_cast<char**>(argv));
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_TRUE(output.find("Invalid -d format, expected yyyy-mm") != std::string::npos);
}