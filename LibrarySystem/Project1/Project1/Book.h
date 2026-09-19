#pragma once
#include<string>
using namespace std;

enum class BookStatus {
	AVAILABLE,   // 在馆，可借（m_availableCount > 0）
	BORROWED,    // 全部借出（m_availableCount == 0）
	OFFLINE      // 已下架 / 停用
};

class Book {
private:
	string m_title;    //书名
	string m_isbn;    //用于分辨不同种类的书
	string m_publisher;    //出版社
	double m_price;    //价格
	int m_pages;   //页数
	int m_totalCount;    //馆藏总数
	int m_availableCount;    //当前可借书量，还有几本在架
	BookStatus m_status;   //可借状态，由m_totalCount,m_availableCount可知
	
	//私有辅助函数，只对内使用，外面看不到
	void refreshstatus();    //根据m_availableCount,刷新m_status

public:
	Book();
	Book(const std::string& title,
		const std::string& isbn,
		const std::string& publisher,
		double             price,
		int                pages,
		int                totalCount = 1);
	~Book() = default;

	string getTitle()     const { return m_title; }
	string getIsbn()      const { return m_isbn; }
	string getPublisher() const { return m_publisher; }
	double      getPrice()     const { return m_price; }
	int         getPages()     const { return m_pages; }
	int         getTotalCount()     const { return m_totalCount; }
	int         getAvailableCount() const { return m_availableCount; }
	BookStatus  getStatus()    const { return m_status; }

	//简单判断，是否可借
	bool isAvailable() const { return m_availableCount > 0; }

	bool setTitle(const string& title);
	bool setPublisher(const string& publisher);
	bool setPrice(double price);
	bool setPages(int pages);

	// 【数量相关操作】借出/归还/入库，内部会同步刷新 m_status。
	bool borrowOne();          // 借出一本，成功返回 true
	bool returnOne();          // 归还一本，成功返回 true
	bool addCopies(int n);     // 增加 n 本馆藏
	bool setOffline();         // 下架

	// 【ISBN 合法性验证】这是阶段一明确点名的功能。
	//   规则：去掉 '-' 后必须是 10 位或 13 位数字。
	static bool isValidIsbn(const std::string& isbn);

	// ---------- 输出 ----------
	void print() const;

};
