#ifndef JULIANDATE_HPP_
#define JULIANDATE_HPP_
#include <Time/Time.hpp>

namespace Datetime
{
	class Year
	{
	private:
		int year;

	public:
		Year(int year_ = 0) throw() : year(year_) {}
		~Year() throw() {}

		void setYear(int newYear) throw() { year = newYear; };
		int getYear() const throw() { return year; };
	};

	class Month
	{
	private:
		int month;
		
	public:
		Month(int month_ = 0) throw() : month(month_) {}
		~Month() throw() {}

		void setMonth(int newMonth) throw() { month = newMonth; };
		int getMonth() const throw() { return month; };
	};

	class Day
	{
	private:
		int day;
		
	public:
		Day(int day_ = 0) throw() : day(day_) {}
		~Day() throw() {}

		void setDay(int newDay) throw() { day = newDay; };
		int getDay() const throw() { return day; };
	};

}

class JulianDate
{
private:
	/**
	 * (紀元1年1月1日を通日の1日とした)ユウリス通日
	 */
	unsigned int julianDay;

	/**
	 * 世界協定時日付からユリウス日(紀元1年1月1日を通日の1日として返す)
	 * を取得
	 * @param year 年、紀元前は説明を参照
	 * @param month 月、0オフセットで始まる
	 * @param day 日
	 * @note 紀元前は1年を0、2年を-1として以下、3年=-2...とする
	 */
	static unsigned int toJulian(int year,
								 int month,
								 int day) throw()
	{
		assert(month >= 0 && month < 12);
		assert(day >= 1 && day <= 31);

		return static_cast<int>(365.25 * year) +
			static_cast<int>(year / 400) -
			static_cast<int>(year / 100) +
			static_cast<int>(30.59 * (month - 1)) + 
			day - 335;
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

	bool operator==(const JulianDate& rhs) const throw()
	{
		return this->julianDay == rhs.julianDay;
	}

	bool operator!=(const JulianDate& rhs) const throw()
	{
		return !this->operator==(rhs);
	}

	bool operator<(const JulianDate& rhs) const throw()
	{
		return this->julianDay < rhs.julianDay;
	}

	bool operator<=(const JulianDate& rhs) const throw()
	{
		return this->julianDay <= rhs.julianDay;
	}

	bool operator>(const JulianDate& rhs) const throw()
	{
		return !this->operator<=(rhs);
	}

	bool operator>=(const JulianDate& rhs) const throw()
	{
		return !this->operator<(rhs);
	}

	JulianDate& operator=(const JulianDate& rhs) throw()
	{
		if (this != &rhs)
			this->julianDay = rhs.julianDay;

		return *this;
	}

	JulianDate& operator+=(const JulianDate& rhs) throw()
	{
		this->julianDay += rhs.julianDay;
		return *this;
	}

	JulianDate operator+(const JulianDate& rhs) const throw()
	{
		return JulianDate(*this) += rhs;
	}

	JulianDate& operator-=(const JulianDate& rhs) throw()
	{
		this->julianDay -= rhs.julianDay;
		return *this;
	}

	JulianDate operator-(const JulianDate& rhs) const throw()
	{
		return JulianDate(*this) -= rhs;
	}

	JulianDate& operator+=(const Datetime::Year& year) throw()
	{
		this->julianDay += year.getYear();
		return *this;
	}

	JulianDate operator+(const Datetime::Year& year) const throw()
	{
		return JulianDate(*this) += year;
	}

	JulianDate& operator+=(const Datetime::Month& month) throw()
	{
		this->julianDay += month.getMonth();
		return *this;
	}

	JulianDate operator+(const Datetime::Month& month) const throw()
	{
		return JulianDate(*this) += month;
	}

	JulianDate& operator+=(const Datetime::Day& day) throw()
	{
		this->julianDay += day.getDay();
		return *this;
	}

	JulianDate operator+(const Datetime::Day& day) const throw()
	{
		return JulianDate(*this) += day;
	}

};

#endif /* JULIANDATE_HPP_ */
