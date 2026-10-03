#pragma once
//目的：反映借出日期/应还日期，对于之后的日期合法性校验和逾期天数/罚款也方便，
//组合关系：借阅记录包含一个Date对象   ← ★同修正8：原注释写成"Data对象"
//设计：内部只存年/月/日，需要比较时用toDays()把日期换成天数序号来计算
#include<string>
using namespace std;

class Date
{
public:
	Date();
	Date(int year, int month, int day);

	//获取
	int getYear() const { return m_year; }
	int getMonth()const { return m_month; }
	int getDay()const { return m_day; }

	//修改，带校验
	bool setDate(int year, int month, int day);
	//规则至于年月日三个数字有关，不依赖与某个具体日期对象，因此用静态工具函数
	static bool isLeapYear(int year);//闰年判断
	static int daysInMonth(int year, int month);//某年某月有几天
	static bool isValidDate(int year, int month, int day);

	/*日期运算：toDays():将日期换算成自某天起过了多少天的整数，
	换算之后比较两日期的先后算,两日期相差多少天*/
	// ★同修正8：原注释写成"taDays()"，函数名拼错了，照注释写会编译不过。
	long long toDays()const;

	// 从本日期到 other 相差多少天（结果可正可负）
   // 【约定】a.daysBetween(b) = b - a，即"从 a 走到 b 需要多少天"。
   //   例：9月19日.daysBetween(10月19日) = 30  →  30 天后到期
   //       应还日.daysBetween(今天) > 0       →  今天已超期
	long long daysBetween(const Date& other) const;

	// 在现有日期上加 n 天，返回新日期（用于"借出 30 天后到期"）
	Date addDays(int n) const;

	// 判断是不是一个已正确初始化的合法日期
	bool isValid() const;

	// 输出
	string toString() const;     // 返回 "2026-09-19" 格式的字符串
	void print() const;

private:
	int m_year;
	int m_month;
	int m_day;
};

