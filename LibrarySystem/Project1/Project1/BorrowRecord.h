#pragma once
//作用：一条借阅记录=谁借的哪本书，什么时候，何时还，还了没有
//组合：BorrowRecord 包含 Date 对象作为成员变量 Date m_borrowDate;Date m_dueDate;
#include<string>
#include"Date.h"
using namespace std;

class BorrowRecord {
public:
	BorrowRecord();
    // 【重载构造】给全信息，直接生成一条完整记录。
    //   borrowDays 表示"可借天数"，默认 30 天。
    //   内部会算出 dueDate = borrowDate + borrowDays。
    //  注意初始化列表里对环境对象成员的初始化写法：
    //   m_dueDate(borrowDate.addDays(borrowDays))
    //   这是在"进入本类构造函数体之前"就把内嵌对象构造好。
    BorrowRecord(const string& isbn, const Date& borrowData, int borrowDays = 30);

    string getIsbn()const { return m_isbn; }
    Date getBorrowDate()const { return m_borrowDate; }
    Date getDueDate()const { return m_dueDate; }
    bool isReturned()const { return m_returned; }
    
    bool markReturned() { m_returned = true; return true; }
    
    //逾期，判断逾期，后面七阶段补上罚款金额，逾期记录统计
    bool isOverdue(const Date& today)const;
    //逾期天数
    int getOverdueDays(const Date& today)const;
    //罚款金额=逾期天数*每日罚款；未逾期=0
    double getFine(const Date& today, double finePerDay = 0.5)const;

    void print(const Date& today)const;

private:
    string m_isbn;//所借图书的ISBN，定位
    Date m_borrowDate;//组合，借出日期
    Date m_dueDate;//组合，应还日期
    bool m_returned;//是否已归还

};