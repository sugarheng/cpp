#pragma once
//读者类：
 //     组合关系:    class Reader
 //             {
 //                break m_records[10];读者包含借阅记录
 //             };
 //     依赖关系:    bool borrowBook(Book* book, const Date& today);
 //                  参数里出现了Book类指针

#include<string>
#include "Book.h"
#include"Date.h"
#include "BorrowRecord.h"

using namespace std;
class Reader {
public:
	Reader();
	Reader(const string& id,
		const string& name,
		const string& department,
		int maxBorrow = 5);
	string getId()const { return m_id; }
	string getname()const { return m_name; }
	string getDepartment()const { return m_department; }
	int getMaxBorrow()const { return m_maxBorrow; }
	int getBorrowCount()const { return m_borrowedCount; }
	
	//当前还能再借几本=上限-已借，判断能否借书
	int getRemainingQuota()const { return m_maxBorrow - m_borrowedCount; }

	//是否还有借书额度
	bool canBorrowMore()const { return m_borrowedCount < m_maxBorrow; }

	//该读者是否已借了某本ISBN的书（防止同一个人重复借同一本书）
	bool hasBorrowed(const string& isbn)const;

	//修改，带校验
	bool setName(const string& name);
	bool setDepartment(const string& department);
	bool setMaxBorrow(int maxBorrow);

	//依赖关系核心：借书/还书
	//参数用Book book值传递，拷贝构造函数执行，生成一本全新的副本，我修改的是副本，不是图书馆的书，主程序的那本书的m_availableCount不会减少
	//因此参数用Book *book传指针，传原Book对象的内存地址，Reader可通过这个指针直接修改图书馆的书的真实状态
	//借书要登记借书时间，还书要登记还书时间，不把系统时间写死在Reader里面，时间是外部传入
	//单一职责原则;Reader只处理借书业务，不获取系统当前时间
	//返回值为bool，借书失败（额度用完，书已借完，状态不合法）
	bool borrowBook(Book* book, const Date& today);
	bool returnBook(Book* book, const Date& today);

	//输出
	void print() const;
	void printRecords(const Date& today) const;   // 打印该读者全部借阅记录
		
private:
	string m_id;
	string m_name;
	string m_department;//院系
	int m_maxBorrow;//最大可借数量
	int m_borrowedCount;//当前已借数量

	// 组合关系 ：读者"包含"一组借阅记录
	// 用固定数组是为了实验二还没用到容器，先用数组把逻辑做通；
	// 阶段五用 std::vector 替换掉它，解决"10 条上限"的弊端。
	BorrowRecord m_records[10];
	int m_recordCount;    // 已使用的记录条数

	// 【私有辅助】找一条"未归还且 ISBN 匹配"的记录，返回下标（找不到返回 -1）
	//   为什么放私有是因为外部不需要知道记录存在数组的哪一格，
	//   这是纯粹的内部实现细节。
	int findActiveRecord(const std::string& isbn) const;
};



