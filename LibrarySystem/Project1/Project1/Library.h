#pragma once
//组合关系：class Library {
//           Book   m_books[100];     ← 图书馆"包含"图书
//           Reader m_readers[50];    ← 图书馆"包含"读者
//       };
//作用：图书馆扮演中介角色，联系Reader和Book
//       Library::borrowBook(readerId, isbn, today)
//          ├─ 按 isbn 从 m_books 里找到那本真书
//          ├─ 按 readerId 从 m_readers 里找到那个真读者
//          └─ 调用 reader->borrowBook(book, today)  ← 触发依赖关系
//Library::borrowBook 传的是馆藏数组里那本书的地址

//Library用对象数组（Book m_books[N]）书对象与图书馆同生共死,组合关系
//后面阶段四用基类指针数组，支持多态
#include<string>
#include"Book.h"
#include"Reader.h"
#include"Date.h"

class Library
{
public:
	Library();
	//馆藏管理
	//   传引用避免拷贝一个完整的 Book 对象；加 const 表示"我只是读它、不改它"。
   //   函数内部再把这本书**拷贝一份**放进馆藏数组 —— 这一步才是真正的"纳入"
	bool addBook(const Book& book);
	//按ISBN找书
	//返回值为Book*，返回是book话，外部改的是副本，改不动馆藏里的书，找不到返回nullptr
	//该指针会被传给Reader::borroweBook()
	Book* findBookByIsbn(const string& isbn);
	int getBookCount()const { return m_bookCount; }
	void showAllBooks() const;

	//读者管理
	bool addReader(const Reader& reader);
	Reader* findReaderById(const string& id);
	int getReaderCount()const { return m_readerCount; }
	void showAllReaders() const;

	//业务操作：借书/还书，中转站
	//定位读者、定位图书、然后**转交给 Reader 的成员函数**。
	//该类只负责找到人和书，不负责实现业务规则，借阅规则归属为读者，谁借书谁负责判断自己的额度
	bool borrowBook(const string& readerId,
		const string& isbn,
		const Date& today);
	bool returnBook(const std::string& readerId,
		const std::string& isbn,
		const Date& today);

	void print()const;

private:
	//组合关系，图书馆拥有图书数组
	//阶段四用vector,消除容量上限
	static const int MAX_BOOKS = 100;
	static const int MAX_READERS = 50;
	Book m_books[MAX_BOOKS];
	int m_bookCount;
	Reader m_readers[MAX_READERS];
	int m_readerCount;
};

