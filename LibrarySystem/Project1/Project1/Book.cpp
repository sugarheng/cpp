#include "Book.h"
#include <iostream>
#include <cctype>
#include <iomanip>

Book::Book() :
	m_title("待录"),
	m_isbn(""),
	m_publisher("出版社"),
	m_price(0.0),
	m_pages(0),
	m_totalCount(0),
	m_availableCount(0),
	m_status(BookStatus::OFFLINE)
{
	//默认构造了一本空书，状态为下架
}

//重载构造函数
Book::Book(const std::string& title,
    const std::string& isbn,
    const std::string& publisher,
    double             price,
    int                pages,
    int                totalCount)
    : m_title(title)
    , m_isbn(isbn)
    , m_publisher(publisher)
    , m_price(price < 0 ? 0.0 : price)          // 价格不能为负
    , m_pages(pages < 0 ? 0 : pages)            // 页数不能为负
    , m_totalCount(totalCount < 0 ? 0 : totalCount)
    , m_availableCount(totalCount < 0 ? 0 : totalCount)  // 新书全部可借
    , m_status(BookStatus::AVAILABLE)
{
    refreshStatus();
}

//修改操作，先判断参数是否合法，不合法返回false，不动原状态
bool Book::setTitle(const std::string& title) {
    if (title.empty())return false;
    m_title = title;
    return true;
}

bool Book::setPublisher(const std::string& publisher) {
    if (publisher.empty())return false;
    m_publisher = publisher;
    return true;
}

bool Book::setPrice(double price) {
    if (price < 0) return false;       // 价格不能为负
    m_price = price;
    return true;
}

bool Book::setPages(int pages) {
    if (pages <= 0) return false;      // 页数必须为正
    m_pages = pages;
    return true;
}

//数量操作，借出/归还/入库/下架

//借出
bool Book::borrowOne() {
    if (m_availableCount <= 0)return false;
    --m_availableCount;
    refreshStatus();
    return true;
}

//归还，不能超过馆藏总数，注意没借出的不能算归还
bool Book::returnOne() {
    if (m_availableCount >= m_totalCount)return false;
    ++m_availableCount;
    refreshStatus();
    return true;
}

//增加n本馆藏，总量和可借数同步加
bool Book::addCopies(int n) {
    if (n <= 0)return false;
    m_totalCount += n;
    m_availableCount += n;
    refreshStatus();          // 若原状态是"已全部借出"，补货后应恢复"在馆可借"
    return true;
}

bool Book::setOffline() {
    m_status = BookStatus::OFFLINE;   // 注意是赋值 '='，不是比较 '=='
    return true;
}

/*ISBN合法性检验：
允许10或13位，允许中间出现'-'分隔，但'-'不计入位数
去掉'-'全是数字，10位ISBN末位可以是x
*/
bool Book::isValidIsbn(const std::string& isbn) {
    if (isbn.empty())return false;
    std::string digits;
    for (char c : isbn) {
        if (c == '-')continue;
        if (std::isdigit(static_cast<unsigned char>(c))) {
            digits.push_back(c);
        }
        else if ((c == 'X' || c == 'x') && digits.size() == 9) {
            digits.push_back('X');                    // 10 位 ISBN 的校验位
        }
        else {
            return false;                             // 出现非法字符
        }
    }
    // 位数校验必须放在循环之后，否则第一个字符就会直接返回
    return digits.size() == 10 || digits.size() == 13;
}

//内部辅助：状态刷新，单一职责
void Book::refreshStatus() {
    if (m_status == BookStatus::OFFLINE)return;
    m_status = (m_availableCount > 0) ? BookStatus::AVAILABLE
        : BookStatus::BORROWED;
}

//输出
//将枚举转为中文方便展示，放匿名namespace只在本文件可见。不会与别的.cpp里的同名函数起冲突
namespace{
    const char* statusToText(BookStatus s) {
        switch (s) {
        case BookStatus::AVAILABLE:return"在馆可借";
        case BookStatus::BORROWED:  return "已全部借出";
        case BookStatus::OFFLINE:   return "已下架";
        }
        return "未知";
    }
}
void Book::print()const {
    // ISBN 为空时显示"未登记"
    std::string isbnShow = m_isbn.empty()
        ? std::string("（未登记）")
        : m_isbn + (isValidIsbn(m_isbn) ? "  [格式合法]" : "  [格式非法]");

    std::cout << "----------------------------------------\n"
        << "书名    ：" << m_title << '\n'
        << "ISBN    ：" << isbnShow << '\n'
        << "出版社  ：" << m_publisher << '\n'
        << "价格    ：￥" << std::fixed << std::setprecision(2) << m_price << '\n'
        << "页数    ：" << m_pages << " 页\n"
        << "馆藏总量：" << m_totalCount << " 本\n"
        << "可借数量：" << m_availableCount << " 本\n"
        << "状态    ：" << statusToText(m_status) << '\n'
        << "----------------------------------------\n";
}
