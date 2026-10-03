#include "BorrowRecord.h"
#include <iostream>
#include <iomanip>

namespace {
    const int DEFAULT_BORROW_DAYS = 30;    // 默认可借天数，与头文件里的默认参数一致
}
BorrowRecord::BorrowRecord()
    : m_isbn("")
    , m_borrowDate()
    , m_dueDate()
    , m_returned(false)
{
}
BorrowRecord::BorrowRecord(const std::string& isbn, const Date& borrowDate, int borrowDays)
    : m_isbn(isbn)
    , m_borrowDate(borrowDate)
    , m_dueDate(borrowDate.addDays(borrowDays > 0 ? borrowDays : DEFAULT_BORROW_DAYS))
    , m_returned(false)
{
}

// 逾期计算
bool BorrowRecord::isOverdue(const Date& today) const {
    return m_dueDate.daysBetween(today) > 0;
}

int BorrowRecord::getOverdueDays(const Date& today) const {
    long long days = m_dueDate.daysBetween(today);
    return days > 0 ? static_cast<int>(days) : 0;
}

// 罚款 = 逾期天数 × 每日单价；未逾期时为 0（因为天数已经是 0）
double BorrowRecord::getFine(const Date& today, double finePerDay) const {
    return getOverdueDays(today) * finePerDay;
}

// 输出
void BorrowRecord::print(const Date& today) const {
    std::cout << "    ISBN    ：" << (m_isbn.empty() ? "（无）" : m_isbn) << '\n'
        << "    借出日期：" << m_borrowDate.toString() << '\n'
        << "    应还日期：" << m_dueDate.toString() << '\n'
        << "    归还状态：" << (m_returned ? "已归还" : "未归还") << '\n';

    if (!m_returned && isOverdue(today)) {
        std::cout << "    逾期情况：已超期 " << getOverdueDays(today) << " 天"
            << "，应缴罚款 ￥" << std::fixed << std::setprecision(2)
            << getFine(today) << '\n';
    }
}
