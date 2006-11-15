#ifndef MPI_HPP_
#define MPI_HPP_


#include <math/ArithmeticException.hpp>
#include <math/theory/AlgebraTheory.hpp>
#include <math/Sieve.hpp>
#include <Cryptography/Random.hpp>
#include <IO/Endian.hpp>
#include <support/HeapChunkAllocator.hpp>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <limits>

/**
 * ���{�������N���X
 * @param BaseUnit_ ��ƂȂ�^
 * @param CalcBase_ �v�Z��ƂȂ�^�B
 * sizeof(BaseUnit_) < sizeof(CalcUnit_)���K�{����
 * @param Allocator �J�X�^���������Ǘ��N���X�BSTL�̃A���P�[�^�݊����K�{����
 */
template <
	typename BaseUnit_ = unsigned int,
	typename CalcBase_ = unsigned long long,
//	typename Allocator = std::allocator<BaseUnit_> >
	typename Allocator = HeapChunkAllocator<BaseUnit_, 8192*16, 64> >
class MultiPrecisionInteger
{
public:
	typedef BaseUnit_ BaseUnit;
	typedef CalcBase_ CalcBase;
	typedef std::vector<BaseUnit, Allocator> MPVector;

private:
	friend class MPITest;

	/// �f�[�^�̓����\��
	MPVector value;

	/// �����Bture�Ń}�C�i�X
	bool isMinusSign;

	/**
	 * value�̐��K���p�w���p�t�@���N�^
	 */
	struct findFunc
	{
	private:
		BaseUnit val;
	public:
		findFunc(const BaseUnit val_)
			: val(val_)
		{
		}

		bool operator()(BaseUnit rhs)
		{
			return val != rhs;
		}
	};

// 	static const std::vector<BaseUnit> getPrimes()
// 	{
// 		return std::vector<BaseUnit>(
// 			&primes[0],
// 			&primes[sizeof(primes) / sizeof(unsigned int)]);
// 	}

	/**
	 * ���オ��̃}�X�N�l�̎擾
	 * @todo �e���v���[�g��enum�g���ăR���p�C�����v�Z�ɂł��邩�H
	 */
	const static CalcBase getCarryValue()
	{
		return std::numeric_limits<BaseUnit>::max() + 1;
	}

	/**
	 * ��̍ő�l�̎擾
	 */
	const static CalcBase getMaxBaseUnit()
	{
		return std::numeric_limits<BaseUnit>::max();
	}

	/**
	 * �\�Z�i�\�������񂩂�̕ϊ�
	 * @param str �ϊ���������
	 */
	void parseString(std::string str)
	{
		const unsigned int tokenLength = sizeof(BaseUnit) * 2;

		if (str.length() == 0)
			throw TokenParseException();

		if (str[0] == '+' ||
			str[0] == '-')
		{
			this->isMinusSign = str[0] == '-' ? true : false;
			str = std::string(str, 1);
		}

		BaseUnit val = 0;
		unsigned int count = 0;
		for (std::string::reverse_iterator current = str.rbegin();
			 current != str.rend();
			 ++current)
		{
			if (*current >= '0' && *current <= '9')
				val = val +
					static_cast<BaseUnit>
					((*current - '0') << (4 * count));

			else if (*current >= 'A' && *current <= 'F')
				val = val +
					static_cast<BaseUnit>
					((*current - 'A' + 10) << (4 * count));

			else if (*current >= 'a' && *current <= 'f')
				val = val +
					static_cast<BaseUnit>
					((*current - 'a' + 10) << (4 * count));

			else
				throw TokenParseException(&*current);

			if (count == tokenLength - 1)
			{
				value.push_back(val);
				val = 0;
			}

			count = (count + 1) % tokenLength;
		}

		if (val != 0)
			value.push_back(val);
	}

	/**
	 * �l�̌���
	 * @return ������̒l
	 * @param �����Ώۂ̃I�u�W�F�N�g
	 */
	MultiPrecisionInteger& swap(MultiPrecisionInteger target) throw()
	{
		this->value.swap(target.value);
		if (this->isMinusSign != target.isMinusSign)
		{
			this->isMinusSign = !this->isMinusSign;
			target.isMinusSign = !target.isMinusSign;
		}
		
		return *this;
	}

	/**
	 * �r�b�g�l�̎擾
	 * @return �r�b�g�l
	 * @param offset LSB����̋���(MAX��MSB-LSB�AMIN��LSB)
	 */
	bool getBit(size_t offset) const
	{
		const size_t blockOffset = offset / (sizeof(BaseUnit) * 8);
		const size_t bitOffset = offset % (sizeof(BaseUnit) * 8);
		
		return ((value[blockOffset] >> bitOffset) & 0x01) == 1;
	}

	/**
	 * �����S������Z
	 * @return ��Z��̒l
	 * @param a �|���鐔
	 * @param b �|�����鐔
	 * @param r �ׂ���
	 * @param n ����Q�̏�]��
	 * @param n_ n�̋t��
	 */
	MultiPrecisionInteger
	montgomeryProduct(const MultiPrecisionInteger& a,
					  const MultiPrecisionInteger& b,
					  const MultiPrecisionInteger& r,
					  const MultiPrecisionInteger& n,
					  const MultiPrecisionInteger& n_) const
	{
		MultiPrecisionInteger t = a * b;

		// m = t * n_ % r;
 		MultiPrecisionInteger m = t * n_;
 		MultiPrecisionInteger m_dash =
 			(m >> static_cast<int>(r.getBitLength() - 1)) <<
			static_cast<int>(r.getBitLength() - 1);
 		m -= m_dash;

		// u = (t + m * n) / r;
		MultiPrecisionInteger u = (t + m * n) >>
			static_cast<int>(r.getBitLength() - 1);
		if (u > n)
			return u - n;
		return u;
	}

	/**
	 * ��]�v�Z
	 * @return BaseUnti �]��
	 * @param divisor ���鐔
	 */
	BaseUnit modulus(const BaseUnit divisor) const
	{
		CalcBase carry = 0;
		for (size_t offset = this->getMaxColumn(); offset > 0; --offset)
		{
			carry = ((carry << (sizeof(BaseUnit) * 8)) +
					 this->value[offset-1]) % divisor;
		}

		return static_cast<BaseUnit>(carry);
	}

	/**
	 * ���Z
	 * @return ��
	 * @param src ���鐔
	 * @param modulo �]��(�o�͒l)
	 */
	MultiPrecisionInteger&
	divide(const MultiPrecisionInteger& src,
		   MultiPrecisionInteger& modulo)
	{
		if (this->isMinusSign != false ||
			src.isMinusSign != false)
			throw OperationSourceException();
		
		if (src == MultiPrecisionInteger(0U))
			throw ZeroDivideException();

		MultiPrecisionInteger result(0U, this->value.size());
		result.isMinusSign = this->isMinusSign ^ src.isMinusSign;

		modulo = 0U;
		
		MultiPrecisionInteger lhs(*this), rhs(src);

		lhs.adjust();
		rhs.adjust();

		int currentDividePosition = // base of zero.
			static_cast<int>(lhs.value.size() - rhs.value.size());

		MultiPrecisionInteger temp;
		while (currentDividePosition >= 0)
		{
			// challenge dividable.
			temp.value.erase(temp.value.begin(), temp.value.end());

			lhs.adjust();
			temp = MultiPrecisionInteger(
				&lhs.value[currentDividePosition],
				&lhs.value[lhs.value.size()]);

			if (temp >= rhs)
			{
				CalcBase count = CalcBase();

				if (temp.value.size() - rhs.value.size() > 0)
				{
					// difference column length.
					assert(temp.value.size() - rhs.value.size() < 2);
					
					CalcBase val =
						(temp.value[temp.value.size()-1] * 
						 ((CalcBase)(std::numeric_limits<BaseUnit>::max()) + 1)) +
						temp.value[temp.value.size()-2];

					count =
						val / rhs.value.back();
				}
				else
				{
					// non difference column length.
					count = 
						temp.value[temp.value.size()-1] /
						rhs.value[rhs.value.size()-1];
				}

				if (count == 0)
					continue;

				MultiPrecisionInteger subTemp(rhs);
				subTemp *= count;
				while (temp < subTemp)
				{
					--count;
					subTemp -= rhs;
				}

				if (count == 0)
					continue;

				result.value[currentDividePosition] =
					static_cast<BaseUnit>(count);

				temp =
					subTemp << 
					((sizeof(BaseUnit) * 8) * currentDividePosition);

				lhs -= temp;
			}
			--currentDividePosition;
		}

		modulo.swap(lhs).adjust();

		this->swap(result);
		return this->adjust();
	}

public:
		
	/**
	 * ���g�̕������]
	 * @return ���g�ւ̎Q��
	 */
	MultiPrecisionInteger& negate()
	{
		isMinusSign = !isMinusSign;
		return *this;
	}

	/**
	 * �������]
	 * @return ���]�����l�����ꎞ�I�u�W�F�N�g
	 */
	MultiPrecisionInteger getNegateValue() const
	{
		return MultiPrecisionInteger(*this).negate();
	}

	/**
	 * �f�t�H���g�R���X�g���N�^
	 */
	MultiPrecisionInteger()
		: value(), isMinusSign(false)
	{}

	/**
	 * �R���X�g���N�^
	 * ��͈͂̒l�ł̏�����
	 * @param value_ ��^�ł̐��l
	 */
	MultiPrecisionInteger(
		const BaseUnit value_)
		: value(), isMinusSign(false)
	{
		value.push_back(value_);
	}

	/**
	 * �R���X�g���N�^
	 * �v�Z���l�͈͂̒l�ł̏�����
	 * @param value_ �v�Z���l�^�ł̐��l
	 */
	MultiPrecisionInteger(
		CalcBase value_)
		: value(), isMinusSign(false)
	{
		const unsigned int ArraySize =
			sizeof(CalcBase) / sizeof(BaseUnit);

		for (unsigned int index = 0;
			 index < ArraySize;
			 ++index)
		{
			value.push_back(
				static_cast<BaseUnit>(value_ &
				std::numeric_limits<BaseUnit>::max()));
			value_ >>= sizeof(BaseUnit) * 8;
		}
	}

	/**
	 * �I�u�W�F�N�g�����^�ł̕\���ւ̕ϊ�
	 * @return �ϊ����ꂽ���l
	 * @exception std::overflow_error �����\���l����^�ɓ���Ȃ������ꍇ
	 */
	BaseUnit toBaseUnit() const
	{
		if (this->getMaxColumn() > sizeof(BaseUnit))
			throw std::overflow_error("can not convert.");

		return this->value.size() ? this->value[0] : 0;
	}

	/**
	 * �R���X�g���N�^
	 * �����l�𓯒l���A���������̂Ŗ��߂ď�����
	 * @param fillValue ���߂�l
	 * @param fillLength ���߂��
	 */
	MultiPrecisionInteger(
		const BaseUnit fillValue,
		const size_t fillLength)
		: value(fillLength), isMinusSign(false)
	{
		std::fill_n(value.begin(), fillLength, fillValue);
	}
	
	/**
	 * �R���X�g���N�^
	 * ��^�z�񂩂�̏�����
	 * @param first �z��̏����|�C���^
	 * @param last �I�[�v�f�̎��̈ʒu��\���|�C���^
	 * @param isMinus �����t���O(true�Ń}�C�i�X)
	 */
	MultiPrecisionInteger(
		const BaseUnit* first,
		const BaseUnit* last,
		bool isMinus = false)
		: value(first, last), isMinusSign(isMinus)
	{
	}

	/**
	 * �R���X�g���N�^
	 * �\�Z�i�\�������񂩂�̏�����
	 * @param cstr �\�Z�i�\���ŏ����ꂽ������
	 */
	MultiPrecisionInteger(
		const char* cstr)
		: value(), isMinusSign(false)
	{
		const std::string str(cstr);
		parseString(cstr);
	}

	/**
	 * �R���X�g���N�^
	 * �\�Z�i�\�������񂩂�̏�����
	 * @param str �\�Z�i�\���ŏ����ꂽ������
	 */
	MultiPrecisionInteger(
		const std::string& str)
		: value(), isMinusSign(false)
	{
		parseString(str);
	}

	/**
	 * �R�s�[�R���X�g���N�^
	 * @param src �R�s�[���̃I�u�W�F�N�g
	 */
	MultiPrecisionInteger(
		const MultiPrecisionInteger& src)
		: value(src.value),
		  isMinusSign(src.isMinusSign)
	{}

	/**
	 * �r�b�g���ۏ؂̑��{�����l�쐬
	 * @param head ���ƂȂ鐔�l�|�C���^�̐擪
	 * @param last ���ƂȂ鐔�l�|�C���^�̍ŏI�v�f�̎�
	 * �����f�[�^�͔j�󂳂��̂Œ���
	 */
	static MultiPrecisionInteger 
	makeNumberOfBitSafe(BaseUnit* head, BaseUnit* last)
	{
		*head |= 0x01; // �f����(2�ȊO��)��f���Ȃ̂�
		// MSB��1�ɂ��邱�ƂŃr�b�g����ۏ�
		*(last-1) |= 
			(((CalcBase)std::numeric_limits<BaseUnit>::max() + 1) >> 1);

		return MultiPrecisionInteger(head, last);
	}
	
	/**
	 * �r�b�g���ۏ؂̑��{�����l�쐬
	 * @param ���ƂȂ鐔�l��std::vector
	 */
	static MultiPrecisionInteger 
	makeNumberOfBitSafe(std::vector<BaseUnit>& src)
	{
		return makeNumberOfBitSafe(&src[0], &src[src.size()]);
	}

	/**
	 * ���{�������̓����\����std::vector<BaseUnit_>�ŕԂ�
	 * @return ���std::vector
	 */
	std::vector<BaseUnit> toVector() const
	{
		return std::vector<BaseUnit>
			(this->value.rbegin(), this->value.rend());
	}

	/**
	 * �r�b�O�G���f�B�A���ł̓����\���̎擾
	 * result[result.size() -1] ����ʌ��Aresult[0]���ŉ��ʌ�
	 * @return �o�C�g�z��Ƃ��Ă̑��{������
	 * @param roundSize �ŏ�ʌ������o�C�g�Ŋۂ߂ď������邩
	 */
	std::vector<unsigned char>
	toBigEndianMemory(const size_t roundSize = 4) const
	{
		EndianConverter converter;
		const size_t valueLength =
			(getMaxColumn() * sizeof(BaseUnit) +
			 ((roundSize -
			   (getMaxColumn() * sizeof(BaseUnit) % roundSize)) %
			  roundSize)) / sizeof(BaseUnit);

		assert(((valueLength * sizeof(BaseUnit)) % roundSize) == 0);

		std::vector<unsigned char> result(valueLength * sizeof(BaseUnit));
		for (size_t offset = getMaxColumn(); 
			 offset > 0;
			 --offset)
		{
			*reinterpret_cast<BaseUnit*>
				(&result[result.size()-offset*sizeof(BaseUnit)]) =
				converter.toBig(this->value[offset-1]);
		}

		return result;
	}

	/**
	 * �r�b�O�G���f�B�A���o�C�g�z�񂩂�̑��{�������\�z
	 * �\�z�ς݃I�u�W�F�N�g�̒l�����������邱�Ƃɂ���Đ������܂�
	 * @return ���g�ւ̎Q��
	 * @param source ���ƂȂ�r�b�O�G���f�B�A���o�C�gstd::vector
	 */
	MultiPrecisionInteger&
	fromBigEndianMemory(std::vector<unsigned char> source)
	{
		if ((source.size() % sizeof(BaseUnit)) != 0)
			throw std::invalid_argument("source length is invalid.");

		EndianConverter converter;
		const size_t valueLength =
			source.size() / sizeof(BaseUnit);
		this->value.clear();
		this->value.resize(valueLength);
		for (size_t offset = valueLength; offset > 0; --offset)
		{
			this->value[offset-1] =
				converter.fromBig(
					*reinterpret_cast<BaseUnit*>(
						&source[source.size()-offset*sizeof(BaseUnit)]));
		}

		this->isMinusSign = false;

		return *this;
	}

	/**
	 * �f�X�g���N�^
	 */
	~MultiPrecisionInteger()
	{}

	/**
	 * �\�Z�i�\��������ւ̕ϊ�
	 * @return �\�Z�i�\���̕�����
	 */
	std::string toString() const
	{
		if (this->isZero())
			return "0";
		
		std::stringstream result;
		if (this->isMinusSign == true)
			result << "-";

		bool isHeadProcessing = true;
		for (typename 
				 MPVector::const_reverse_iterator itor =
				 this->value.rbegin();
			 itor != value.rend();
			 ++itor)
		{
			if (isHeadProcessing == true && *itor == 0)
				continue;

			if (isHeadProcessing == true)
			{
				isHeadProcessing = false;
				result 
					<< std::hex 
					<< *itor;
				result.fill('0');
			}
			else
			{
				result 
					<< std::hex 
					<< std::setw(sizeof(BaseUnit)*2)
					<< *itor;
			}
		}

		return result.str();
	}

	/**
	 * �����\����0���ǂ���
	 * @return 0�Ȃ�true
	 */
	bool isZero() const
	{
		if (this->value.size() == 0)
			return true;

		if ((this->value[0] == 0) &&
			(getMaxColumn() == 0))
			return true;

		return false;
	}

	/**
	 * �����\�����������ǂ���
	 * @return �����Ȃ�true
	 */
	bool isEven() const
	{
		if (isZero() == true)
			return true;

		return !(this->value[0] & 0x01);
	}

	/**
	 * �ׂ���v�Z
	 * @param powerValue_ �ׂ��搔
	 * @return �ׂ��悳�ꂽ��
	 * @todo ������Ȏ����Ȃ̂Ŏ��g���ׂ��悷��悤�ɏ���������
	 * const�ł����悤�ɕύX����ׂ�
	 */
	MultiPrecisionInteger
	power(const MultiPrecisionInteger& powerValue_) const
	{
		if (isZero())
			return MultiPrecisionInteger(0U);
		if (powerValue_.isZero())
			return MultiPrecisionInteger(1U);

		MultiPrecisionInteger targetValue(*this);
		MultiPrecisionInteger powerValue(powerValue_);
		MultiPrecisionInteger result(1U);

		while (!powerValue.isZero())
		{
			if (powerValue.isEven())
			{
				powerValue >>= 1;
				targetValue *= targetValue;
			}
			else
			{
				--powerValue;
				result *= targetValue;
			}
		}

		return result;
	}

	/**
	 * �����S������]���Z�B�ʏ�ׂ̂���]���Z���2�{���x�����ɂȂ�܂�
	 * @param e �ׂ��搔
	 * @param n ��]��
	 * @return ���g�ւ̎Q��
	 */
	MultiPrecisionInteger&
	montgomeryModulusExponential(const MultiPrecisionInteger& e,
								 const MultiPrecisionInteger& n)
	{
		MultiPrecisionInteger r = 1U;

		r <<= static_cast<int>(n.getBitLength());

		MultiPrecisionInteger& a = this->operator*=(r).operator%=(n);
		MultiPrecisionInteger x = r % n;
		MultiPrecisionInteger invertR = modulusInvert(r, n);
		MultiPrecisionInteger n_dash = ((r * invertR) - 1U) / n;

		for (size_t index = e.getBitLength();
			 index > 0;
			 --index)
		{
			x = montgomeryProduct(x, x, r, n, n_dash);
			if (e.getBit(index-1))
				x = montgomeryProduct(a, x, r, n, n_dash);
		}

		*this =
			montgomeryProduct(x, MultiPrecisionInteger(1U), r, n, n_dash);
		return *this;
	}

	/**
	 * ���l��\������̂ɉ��r�b�g���K�v�����擾
	 * @return �\������̂ɕK�v��bit��
	 */
	size_t getBitLength() const
	{
		typename 
			MPVector::difference_type count = 
			std::distance(
				value.begin(),
				std::find_if(
					value.rbegin(),
					value.rend(),
					findFunc(0)).base());

		if (count == 0)
			return 0;

		BaseUnit val = 
			value.at(count-1);

		int offset;
		for (offset = sizeof(BaseUnit) * 8;
			 offset > 0;
			 --offset)
		{
			if (val & (1 << (offset - 1)))
				break;
		}
		
		return static_cast<size_t>
			((count-1) * sizeof(BaseUnit) * 8 + offset);
	}

	/**
	 * ���l��\������̂Ɋ�̔z��ŉ��J�����K�v�����擾
	 * @return �\���ɕK�v�ȃJ������
	 */
	const size_t getMaxColumn() const
	{
		return 
			static_cast<size_t>(
				std::distance(
					value.begin(),
					std::find_if(value.rbegin(),
								 value.rend(),
								 findFunc(0)).base()));
	}

	/**
	 * �����\���̓��ɂ���0�p�f�B���O�̍폜
	 * @return ���g�ւ̎Q��
	 */
	MultiPrecisionInteger& adjust()
	{
		value.erase(
			std::find_if(value.rbegin(),
						 value.rend(),
						 findFunc(0)).base(),
			value.end());

		return *this;
	}

public:

	/**
	 * ������Z�q
	 * @param rhs �����
	 * @return �����̎��g�ւ̎Q��
	 */
	MultiPrecisionInteger&
	operator=(const MultiPrecisionInteger& rhs)
	{
		if (this == &rhs)
			return *this;

		this->value = MPVector(rhs.value);
		this->isMinusSign = rhs.isMinusSign;
		return *this;
	}

	/**
	 * ���Z
	 * @param �E�Ӓl
	 * @return ���Z����
	 */
	MultiPrecisionInteger
	operator+(const MultiPrecisionInteger& rhs) const
	{
		return MultiPrecisionInteger(*this).operator+=(rhs);
	}

	/**
	 * ���Z
	 * @param �E�Ӓl
	 * @return ���Z����
	 */
	MultiPrecisionInteger
	operator-(const MultiPrecisionInteger& rhs) const
	{
		return MultiPrecisionInteger(*this).operator-=(rhs);
	}

	/**
	 * ��Z
	 * @param �搔
	 * @return ��Z����
	 */
	MultiPrecisionInteger
	operator*(const MultiPrecisionInteger& rhs) const
	{
		return MultiPrecisionInteger(*this).operator*=(rhs);
	}

	/**
	 * ���Z
	 * @param ����
	 * @return ���Z����
	 */
	MultiPrecisionInteger
	operator/(const MultiPrecisionInteger& rhs) const
	{
		MultiPrecisionInteger modulo;
		return MultiPrecisionInteger(*this).divide(rhs, modulo);
	}

	/**
	 * ��]�v�Z
	 * @param ����
	 * @return �]��
	 */
	MultiPrecisionInteger
	operator%(const MultiPrecisionInteger& rhs) const
	{
		MultiPrecisionInteger modulo;
		MultiPrecisionInteger(*this).divide(rhs, modulo);
		return modulo;
	}

	/**
	 * ��]�v�Z
	 * ��������Ŏ��܂�ꍇ�͂�������g���Ă��������B
	 * ��ʃo�[�W�����Ɣ�ׂĂ͂邩�ɍ����Ɏ��s�ł��܂��B
	 * @param ����(�������^�͊)
	 * @return �]��
	 */
	BaseUnit operator%(const BaseUnit& rhs) const
	{
		return this->modulus(rhs);
	}

	/**
	 * ���Z���
	 * @param rhs �E�Ӓl
	 * @return ���Z��̎��g�ւ̎Q��
	 */
	MultiPrecisionInteger&
	operator+=(const MultiPrecisionInteger& rhs)
	{
		if (this->isMinusSign != rhs.isMinusSign)
			if (*this > rhs)
				if (this->isMinusSign == true)
					return
						this->negate().operator-=(rhs).negate().adjust();
				else
					return
						this->operator-=(MultiPrecisionInteger(rhs).negate()).adjust();
			else
				if (rhs.isMinusSign == true)
					return
						this->swap(MultiPrecisionInteger(rhs).negate().
						operator-=(*this).negate()).adjust();
				else
					return this->swap(rhs - this->negate()).adjust();

		if (this->value.size() < rhs.value.size())
			this->value.resize(rhs.value.size());

		assert(this->value.size() >= rhs.value.size());

		BaseUnit carry = BaseUnit();
		for (unsigned int offset = 0; offset < this->value.size(); ++offset)
		{
			CalcBase temp;
			temp =
				(CalcBase)this->value[offset] + 
				(offset < rhs.value.size() ? rhs.value[offset] : 0) + carry;

			if (temp > getMaxBaseUnit())
				carry = 1;
			else
				carry = 0;

			this->value[offset] =
				static_cast<BaseUnit>(temp & getMaxBaseUnit());
		}

		if (carry != 0)
			this->value.push_back(carry);

		return this->adjust();
	}

	/**
	 * ���Z���
	 * @param rhs �E�Ӓl
	 * @return ���Z��̎��g�ւ̎Q��
	 */
	MultiPrecisionInteger&
	operator-=(const MultiPrecisionInteger& rhs)
	{
		if (this->isMinusSign == false &&
			rhs.isMinusSign == false)
		{
			// (+a - +b) -> -(-b - +a)
			if (*this < rhs)
			{
				// a < b
				return this->swap(
					MultiPrecisionInteger(rhs).operator-(*this).negate()).adjust();
			}
			// a > b. no action.
		}
		else if (this->isMinusSign == false &&
				 rhs.isMinusSign == true)
		{
			// (+ - -) -> (+ + +)
			return this->operator+=(MultiPrecisionInteger(rhs).negate()).adjust();
		}
		else if (this->isMinusSign == true &&
				 rhs.isMinusSign == false)
		{
			// (-a - +b) -> (-(+a + +b)
			return this->negate().operator+=(rhs).negate().adjust();
		}
		else // (-a - -b) -> -(a - b)
		{
			return this->negate().
				operator-=(MultiPrecisionInteger(rhs).negate()).negate().adjust();
		}

		BaseUnit carry = BaseUnit();
		const size_t rhsMaxColumn = rhs.getMaxColumn();
		for (unsigned int offset = 0;
			 offset < rhsMaxColumn;
			 ++offset)
		{
			if (this->value[offset] >= (rhs.value[offset] + carry))
			{
				this->value[offset] =
					this->value[offset] - rhs.value[offset] - carry;
				carry = 0;
			}
			else
			{
				this->value[offset] =
					static_cast<BaseUnit>(
						getCarryValue() -
						rhs.value[offset] + 
						this->value[offset] - carry);
				carry = 1;
			}
		}

		const size_t lhsMaxColumn = this->getMaxColumn();
		for (unsigned int offset = 
				 static_cast<unsigned int>(rhs.getMaxColumn());
			 offset < lhsMaxColumn;
			 ++offset)
		{
			this->value[offset] =
				this->value[offset] - static_cast<BaseUnit>(carry);
			if (this->value[offset] == getMaxBaseUnit() &&
				carry != 0)
				carry = 1;
			else
			{
				carry = 0;
				break;
			}
		}

		assert(carry == 0);

		return this->adjust();
	}
	
	/**
	 * ��Z���
	 * @param rhs �搔
	 * @return ��Z��̎��g�ւ̎Q��
	 */
	MultiPrecisionInteger&
	operator*=(const MultiPrecisionInteger& rhs)
	{
		MultiPrecisionInteger
			result(0, this->getMaxColumn() + rhs.getMaxColumn());

		result.isMinusSign = this->isMinusSign ^ rhs.isMinusSign;

		BaseUnit carry = BaseUnit();
		unsigned int offset;
		unsigned int subOffset;
		const size_t rhsMaxColumn = rhs.getMaxColumn();
		const size_t lhsMaxColumn = this->getMaxColumn();
		for (subOffset = 0; subOffset < rhsMaxColumn; ++subOffset)
		{
			carry = 0;
			for (offset = 0; offset < lhsMaxColumn; ++offset)
			{
				CalcBase tempResult = 
					(CalcBase)rhs.value[subOffset] *
					this->value[offset] +
					carry +
					result.value[offset+subOffset];
				carry = static_cast<BaseUnit>(
					(tempResult >> (sizeof(BaseUnit) * 8)) & 
					getMaxBaseUnit());

				result.value[offset+subOffset]=
					static_cast<BaseUnit>(tempResult & getMaxBaseUnit());
			}
			while (carry)
			{
				CalcBase tempResult =
					(CalcBase)carry + result.value[offset+subOffset];
				carry = static_cast<BaseUnit>(
					(tempResult >> (sizeof(BaseUnit) * 8)) & 
					getMaxBaseUnit());

				result.value[offset+subOffset]=
					static_cast<BaseUnit>(tempResult & getMaxBaseUnit());

				++offset;
			}
		}

		this->swap(result);
		return this->adjust();
	}

	/**
	 * ���Z���
	 * @param rhs ����
	 * @return ����ꂽ��
	 */
	MultiPrecisionInteger&
	operator/=(const MultiPrecisionInteger& rhs)
	{
		MultiPrecisionInteger modulo;
		return this->divide(rhs, modulo);
	}

	/**
	 * ��]���
	 * @param rhs ����
	 * @return �]������������g�ւ̎Q��
	 */
	MultiPrecisionInteger&
	operator%=(const MultiPrecisionInteger& rhs)
	{
		MultiPrecisionInteger modulo;
		this->divide(rhs, modulo);
		this->swap(modulo);
		return *this;
	}
	
	/**
	 * �O�u�C���N�������g
	 * @return �C���N�������g��̎��g�ւ̎Q��
	 */
	MultiPrecisionInteger& operator++()
	{
		if (this->isMinusSign == true)
			return this->negate().operator--().negate();

		if (this->value[0] != getMaxBaseUnit())
			this->value[0] += 1;
		else
			*this += 1U;

		return *this;
	}

	/**
	 * ��u�C���N�������g
	 * @return �C���N�������g�O�̒l
	 * @todo �O�u�C���N�������g�ֈϏ���������ւ̕ύX
	 */
	MultiPrecisionInteger operator++(int)
	{
		if (this->isMinusSign == true)
			return this->negate().operator--(0).negate();

		MultiPrecisionInteger result(*this);
		if (this->value[0] != getMaxBaseUnit())
			++this->value[0];
		else
			*this += 1;

		return result;
	}

	/**
	 * �O�u�f�N�������g
	 * @return �f�N�������g��̎��g�ւ̎Q��
	 */
	MultiPrecisionInteger& operator--()
	{
		if (this->isMinusSign == true)
			return this->negate().operator++().negate();
		
		if (!this->value[0] != 0)
			--this->value[0];
		else
			*this -= 1U;

		return *this;
	}

	/**
	 * ��u�f�N�������g
	 * @return �f�N�������g�O�̒l
	 * @todo �O�u�f�N�������g�ւ̈Ϗ����g���������ւ̕ύX
	 */
	MultiPrecisionInteger operator--(int)
	{
		if (this->isMinusSign == true)
			return this->negate().operator++(0).negate();

		MultiPrecisionInteger result(*this);
		if (!this->value[0] != 0)
			--this->value[0];
		else
			*this -= 1;

		return result;
	}

	/**
	 * ���V�t�g
	 * @param �V�t�g����r�b�g��
	 * @return n �V�t�g��̃I�u�W�F�N�g
	 */
	MultiPrecisionInteger operator<<(int n) const
	{
		return MultiPrecisionInteger(*this).operator<<=(n);
	}

	/**
	 * ���V�t�g
	 * @param �V�t�g����r�b�g��
	 * @return n �V�t�g��̎��g�ւ̎Q��
	 */
	MultiPrecisionInteger& operator<<=(int n)
	{
		const size_t downShiftBitCount =
			(sizeof(BaseUnit) * 8) -
			(static_cast<unsigned int>(n) % (sizeof(BaseUnit) * 8));

		const size_t shiftColumnCount =
			static_cast<unsigned int>(n) / (sizeof(BaseUnit) * 8) +
			(downShiftBitCount == 0 ? 0 : 1);

		MPVector temp(this->value.size() + shiftColumnCount);
		std::copy(this->value.begin(),
				  this->value.end(),
				  temp.begin() + shiftColumnCount);

		this->value.swap(temp);
		if (downShiftBitCount > 0)
			this->operator>>=(static_cast<int>(downShiftBitCount));

		return this->adjust();
	}

	/**
	 * ���V�t�g
	 * @param n �V�t�g����r�b�g��
	 * @return �V�t�g��̒l
	 */
	MultiPrecisionInteger operator>>(int n) const
	{
		return MultiPrecisionInteger(*this).operator>>=(n);
	}

	/**
	 * ���V�t�g
	 * @param n �V�t�g����r�b�g��
	 * @return �V�t�g��̎��g�ւ̎Q��
	 */
	MultiPrecisionInteger& operator>>=(int n)
	{
		const size_t shiftColumnCount =
			static_cast<unsigned int>(n) / (sizeof(BaseUnit) * 8);

		if (shiftColumnCount > 0)
			this->value.erase(this->value.begin(),
							  this->value.begin() + shiftColumnCount);

		const size_t shiftBitCount =
			static_cast<unsigned int>(n) % (sizeof(BaseUnit) * 8);

		assert(shiftBitCount < (sizeof(BaseUnit) * 8));
		if (shiftBitCount > 0)
		{
			BaseUnit downCarry = 0;
			const BaseUnit mask =
				static_cast<BaseUnit>((~0U << shiftBitCount) &
				getMaxBaseUnit());

			for (size_t column = this->getMaxColumn();
				 column > 0;
				 --column)
			{
				const BaseUnit newValue = 
					downCarry |
					static_cast<BaseUnit>(
						(this->value[column-1] >> shiftBitCount));
					
				downCarry = static_cast<BaseUnit>(
					(this->value[column-1] & ~mask) <<
					(sizeof(BaseUnit) * 8 - shiftBitCount));
			
				this->value[column-1] = newValue;
			}
		}

		return this->adjust();
	}

	/** 
	 * �������Z�q
	 * @return �����l�Ȃ�true
	 * @param rhs ��r�Ώ�
	 */
	bool operator==(const MultiPrecisionInteger& rhs) const
	{
		if (this->isZero() && rhs.isZero())
			return true;

		if (this->isMinusSign != rhs.isMinusSign)
			return false;

		if (this->getMaxColumn() != rhs.getMaxColumn())
			return false;

		const int maxLength = 
			static_cast<const int>(this->getMaxColumn());
		for (int offset = 0; offset < maxLength; ++offset)
		{
			if (this->value[offset] != rhs.value[offset])
				return false;
		}

		return true;
	}

	/**
	 * �ے艉�Z�q
	 * @param rhs ��r�Ώ�
	 * @return �Ⴄ�l�������Ă����true
	 */
	bool operator!=(const MultiPrecisionInteger& rhs) const
	{
		return !this->operator==(rhs);
	}

	/**
	 * LessThan��r���Z�q
	 * @param rhs ��r�Ώ�
	 * @return ��r�Ώۂ�菬�������true
	 */
	bool operator<(const MultiPrecisionInteger& rhs) const
	{
		if (this->isMinusSign == true && rhs.isMinusSign == false)
			return true;
		if (this->isMinusSign == false && rhs.isMinusSign == true)
			return false;

		if (this->getMaxColumn() > rhs.getMaxColumn())
			return this->isMinusSign ?  true : false;
		else if (this->getMaxColumn() < rhs.getMaxColumn())
			return this->isMinusSign ? false : true;

		const int maxLength = 
			static_cast<const int>(this->getMaxColumn()-1);
		for (int offset = maxLength; offset >= 0; --offset)
		{
			if (this->value[offset] > rhs.value[offset])
				return this->isMinusSign ? true : false;
			else if (this->value[offset] < rhs.value[offset])
				return this->isMinusSign ? false : true;
		}
		return this->isMinusSign ? true : false;
	}

	/**
	 * GreaterEqual��r���Z�q
	 * @param rhs ��r�Ώ�
	 * @return �ΏۂƓ������܂��͂��傫�����true
	 */
	bool operator>=(const MultiPrecisionInteger& rhs) const
	{
		return !this->operator<(rhs);
	}

	/**
	 * GreaterThan��r���Z�q
	 * @param rhs ��r�Ώ�
	 * @return �Ώۂ��傫�����true
	 */
	bool operator>(const MultiPrecisionInteger& rhs) const
	{
		if (this->isMinusSign == false && rhs.isMinusSign == true)
			return true;
		if (this->isMinusSign == true && rhs.isMinusSign == false)
			return false;

		if (this->getMaxColumn() < rhs.getMaxColumn())
			return this->isMinusSign ? true : false;
		else if (this->getMaxColumn() > rhs.getMaxColumn())
			return this->isMinusSign ? false : true;

		const int maxLength =
			static_cast<const int>(this->getMaxColumn()-1);
		for (int offset = maxLength; offset >= 0; --offset)
		{
			if (this->value[offset] < rhs.value[offset])
				return this->isMinusSign ? true : false;
			else if (this->value[offset] > rhs.value[offset])
				return this->isMinusSign ? false : true;
		}
		return this->isMinusSign ? true : false;
	}

	/**
	 * LessThan��r���Z�q
	 * @param rhs ��r�Ώ�
	 * @return �ΏۂƓ�������菬�������true
	 */
	bool operator<=(const MultiPrecisionInteger& rhs) const
	{
		return !this->operator>(rhs);
	}

	/**
	 * �^�f���̐���
	 * @param bitToLength ��������^�f���̃r�b�g��
	 * @param random Random�C���^�t�F�[�X�����[�������I�u�W�F�N�g
	 * @param checkDepth ���r��-�~���[�@�ɂ��`�F�b�N�p�X
	 * @return ���������^�f��
	 * @todo cehckDepth�����߂�4���炢�ɂ͂��Ƃ����E�E�E
	 */
	static MultiPrecisionInteger
	getProbablePrime(const size_t bitToLength,
					 Random& random,
					 const unsigned int checkDepth = 2)
	{
		typedef std::vector<BaseUnit> BaseUnitArray;

		const size_t numberOfBytes =
			(bitToLength / 8) +
			((bitToLength % 8) == 0 ? 0 : 1);


		BaseUnitArray numberSource =
			random.getRandomDoubleWordVector(numberOfBytes / 4);
		MultiPrecisionInteger baseNumber =
			MultiPrecisionInteger::makeNumberOfBitSafe(numberSource);

		for (;;)
		{
			Sieve<MultiPrecisionInteger> sieve(baseNumber);

			for (unsigned int offset = 0; offset < sieve.size(); ++offset)
			{
				if (sieve.isCompositeNumber(offset))
					continue;

				// �f���\���Ȃ��Ȃ���or���ׂĂ̑f������蔲����

				for (unsigned int primeCheckDepth = 0;
					 primeCheckDepth < checkDepth;
					 ++primeCheckDepth)
				{
					if (primeCheckDepth == 0)
					{
						if (RabinPrimeTest(
								baseNumber + offset,
								MultiPrecisionInteger(2U)) == false)
							break;
					}
					else
					{
						// �؋����̍쐬
						if (RabinPrimeTest(
								baseNumber + offset,
								MultiPrecisionInteger(primeCheckDepth + 2U))
							== false)
							break;
						else
							if (primeCheckDepth == (checkDepth - 1))
							{
								return baseNumber + offset;
							}
					}
				}
			}
			// ⿂͈̔͌��E�𒴂������߁A��l�����炵�čĎ��s
			baseNumber += (BaseUnit)sieve.size();
		}
	}
};


typedef MultiPrecisionInteger<> MPInteger;

#endif /* MPI_HPP_ */