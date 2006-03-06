#ifndef RJINDAEL_HPP_
#define RJINDAEL_HPP_

#include <vector>

/**
 * ���C���_�[���Í�
 * @TODO �܂����������S�ł͂���܂���
 */
template <size_t cipherBitsSize = 128, size_t keyBitsSize = 128>
class Rijndael
{
	friend class RijndaelTest;

private:
	const int numberOfKey;
	const int numberOfBlock;

	/**
	 * ���C���_�[���㐔
	 * ���C���_�[���Í��̃x�[�X�ɂȂ�܂�
	 */
	class RijndaelNumber
	{
		friend class RijndaelTest;

	private:
		unsigned char number;

		unsigned char modulus(unsigned short source) const
		{
			if (source > 0xff)
				return
					static_cast<unsigned char>((source ^ 0x1B) & 0xff);
			else
				return
					static_cast<unsigned char>(source);
		}

		unsigned char xtime(const unsigned char value) const
		{
			return modulus(static_cast<unsigned short>(value) << 1);
		}

	public:
		RijndaelNumber(const unsigned char value)
			: number(value)
		{}

		RijndaelNumber(const RijndaelNumber& value)
			: number(value.number)
		{}

		~RijndaelNumber()
		{}

		RijndaelNumber invert() const
		{
			if (this->number == 0)
				return RijndaelNumber(static_cast<unsigned char>(0));

			for (int i = 1; i < 256; ++i)
			{
				if (RijndaelNumber(*this).
					operator*=(RijndaelNumber(
								   static_cast<unsigned char>(i))).
					number == 1)
					return RijndaelNumber(static_cast<unsigned char>(i));
			}

			throw std::logic_error("�t����������܂���B���W�b�N�G���[");
		}

		RijndaelNumber& operator+=(const RijndaelNumber& rhs)
		{
			this->number ^= rhs.number;
			return *this;
		}

		RijndaelNumber operator+(const RijndaelNumber& rhs)
		{
			return RijndaelNumber(*this).operator+=(rhs);
		}

		RijndaelNumber& operator<<=(const int shiftCount)
		{
			if (shiftCount >= 256)
				throw std::logic_error("shift count over than 8.");

			for (int count = 0; count < shiftCount; ++count)
				this->number = xtime(this->number);

			return *this;
		}

		RijndaelNumber& RotateLeft()
		{
			number = static_cast<unsigned char>(
				(((number << 1) & 0xfe) | ((number >> 7) & 0x01)) & 0xff);
			return *this;
		}

		RijndaelNumber& RotateRight()
		{
			number = static_cast<unsigned char>(
				(((number >> 1) & 0x7f) | ((number << 7) & 0x80)) & 0xff);
			return *this;
		}

		RijndaelNumber& operator*=(const RijndaelNumber& rhs)
		{
			unsigned char& result = this->number;
			unsigned char temp = this->number;

			result = 0;
			for (unsigned int offset = 0;
				 offset < (sizeof(unsigned char) * 8);
				 ++offset)
			{
				if (rhs.number & (1 << offset))
				{
					result ^= temp;
				}
				temp = xtime(temp);
			}

			return *this;
		}

		RijndaelNumber operator*(const RijndaelNumber& rhs) const
		{
			return RijndaelNumber(*this).operator*=(rhs);
		}

		RijndaelNumber& operator=(const RijndaelNumber& rhs)
		{
			this->number = rhs.number;
			return *this;
		}

		bool operator==(const RijndaelNumber& target) const
		{
			return this->number == target.number;
		}

		bool operator!=(const RijndaelNumber& target) const
		{
			return this->number != target.number;
		}

		bool operator<(const RijndaelNumber& target) const
		{
			return this->number < target.number;
		}

		bool operator<=(const RijndaelNumber& target) const
		{
			return !(this->number > target.number);
		}

		bool operator>(const RijndaelNumber& target) const
		{
			return this->number > target.number;
		}

		bool operator>=(const RijndaelNumber& target) const
		{
			return !(this->number < target.number);
		}

	};

	/**
	 * �Í������̃��E���h���擾
	 */
	const int getNumberOfRounds() const
	{
		if (numberOfBlock > numberOfKey)
			return numberOfBlock + 6;
		else
			return numberOfKey + 6;
	}

	/**
	 * SBox�������{�������C���_�[�����̎擾
	 * @param value �������郉�C���_�[����
	 * @return �������ꂽ���C���_�[����
	 */
	const RijndaelNumber
	getSBoxValue(const RijndaelNumber& value) const
	{
		const Rijndael<>::RijndaelNumber constant(0x63);
		Rijndael<>::RijndaelNumber rn(value);
		rn = rn.invert();
		Rijndael<>::RijndaelNumber result(rn);
		rn.RotateLeft();
		result += rn;
		rn.RotateLeft();
		result += rn;
		rn.RotateLeft();
		result += rn;
		rn.RotateLeft();
		result += rn;
		result += constant;

		return result;
	}

	/**
	 * SBox�������ꂽ���C���_�[�������猳�̃��C���_�[���������߂�
	 * @TODO ������
	 */
	const RijndaelNumber
	getInvertSBoxValue(const RijndaelNumber& value) const
	{
	}

public:
	/**
	 * �R���X�g���N�^
	 */
	Rijndael() throw()
		: numberOfKey(keyBitsSize / 8),
		  numberOfBlock(cipherBitsSize / 8)
	{
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~Rijndael()
	{
	}

	/**
	 * �Í�������
	 * @param plaintext �����o�C�g��
	 * @param key �Í���
	 * @return �Í��������{���ꂽ�o�C�g��
	 */
	std::vector<unsigned char>
	encrypt(const std::vector<unsigned char>& plaintext,
			const std::vector<unsigned char>& key)
	{
		std::vector<RijndaelNumber> state(numberOfBlock*4);
		std::vector<RijndaelNumber> expandedKey(4);

		if (plaintext.size() != numberOfBlock * 4 ||
			key.size() != numberOfKey * 4)
			throw std::runtime_error(
				"plain text or password length is invalid.");

		// copy plaintext to state
		for (size_t offset = 0; offset < plaintext.size(); ++offset)
			state[offset % 4][offset / 4] = RijndaelNumber(plaintext[offset]);

		// key expansion.
	}

private:
	RijndaelNumber ByteSub(const RijndaelNumber& rhs) const
	{
		reutrn (rhs * RijndaelNumber(0x8f)) + RijndaelNumber(0x63);
	}


	public:
};

#endif /*  RJINDAEL_HPP_ */
