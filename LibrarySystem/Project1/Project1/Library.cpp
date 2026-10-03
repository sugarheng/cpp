#include "Library.h"
#include <iostream>
#include"Book.h"
//构造函数
//组合关系：进入函数体之前，下面这些内嵌对象已经被自动创建完毕：
//       m_books[0] ~ m_books[99]     → 调用 100 次 Book()
//       m_readers[0] ~ m_readers[49] → 调用 50 次 Reader()
//   而每个 Reader 里又内嵌了 10 个 BorrowRecord，
//   每个 BorrowRecord 里又内嵌了 2 个 Date……
Library::Library()
    : m_bookCount(0)
    , m_readerCount(0)
{
}

//馆藏管理
bool Library::addBook(const Book& book) {
    if (m_bookCount >= MAX_BOOKS) {
        cout << "  [警告] 馆藏容量已满（上限 " << MAX_BOOKS << " 种）。\n";
        return false;
    }
    //组合的实质作用：把外部传进来的书拷贝进馆藏数组，从此这本书属于图书馆，生命周期与图书馆绑定
    m_books[m_bookCount] = book;
    ++m_bookCount;
    return true;
}
//按ISBN查找，返回馆藏那本真书地址
Book* Library::findBookByIsbn(const string& isbn) {
    for (int i = 0; i < m_bookCount; ++i) {
        if (m_books[i].getIsbn() == isbn) {
            return &m_books[i];        // ★返回真书地址
        }
    }
    return nullptr;                    // 馆里没有这本
}
void Library::showAllBooks() const {
    if (m_bookCount == 0) {
        std::cout << "  （馆藏为空）\n";
        return;
    }
    for (int i = 0; i < m_bookCount; ++i) {
        std::cout << "  [" << (i + 1) << "] ";
        m_books[i].print();
    }
}

    //读者管理
    bool Library::addReader(const Reader & reader) {
        if (m_readerCount >= MAX_READERS) {
            std::cout << "  [警告] 读者容量已满（上限 " << MAX_READERS << " 人）。\n";
            return false;
        }
        m_readers[m_readerCount] = reader;
        ++m_readerCount;
        return true;
    }
    Reader* Library::findReaderById(const std::string& id) {
        for (int i = 0; i < m_readerCount; ++i) {
            if (m_readers[i].getId() == id) {
                return &m_readers[i];      // ★ 同样返回真读者地址
            }
        }
        return nullptr;
    }
    void Library::showAllReaders() const {
        if (m_readerCount == 0) {
            std::cout << "  （暂无注册读者）\n";
            return;
        }
        for (int i = 0; i < m_readerCount; ++i) {
            std::cout << "  [" << (i + 1) << "] ";
            m_readers[i].print();
        }
    }

    //业务操作：借书/还书的中转
    //职责分层;Library只做定位（找人，找书)，不重复实现借阅规则
    //判断能不能借：Reader::borrowBook()
    bool Library::borrowBook(const string& readerId,
        const string& isbn,
        const Date& today) {
            cout << "\n>> 借书请求：读者[" << readerId << "] 借 图书[" << isbn << "]\n";
            //定位读者
            Reader* reader = findReaderById(readerId);
            if (reader == nullptr) {
                cout << "  [失败] 未找到该读者：" << readerId << '\n';
                return false;
            }
            //定位图书（找到馆藏那本书的地址）
            Book* book = findBookByIsbn(isbn);
            if (book == nullptr){
                cout << "  [失败] 馆藏中没有该书：" << isbn << '\n';
                return false;
            }
            //把两个指针交给Reader触发依赖关系
            //reader 的成员函数接收 book 指针作为参数 → Reader 依赖 Book。
            return reader->borrowBook(book, today);
    }
    bool Library::returnBook(const string& readerId,
        const string& isbn,
        const Date& today) {
        cout << "\n>> 还书请求：读者[" << readerId << "] 还 图书[" << isbn << "]\n";
        Reader* reader = findReaderById(readerId);
        if (reader == nullptr) {
            cout << "[失败]未找到该读者" << readerId << endl;
            return false;
        }
        Book* book = findBookByIsbn(isbn);
        if (book == nullptr) {
            std::cout << "  [失败] 馆藏中没有该书：" << isbn << endl;
            return false;
        }
        return reader->returnBook(book, today);
    }

    //输出
    void Library::print()const {
        std::cout << "========================================\n"
            << "  图书馆当前状态\n"
            << "  馆藏种类：" << m_bookCount << " 种\n"
            << "  注册读者：" << m_readerCount << " 人\n"
            << "========================================\n";
    }
       
