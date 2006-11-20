#ifndef JULIANDATE_HPP_
#define JULIANDATE_HPP_
//#include <Time/Time.hpp>
#include <cassert>

/**
 * 修正ユリウス通日クラス
 * @see http://ja.wikipedia.org/wiki/%E3%83%A6%E3%83%AA%E3%82%A6%E3%82%B9%E9%80%9A%E6%97%A5
 * @see http://64.233.179.104/search?q=cache:pkzGhrOFHTIJ:kwi.cocolog-nifty.com/blog/delphi/index.html+%E3%83%95%E3%83%AA%E3%83%BC%E3%82%B2%E3%83%AB%E3%81%AE%E5%85%AC%E5%BC%8F&hl=ja&gl=jp&ct=clnk&cd=7
 * @todo ユリウス暦と勘違いされそうなクラス名だな・・・改名したほうがいいな
 */
class JulianDate
{
	friend class JulianDateTest;

private:
	/**
	 * 修正ユウリス通日
	 */
	int julianDay;

	/**
	 * 日付から修正ユリウス日を取得
	 * @param year 年、紀元前は説明を参照
	 * @param month 月、1オフセットで始まる
	 * @param day 日
	 * @note 紀元前は1年を0、2年を-1として以下、3年=-2...とする。
	 * epochは1878/11/17になる
	 * @see フリーゲルの公式
	 */
	static int toJulian(int year,
						int month,
						int day) throw()
	{
		assert(month >= 1 && month <= 12);
		assert(day >= 1 && day <= 31);

		if (month == 1 || month == 2)
		{
			--year;
			month += 12;
		}

		return floor(365.25 * year) +
			floor(year / 400) -
			floor(year / 100) +
			floor(30.59 * (month - 2)) + 
			day - 678912;
	}

	/**
	 * ガウスの床関数
	 * 正の数の場合、少数の切捨て。負の数の場合マイナス無限大方向に切り上げ。
	 * @param value 対象となる数
	 * @return 処理結果
	 */
	static int floor(const double value)
	{
		if (value >= 0)
			return static_cast<int>(value);

		return static_cast<int>(value * -1) * -1;
	}

	/**
	 * 修正ユリウス日からグレゴリオ日付への変換
	 * @param julianDay 修正ユリウス日
	 * @param year 出力される年
	 * @param month 出力される月
	 * @param day 出力される日
	 */
	static void toGregorian(const int julianDay,
							int& year,
							int& month,
							int& day)
	{
		year = floor((julianDay - 15078.2) / 365.25);

		month = floor((julianDay - 14956.1
								  - floor(year * 365.25))
								 / 30.6001);

		if (month <= 1)
		{
			month += 12;
			--year;
		}

		day = julianDay
			- 14956 - floor(year * 365.25)
			   - floor(month * 30.6001);

		int carry = 0;
		if (month == 14 || month == 15)
			carry = 1;
		else
			carry = 0;
			
		year += (carry + 1900);
		month -= (1 + (carry ? 12 : 0));
	}

public:
	JulianDate() throw()
		: julianDay(0)
	{
	}

	JulianDate(int year,
			   unsigned char month,
			   unsigned char day) throw():
		julianDay(toJulian(year, month, day))
	{
	}

	JulianDate(const JulianDate& julianDate) throw():
		julianDay(julianDate.julianDay)
	{
	}
	
	JulianDate(unsigned int julianDay_) throw():
		julianDay(julianDay_)
	{
	}
	
	virtual ~JulianDate() throw()
	{
	}

	/**
	 * 内部データの年を返す
	 * @return 年
	 */
	int getYear() const
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		return y;
	}

	/**
	 * 年の設定
	 * @param newYear 新たに設定する年
	 */
	void setYear(const int newYear)
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		julianDay = toJulian(newYear, m, d);
	}

	/**
	 * 年の加算
	 * @param yearDiff 加算する年数
	 */
	void addYear(const int yearDiff)
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		julianDay = toJulian(y+yearDiff, m, d);
	}

	/**
	 * 内部データの月を返す
	 * @return 月
	 */
	int getMonth() const
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		return m;
	}

	/**
	 * 月の設定
	 * @param newMonth 新たに設定する月
	 */
	void setMonth(const int newMonth)
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		julianDay = toJulian(y, newMonth, d);
	}

	/**
	 * 月の加算
	 * @param monthDiff 加算する月数
	 */
	void addMonth(const int monthDiff)
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		julianDay = toJulian(y, monthDiff, d);
	}

	/**
	 * 内部データの日を返す
	 * @return 日
	 */
	int getDay() const
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		return d;
	}

	/**
	 * 日の設定
	 * @param newDay 新たに設定する日
	 */
	void setDay(const int newDay)
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		julianDay = toJulian(y, m, newDay);
	}

	/**
	 * 日の加算
	 * @param monthDiff 加算する日数
	 */
	void addDay(const int dayDiff)
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		julianDay = toJulian(y, m, d+dayDiff);
	}

	/**
	 * 日付の正当性確認
	 * @return 存在する日付であればtrue
	 */
	static bool isValidate(const int year, const int month, const int day)
	{
		int y, m, d;
		toGregorian(toJulian(year, month, day), y, m, d);
		
		return d == day &&
			m == month &&
			y == year;
	}

	/**
	 * 曜日の取得
	 * @return 日曜を0オフセットとした曜日表現
	 */
	int getWeekOfDay() const
	{
		return (julianDay - 4)% 7;
	}

	/**
	 * 等価検査
	 * @param rhs 比較JulianDateオブジェクト
	 * @return 等しい場合
	 */
	bool operator==(const JulianDate& rhs) const throw()
	{
		return this->julianDay == rhs.julianDay;
	}

	/**
	 * 非等価検査
	 * @param rhs 比較JulianDateオブジェクト
	 * @return 等しくない場合
	 */
	bool operator!=(const JulianDate& rhs) const throw()
	{
		return !this->operator==(rhs);
	}

	/**
	 * 大小検査
	 * @param rhs 比較JulianDateオブジェクト
	 * @return 小さい場合
	 */
	bool operator<(const JulianDate& rhs) const throw()
	{
		return this->julianDay < rhs.julianDay;
	}

	/**
	 * 大小検査
	 * @param rhs 比較JulianDateオブジェクト
	 * @return 等しいか小さい場合
	 */
	bool operator<=(const JulianDate& rhs) const throw()
	{
		return this->julianDay <= rhs.julianDay;
	}

	/**
	 * 大小検査
	 * @param rhs 比較JulianDateオブジェクト
	 * @return 大きい場合
	 */
	bool operator>(const JulianDate& rhs) const throw()
	{
		return !this->operator<=(rhs);
	}

	/**
	 * 大小検査
	 * @param rhs 比較JulianDateオブジェクト
	 * @return 等しいか大きい場合
	 */
	bool operator>=(const JulianDate& rhs) const throw()
	{
		return !this->operator<(rhs);
	}

	/**
	 * 代入
	 * @param rhs コピー元JulianDateオブジェクト
	 * @return コピー後の自身への参照
	 */
	JulianDate& operator=(const JulianDate& rhs) throw()
	{
		if (this != &rhs)
			this->julianDay = rhs.julianDay;

		return *this;
	}

	/**
	 * 加算
	 * @param rhs 加算対象JulianDateオブジェクト
	 * @return 加算後の自身への参照
	 */
	JulianDate& operator+=(const JulianDate& rhs) throw()
	{
		this->julianDay += rhs.julianDay;
		return *this;
	}

	/**
	 * 加算
	 * @param rhs 加算対象JulianDateオブジェクト
	 * @return 加算後のオブジェクト
	 */
	JulianDate operator+(const JulianDate& rhs) const throw()
	{
		return JulianDate(*this) += rhs;
	}

	/**
	 * 減算
	 * @param rhs 減算対象JulianDateオブジェクト
	 * @return 減算後の自身への参照
	 */
	JulianDate& operator-=(const JulianDate& rhs) throw()
	{
		this->julianDay -= rhs.julianDay;
		return *this;
	}

	/**
	 * 減算
	 * @param rhs 減算対象JulianDateオブジェクト
	 * @return 減算後のオブジェクト
	 */
	JulianDate operator-(const JulianDate& rhs) const throw()
	{
		return JulianDate(*this) -= rhs;
	}

};

#endif /* JULIANDATE_HPP_ */
