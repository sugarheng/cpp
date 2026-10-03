#include "Date.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// 私有小工具：默认日期（相当于"空日期"的占位值）
namespace {
    const int DEFAULT_YEAR = 1970;
    const int DEFAULT_MONTH = 1;
    const int DEFAULT_DAY = 1;
}

// 构造

Date::Date()
    : m_year(DEFAULT_YEAR)
    , m_month(DEFAULT_MONTH)
    , m_day(DEFAULT_DAY)
{
}

Date::Date(int year, int month, int day)
    : m_year(DEFAULT_YEAR)
    , m_month(DEFAULT_MONTH)
    , m_day(DEFAULT_DAY)
{
    if (isValidDate(year, month, day)) {
        m_year = year;
        m_month = month;
        m_day = day;
    }
}

// 静态工具函数：闰年 / 每月天数 / 日期合法性
//   规则：(能被 4 整除 且 不能被 100 整除) 或 (能被 400 整除)。
//   例：2024 是闰年；1900 不是（被 100 整除但不被 400 整除）；2000 是。
bool Date::isLeapYear(int year) {
    if (year <= 0) return false;                 // 防御：年份必须是正数
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// 返回"某年某月有几天"，非法月份返回 0（不抛异常，交给调用方判断）。
//  2 月的天数必须依赖闰年判断，不能写成固定 28。
int Date::daysInMonth(int year, int month) {
    static const int DAYS[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (month < 1 || month > 12) return 0;       // 非法月份
    if (month == 2 && isLeapYear(year)) return 29;
    return DAYS[month - 1];
}

// 日期合法性 = 年正 + 月 1~12 + 日 1~该月天数
// 这里就是 main.cpp 中 setDate(2026,2,30) 会被拒绝的那道关卡
bool Date::isValidDate(int year, int month, int day) {
    if (year < 1 || year > 9999) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > daysInMonth(year, month)) return false;
    return true;
}


// setDate 只在全部合法时才写入，否则原样返回 false、不动任何成员。
//  先校验、后赋值的顺序很关键：如果先赋值再校验，
bool Date::setDate(int year, int month, int day) {
    if (!isValidDate(year, month, day)) return false;
    m_year = year;
    m_month = month;
    m_day = day;
    return true;
}

// 日期运算
// toDays() 是全部日期运算的地基。
//   思路：把"年/月/日"折算成"从 1970-01-01 起过了多少天"的一个整数，
//   之后比大小、算间隔就都变成整数加减法了，不用再逐月累加。
//   下面用的是 Howard Hinnant 的 days_from_civil 算法，
//   它把 400 年当作一个"纪元"来处理闰年周期，全程只有整数运算，
//   不会出现"闰年算错"或"负数年份取整方向错"这类问题。
long long Date::toDays() const {
    long long y = m_year;
    unsigned  m = static_cast<unsigned>(m_month);
    unsigned  d = static_cast<unsigned>(m_day);

    y -= (m <= 2);                                       // 把 1、2 月算作上一年的年末
    const long long era = (y >= 0 ? y : y - 399) / 400;  // 所在 400 年纪元
    const unsigned  yoe = static_cast<unsigned>(y - era * 400);      // 纪元内第几年 [0,399]
    // 原写法 (m > 2 ? -3u : 9u) 里出现了对无符号字面量取负的 -3u，
    //   MSVC 报 warning C4146（一元负运算符应用于无符号类型，结果仍为无符号类型）。
    //   改成减法分支 (m - 3u) 后数学含义完全不变：
    //   m 是月份且已确认大于 2，减去 3 不可能下溢。
    const unsigned  doy = (153u * ((m > 2u) ? (m - 3u) : (m + 9u)) + 2u) / 5u + d - 1u; // 年内第几天
    const unsigned  doe = yoe * 365u + yoe / 4u - yoe / 100u + doy;  // 纪元内第几天
    return era * 146097LL + static_cast<long long>(doe) - 719468LL;  // 相对 1970-01-01
}

// 逆运算：把"天数序号"还原成"年/月/日"
// 放在匿名 namespace 里，只在本文件可见，不污染其它翻译单元
namespace {
    void civilFromDays(long long z, int& year, int& month, int& day) {
        z += 719468;
        const long long era = (z >= 0 ? z : z - 146096) / 146097;
        const unsigned  doe = static_cast<unsigned>(z - era * 146097);
        const unsigned  yoe = (doe - doe / 1460u + doe / 36524u - doe / 146096u) / 365u;
        const long long y = static_cast<long long>(yoe) + era * 400;
        const unsigned  doy = doe - (365u * yoe + yoe / 4u - yoe / 100u);
        const unsigned  mp = (5u * doy + 2u) / 153u;
        const unsigned  d = doy - (153u * mp + 2u) / 5u + 1u;
        const unsigned  m = (mp < 10u) ? (mp + 3u) : (mp - 9u);

        year = static_cast<int>(y + (m <= 2 ? 1 : 0));
        month = static_cast<int>(m);
        day = static_cast<int>(d);
    }
}

// ★修正9-g：daysBetween 的方向按 Date.h 的约定实现 —— a.daysBetween(b) = b - a。
//   即"从 a 走到 b 需要多少天"，结果可正可负：
//       9-19 .daysBetween(10-19) = +30   → 30 天后到期
//       应还日 .daysBetween(今天) > 0     → 今天已超期
//   方向搞反的话，逾期判断会整体反过来（按期归还被判成逾期），
//   这是日期类最经典的一个 bug，所以这里严格照注释实现。
long long Date::daysBetween(const Date& other) const {
    return other.toDays() - toDays();
}

// 加 n 天（n 可为负），返回新日期，不修改自己
Date Date::addDays(int n) const {
    int y = 0, m = 0, d = 0;
    civilFromDays(toDays() + n, y, m, d);
    return Date(y, m, d);
}

// 自身是否是合法日期（默认构造后、或被人为破坏后用它自查）
bool Date::isValid() const {
    return isValidDate(m_year, m_month, m_day);
}

//   月/日要固定两位：2026-9-19 应输出 "2026-09-19"。
//   不补齐的话，既不好看，还会让"2026-1-19"和"2026-11-9"这类字符串
//   在按字典序比较时出现 1 月大于 11 月的荒谬结果。
//   setw(2)/setfill('0') 只对紧随其后的那一次输出生效，所以要写两遍。
string Date::toString() const {
    std::ostringstream oss;
    oss << m_year << '-'
        << std::setw(2) << std::setfill('0') << m_month << '-'
        << std::setw(2) << std::setfill('0') << m_day;
    return oss.str();
}

void Date::print() const {
    std::cout << toString();
}
