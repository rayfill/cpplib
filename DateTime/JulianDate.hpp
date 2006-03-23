#ifndef JULIANDATE_HPP_
#define JULIANDATE_HPP_
#include <Time/Time.hpp>

/**
 * �C�������E�X�ʓ��N���X
 * @see http://ja.wikipedia.org/wiki/%E3%83%A6%E3%83%AA%E3%82%A6%E3%82%B9%E9%80%9A%E6%97%A5
 * @see http://64.233.179.104/search?q=cache:pkzGhrOFHTIJ:kwi.cocolog-nifty.com/blog/delphi/index.html+%E3%83%95%E3%83%AA%E3%83%BC%E3%82%B2%E3%83%AB%E3%81%AE%E5%85%AC%E5%BC%8F&hl=ja&gl=jp&ct=clnk&cd=7
 * @todo �����E�X��Ɗ��Ⴂ���ꂻ���ȃN���X�����ȁE�E�E���������ق���������
 */
class JulianDate
{
	friend class JulianDateTest;

private:
	/**
	 * �C�����E���X�ʓ�
	 */
	int julianDay;

	/**
	 * ���t����C�������E�X�����擾
	 * @param year �N�A�I���O�͐������Q��
	 * @param month ���A1�I�t�Z�b�g�Ŏn�܂�
	 * @param day ��
	 * @note �I���O��1�N��0�A2�N��-1�Ƃ��Ĉȉ��A3�N=-2...�Ƃ���B
	 * epoch��1878/11/17�ɂȂ�
	 * @see �t���[�Q���̌���
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
	 * �K�E�X�̏��֐�
	 * ���̐��̏ꍇ�A�����̐؎̂āB���̐��̏ꍇ�}�C�i�X����������ɐ؂�グ�B
	 * @param value �ΏۂƂȂ鐔
	 * @return ��������
	 */
	static int floor(const double value)
	{
		if (value >= 0)
			return static_cast<int>(value);

		return static_cast<int>(value * -1) * -1;
	}

	/**
	 * �C�������E�X������O���S���I���t�ւ̕ϊ�
	 * @param julianDay �C�������E�X��
	 * @param year �o�͂����N
	 * @param month �o�͂���錎
	 * @param day �o�͂�����
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
	 * �����f�[�^�̔N��Ԃ�
	 * @return �N
	 */
	int getYear() const
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		return y;
	}

	/**
	 * �N�̐ݒ�
	 * @param newYear �V���ɐݒ肷��N
	 */
	void setYear(const int newYear)
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		julianDay = toJulian(newYear, m, d);
	}

	/**
	 * �N�̉��Z
	 * @param yearDiff ���Z����N��
	 */
	void addYear(const int yearDiff)
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		julianDay = toJulian(y+yearDiff, m, d);
	}

	/**
	 * �����f�[�^�̌���Ԃ�
	 * @return ��
	 */
	int getMonth() const
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		return m;
	}

	/**
	 * ���̐ݒ�
	 * @param newMonth �V���ɐݒ肷�錎
	 */
	void setMonth(const int newMonth)
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		julianDay = toJulian(y, newMonth, d);
	}

	/**
	 * ���̉��Z
	 * @param monthDiff ���Z���錎��
	 */
	void addMonth(const int monthDiff)
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		julianDay = toJulian(y, monthDiff, d);
	}

	/**
	 * �����f�[�^�̓���Ԃ�
	 * @return ��
	 */
	int getDay() const
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		return d;
	}

	/**
	 * ���̐ݒ�
	 * @param newDay �V���ɐݒ肷���
	 */
	void setDay(const int newDay)
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		julianDay = toJulian(y, m, newDay);
	}

	/**
	 * ���̉��Z
	 * @param monthDiff ���Z�������
	 */
	void addDay(const int dayDiff)
	{
		int y, m, d;
		toGregorian(julianDay, y, m, d);
		julianDay = toJulian(y, m, d+dayDiff);
	}

	/**
	 * ���t�̐������m�F
	 * @return ���݂�����t�ł����true
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
	 * �j���̎擾
	 * @return ���j��0�I�t�Z�b�g�Ƃ����j���\��
	 */
	int getWeekOfDay() const
	{
		return (julianDay - 4)% 7;
	}

	/**
	 * ��������
	 * @param rhs ��rJulianDate�I�u�W�F�N�g
	 * @return �������ꍇ
	 */
	bool operator==(const JulianDate& rhs) const throw()
	{
		return this->julianDay == rhs.julianDay;
	}

	/**
	 * �񓙉�����
	 * @param rhs ��rJulianDate�I�u�W�F�N�g
	 * @return �������Ȃ��ꍇ
	 */
	bool operator!=(const JulianDate& rhs) const throw()
	{
		return !this->operator==(rhs);
	}

	/**
	 * �召����
	 * @param rhs ��rJulianDate�I�u�W�F�N�g
	 * @return �������ꍇ
	 */
	bool operator<(const JulianDate& rhs) const throw()
	{
		return this->julianDay < rhs.julianDay;
	}

	/**
	 * �召����
	 * @param rhs ��rJulianDate�I�u�W�F�N�g
	 * @return ���������������ꍇ
	 */
	bool operator<=(const JulianDate& rhs) const throw()
	{
		return this->julianDay <= rhs.julianDay;
	}

	/**
	 * �召����
	 * @param rhs ��rJulianDate�I�u�W�F�N�g
	 * @return �傫���ꍇ
	 */
	bool operator>(const JulianDate& rhs) const throw()
	{
		return !this->operator<=(rhs);
	}

	/**
	 * �召����
	 * @param rhs ��rJulianDate�I�u�W�F�N�g
	 * @return ���������傫���ꍇ
	 */
	bool operator>=(const JulianDate& rhs) const throw()
	{
		return !this->operator<(rhs);
	}

	/**
	 * ���
	 * @param rhs �R�s�[��JulianDate�I�u�W�F�N�g
	 * @return �R�s�[��̎��g�ւ̎Q��
	 */
	JulianDate& operator=(const JulianDate& rhs) throw()
	{
		if (this != &rhs)
			this->julianDay = rhs.julianDay;

		return *this;
	}

	/**
	 * ���Z
	 * @param rhs ���Z�Ώ�JulianDate�I�u�W�F�N�g
	 * @return ���Z��̎��g�ւ̎Q��
	 */
	JulianDate& operator+=(const JulianDate& rhs) throw()
	{
		this->julianDay += rhs.julianDay;
		return *this;
	}

	/**
	 * ���Z
	 * @param rhs ���Z�Ώ�JulianDate�I�u�W�F�N�g
	 * @return ���Z��̃I�u�W�F�N�g
	 */
	JulianDate operator+(const JulianDate& rhs) const throw()
	{
		return JulianDate(*this) += rhs;
	}

	/**
	 * ���Z
	 * @param rhs ���Z�Ώ�JulianDate�I�u�W�F�N�g
	 * @return ���Z��̎��g�ւ̎Q��
	 */
	JulianDate& operator-=(const JulianDate& rhs) throw()
	{
		this->julianDay -= rhs.julianDay;
		return *this;
	}

	/**
	 * ���Z
	 * @param rhs ���Z�Ώ�JulianDate�I�u�W�F�N�g
	 * @return ���Z��̃I�u�W�F�N�g
	 */
	JulianDate operator-(const JulianDate& rhs) const throw()
	{
		return JulianDate(*this) -= rhs;
	}

};

#endif /* JULIANDATE_HPP_ */
