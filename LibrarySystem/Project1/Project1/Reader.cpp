#include<iostream>
#include<iomanip>
#include"Reader.h"
// ★修正1：头文件名写错成"BookRecord.h"，磁盘上根本没有这个文件（fatal error C1083）。
//         真正的文件叫 BorrowRecord.h，与 Reader.h 里 #include 的名字保持一致。
#include"BorrowRecord.h"
using namespace std;

//构造函数
Reader::Reader() :
	m_id(""),
	m_name("未知"),
	m_department(""),
	m_maxBorrow(5),
	m_borrowedCount(0),
	m_recordCount(0)
{
}
Reader::Reader(const string& id,
	const string& name,
	const string& department,
	int maxBorrow) :
	m_id(id),
	m_name(name),
	m_department(department),
	m_maxBorrow(maxBorrow > 0 ? maxBorrow : 5),//上限至少为1，默认为5
	m_borrowedCount(0),
	m_recordCount(0)
{
}

//修改操作，带校验
bool Reader::setName(const string& name) {
	if (name.empty())return false;
	// ★修正2：原来写的是 m_department = name;，把"改姓名"错写成了"改院系"。
	//         结果是 setName() 永远改不动姓名，反而把院系冲掉，而 setDepartment()
	//         又去改院系，两个 setter 职责串了位。改为赋值给 m_name。
	m_name = name;
	return true;
}
bool Reader::setDepartment(const string& department) {
	if (department.empty())return false;
	m_department = department;
	return true;
}
//改借阅上限，边界;不能把上限改的比当前已借还少
bool Reader::setMaxBorrow(int maxBorrow) {
	if (maxBorrow < m_borrowedCount)return false;
	m_maxBorrow = maxBorrow;
	return true;
}

//查询辅助
bool Reader::hasBorrowed(const string& isbn)const {
	return findActiveRecord(isbn) >= 0;
}
//找到一条生效中的记录，未归还且isbn匹配，内部工具
int Reader::findActiveRecord(const string& isbn)const {
	for (int i = 0;i < m_recordCount;++i) {
		if (!m_records[i].isReturned() && m_records[i].getIsbn() == isbn) {
			return i;
		}
		// ★修正3：原来这里的 return -1; 写在 for 循环体内部，
		//   导致循环只转一圈就退出 —— 只看第 0 条记录，第 1 条之后永远找不到。
		//   更糟的是，若 m_recordCount == 0，循环体一次都不执行，
		//   函数会走完全程却没有 return，返回值是未定义的随机数（UB），
		//   它会随机地让"重复借书检查"和"还书找记录"判断出错。
		//   把 return -1; 挪到循环结束之后，代表"所有记录都比对过了，确实没有"。
	}
	return -1;      // 全部记录都比对完了，没有找到
}

//借书：依赖关系的实现
//双方状态都要判断
//     (1) 图书指针是否有效        —— 防止空指针
//     (2) 这本书是否可借          —— 书的状态
//     (3) 读者额度是否够用        —— 读者的状态
//     (4) 是否重复借同一本书      —— 业务合理性
//     (5) 记录数组是否还有空位    —— 内部容量限制
//   全部通过，才执行"借出"。
bool Reader::borrowBook(Book* book, const Date& today) {
	//检查1：指针有效性：空指针引用会直接崩溃
	if (book == nullptr) {
		cout << "[失败]图书指针为空" << endl;
		// ★修正4：这里原来只打印了一句提示，没有 return false。
		//   函数会带着空指针继续往下走，第 76 行 book->isAvailable() 立刻解引用空指针 → 程序崩溃。
		//   守卫语句必须"打印 + 返回"，否则等于没守卫。
		return false;
	}
	//检查2：图书是否可借（被依赖方Book的状态）
	if (!book->isAvailable()) {
		std::cout << "  [失败] 《" << book->getTitle()
			<< "》当前无可借复本（馆藏 " << book->getTotalCount()
			<< " 本，可借 " << book->getAvailableCount() << " 本）。\n";
		return false;
	}
	//检查3：读者额度（依赖方Reader自己的状态）
	if (!canBorrowMore()) {
		std::cout << "  [失败] " << m_name << " 已达借阅上限（"
			<< m_borrowedCount << "/" << m_maxBorrow << "）。"<<endl;
		return false;
	}
	//检查4：不允许重复借同一本书
	if (hasBorrowed(book->getIsbn())) {
		cout<< "  [失败] " << m_name << " 已借了《"
			<< book->getTitle() << "》且尚未归还。"<<endl;
		return false;
	}
	//检查5：本读者记录数组还有空位
	if (m_recordCount >= 10) {
		cout << "[失败]借阅记录已满（上限十条）" << endl;
		return false;
	}
    //全部检查通过，执行借出，双方状态同时改变
	//（1）改图书状态：可借数-1（通过指针改的是真书不是副本，传地址）
	book->borrowOne();
	//（2）改读者状态：已借数+1
	++m_borrowedCount;
	//（3）添一条借阅记录，组合关系内嵌对象的实际使用
	m_records[m_recordCount] = BorrowRecord(book->getIsbn(), today, 30);
	++m_recordCount;
	cout << "  [成功] " << m_name << " 借出《" << book->getTitle()
              << "》，借出日期 " << today.toString()
              << "，应还日期 " << m_records[m_recordCount - 1].getDueDate().toString()
              << "。\n";
    cout << "         当前已借 " << m_borrowedCount << "/" << m_maxBorrow
              << " 本；该书可借余量 " << book->getAvailableCount() << " 本。\n";
    return true;
}

//还书
//找记录，校验，改双方状态，标记记录已归还，算逾期罚款
bool Reader::returnBook(Book* book, const Date& today) {
	if (book == nullptr) {
		cout << "[失败]图书指针为空" << endl;
		return false;
	}
	// 找到还没还的那条记录
		int idx = findActiveRecord(book->getIsbn());
	if (idx < 0) {
		std::cout << "  [失败] " << m_name << " 没有借《"
			<< book->getTitle() << "》的记录，无法归还。\n";
		return false;
	}
	//归还前先算逾期情况，在标记已归还前算
	bool   overdue = m_records[idx].isOverdue(today);
	int    days = m_records[idx].getOverdueDays(today);
	double fine = m_records[idx].getFine(today);
	//改双方状态
	book->returnOne();      // 图书可借数 +1
	--m_borrowedCount;      // 读者已借数 -1
	m_records[idx].markReturned();
	cout << "  [成功] " << m_name << " 归还《" << book->getTitle()
		<< "》，归还日期 " << today.toString() << "。\n";
	if (overdue) {
		// ★修正14：字符串里的 ⚠（U+26A0）在代码页 936 里没有编码，
		//   MSVC 报 warning C4566，运行时输出会变成 '?'，
		//   屏幕上显示成"         ? 逾期 20 天"，反而像出了故障。
		//   换成纯中文的【逾期】超期，任何中文环境下都能正常显示。
		std::cout << "         【逾期】超期 " << days << " 天，应缴罚款 ￥"
			<< std::fixed << std::setprecision(2) << fine << "。\n";
	}
	else {
		std::cout << "         按期归还，无罚款。\n";
	}
	cout << "         当前已借 " << m_borrowedCount << "/" << m_maxBorrow
		<< " 本；该书可借余量 " << book->getAvailableCount() << " 本。\n";
	return true;
}

//输出
void Reader::print() const {
	std::cout << "----------------------------------------\n"
		<< "学号/工号：" << m_id << '\n'
		<< "姓名      ：" << m_name << '\n'
		<< "院系      ：" << m_department << '\n'
		<< "借阅上限  ：" << m_maxBorrow << " 本\n"
		<< "当前已借  ：" << m_borrowedCount << " 本"
		<< "（还可借 " << getRemainingQuota() << " 本）\n"
		<< "----------------------------------------\n";
}
//打印全部借阅记录
//组合关系的传递性，Reader 包含 BorrowRecord，BorrowRecord 又包含 Date，
void Reader::printRecords(const Date& today) const {
	if (m_recordCount == 0) {
		std::cout << "  （暂无借阅记录）\n";
		return;
	}
	for (int i = 0; i < m_recordCount; ++i) {
		std::cout << "  记录 " << (i + 1) << "：\n";
		m_records[i].print(today);
	}
	// ★修正5：文件到这里就断了，printRecords 函数体的右花括号 } 漏掉了。
	//   上一个函数 print() 之所以能正常编译，是因为编译器会把后面遇到的
	//   第一个 }（也就是 for 循环的那个）当作它的结束符；
	//   但轮到 printRecords 时就已经"借不到"花括号了，
	//   于是整个文件在末尾少一个 }，报错形如
	//   "error C1075: 文件结尾处的 }; 与前面的 { 不匹配"。
	//   补上这个 }，函数体才完整闭合。
}