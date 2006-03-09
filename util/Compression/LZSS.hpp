#ifndef LZSS_HPP_
#define LZSS_HPP_

#include <util/Calculate.hpp>
#include <algorithm>
#include <stdexcept>
#include <cassert>

/**
 * ���֐�
 * ���������z��𐔊w�I���֐����g���ċ�؂��ĕ\������
 * @param RingSize �����O�o�b�t�@�̃T�C�Y
 * @todo �ŏ��T�C�Y0����̊g�������ւ̕ύX
 */
template <size_t WindowSize = 4096>
class WindowFunction
{
private:
	/**
	 * ���̐擪�ʒu
	 */
	const char* windowHead;
	/**
	 * ���̏I�[�ʒu(�A�N�Z�X�\�ʒu���ЂƂ�)
	 */
	const char* windowTail;

	/**
	 * ���������̏I�[�ʒu(�A�N�Z�X�\�ʒu���ЂƂ�)
	 */
	const char* tail;

	/**
	 * ���T�C�Y�̊g��
	 * @param widenSize �����g�傷��T�C�Y
	 * @note �ő呋�T�C�Y�̏ꍇ�͉������Ȃ�
	 * @return �����g�����ꂽ�ꍇ��true
	 */
	bool widen(const size_t widenSize = 1)
	{
		if (getCurrentWindowSize() + widenSize < getMaxWindowSize() &&
			isSlidable(widenSize))
		{
			windowTail += widenSize;
			return true;
		}

		return false;
	}

public:
	/**
	 * �R���X�g���N�^
	 * @param head_ �������̊J�n�ʒu
	 * @param tail_ �������̏I�[�ʒu
	 */
	WindowFunction(const char* head_, const char* tail_):
		windowHead(head_),
		windowTail(head_ + 1),
		tail(tail_)
	{
		assert(head_ < tail_);
	}

	/**
	 * �R���X�g���N�^
	 * @param head_ �������̊J�n�ʒu
	 * @param length �������̈�̃T�C�Y
	 */
	WindowFunction(const char* head, const size_t length):
		windowHead(head),
		windowTail(head + 1),
		tail(head+length)
	{}

	/**
	 * ���݂̑��T�C�Y�̎擾
	 * @return ���݂̑��T�C�Y
	 */
	size_t getCurrentWindowSize() const
	{
		return windowTail - windowHead;
	}

	/**
	 * �ő呋�T�C�Y���擾
	 * @return �ő呋�T�C�Y
	 */
	size_t getMaxWindowSize() const
	{
		return WindowSize;
	}

	/**
	 * ����1�o�C�g�X���C�h
	 * @exception std::out_of_range �X���C�h�͈͂𒴂��Ă����ꍇ
	 */
	void slide() throw(std::out_of_range)
	{
		slide(1);
	}

	/**
	 * ���̃X���C�h
	 * @param slideSize ���X���C�h����o�C�g��
	 * @exception std::out_of_range �X���C�h�͈͂𒴂��Ă����ꍇ
	 */
	void slide(const size_t slideSize) throw(std::out_of_range)
	{
		if (!isSlidable(slideSize))
			throw std::out_of_range("�͈͊O�̃A�N�Z�X�ł�");

		if (!widen(slideSize))
		{
			windowHead += slideSize;
			windowTail += slideSize;
		}

		assert(getCurrentWindowSize() <= getMaxWindowSize());
	}

	/**
	 * ���o�C�g�X���C�h�ł��邩
	 * @return �X���C�h�\�o�C�g��
	 */
	size_t getSlidableSize() const
	{
		return static_cast<size_t>(tail - windowTail);
	}

	/**
	 * �܂��X���C�h�\���ǂ���
	 * @return �X���C�h�\�Ȃ�true
	 */
	bool isSlidable() const
	{
		return windowTail < tail;
	}
	
	/**
	 * �܂��X���C�h�\���ǂ���
	 * @param �X���C�h������o�C�g��
	 * @return �X���C�h�\�Ȃ�true
	 */
	bool isSlidable(const size_t slideSize) const
	{
		return (windowTail + slideSize) <= tail;
	}

	/**
	 * ���̈�̃f�[�^�̎擾
	 * @param offset
	 * @return �I�t�Z�b�g�ʒu�̃o�C�g�f�[�^
	 * @exception std::out_of_range �̈�O�փA�N�Z�X�����ꍇ
	 */
	char getCharacter(const size_t offset) const throw(std::out_of_range)
	{
		if (offset > getCurrentWindowSize())
			throw std::out_of_range("offset out of range.");

		return *(windowHead + offset);
	}
};


/**
 * �g�[�N���̊e��|���V�[�y�уw���p�֐�
 * @param positionSize �ʒu��\���f�[�^����߂�r�b�g��
 * @param lengthSize ����������킷�f�[�^����߂�r�b�g��
 */
template <size_t positionSize = 12,
		  size_t lengthSize = 4>
class TokenPolicy
{
	friend class TokenPolicyTest;

private:
	/**
	 * �g�[�N���f�[�^����r�b�g�\���ւ̕ϊ�
	 * @param token �g�[�N���f�[�^
	 * @return �r�b�g�\���ł̒l
	 */
	static unsigned int tokenToBitReps(const token_data_t& token)
	{
		unsigned int bitReps = 0;

		for (token_data_t::const_iterator itor = token.begin();
			 itor != token.end();
			 ++itor)
		{
			bitReps <<= 8;
			bitReps |= *itor;
		}

		return bitReps;
	}

public:
	/// �g�[�N���f�[�^�̌^
	typedef std::vector<char>  token_data_t;

	enum {
		/// �ʒu���̐�߂�r�b�g��
		PositionSize = positionSize,
		
		/// �������̐�߂�r�b�g��
		LengthSize = lengthSize,

		/// �g�[�N���̒����̃o�C�g��
		TokenLength = (positionsSize + lengthSize) / 8
	};

	/**
	 * ��}�b�`�g�[�N���f�[�^�̍쐬
	 * @param ch ����
	 * @return ��}�b�`�g�[�N���f�[�^
	 * @note <MSB>[ch][length=0]<LSB>��BigEndian�Ƃ���
	 * �f�[�^�������Ă܂��B
	 */
	static token_data_t buildNoMatchToken(const char ch)
	{
		const size_t length = 0;
		const size_t character =
			static_cast<size_t>(std::char_traits<char>::to_int_type(ch));
		const unsigned int bitReps = (character << lengthSize) | length;
		token_data_t result(TokenSize);
		for (int index = 1; index <= TokenLength; ++index)
		{
			result.push_back((bitReps >>
							  ((TokenLength - index) * 8)) & 0xff); 
		}
	}

	/**
	 * �}�b�`�g�[�N���f�[�^�̍쐬
	 * @param position �}�b�`�ʒu
	 * @param length �}�b�`��
	 * @return �}�b�`�g�[�N���f�[�^
	 * @note <MSB>[position][length]<LSB>��BigEndian�Ƃ���
	 * �f�[�^�������Ă܂��B
	 */
	static token_data_t buildMatchToken(const size_t position,
										const size_t length)
	{
		const unsigned int bitReps = (position << lengthSize) | length;
		token_data_t result(TokenSize);
		for (int index = 1; index <= TokenLength; ++index)
		{
			result.push_back((bitReps >>
							  ((TokenLength - index) * 8)) & 0xff); 
		}
	}

	/**
	 * �}�b�`�g�[�N���f�[�^����ʒu�f�[�^�̎��o��
	 * @param token �g�[�N���f�[�^
	 * @return �ʒu�f�[�^�̒l
	 */
	static size_t getPosition(const token_data_t& token)
	{
		assert(token.size() == TokenLength);
		const unsigned int positionMask = (1 << PositionSize) - 1;

		return static_cast<size_t((tokenToBitReps(token) >> LengthSize)
								  & positionMask);
	}

	/**
	 * �}�b�`�g�[�N���f�[�^����}�b�`���f�[�^�̎��o��
	 * @param token �g�[�N���f�[�^
	 * @return �}�b�`���f�[�^�̒l
	 */
	static size_t getLength(const token_data_t& token)
	{
		assert(token.size() == TokenLength);
		const unsigned int lengthMask = (1 << LengthSize) - 1;
	
		return static_cast<size_t>(tokenToBitReps(token) & lengthMask);
	}
};

/**
 * �������ʂƂ��ĕԂ��g�[�N���̊��B
 * @param positionSize �ʒu��\���f�[�^����߂�r�b�g��
 * @param lengthSize ����������킷�f�[�^����߂�r�b�g��
 */
template <typename TokenPolicyType>
class AbstructToken
{
private:
	typedef TokenPolicyType token_policy_t;

public:
	/**
	 * �R���X�g���N�^
	 */
	AbstructToken()
	{}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~AbstructToken() throw()
	{}

	/**
	 * �g�[�N���̒�����Ԃ�
	 */
	size_t getTokenLength() const
	{
		return token_policy_t::TokenLength;
	}

	/**
	 * �o�C�g��\���Ƃ��Ẵg�[�N���̎擾
	 * @return �o�C�g��\���Ƃ��Ẵg�[�N��
	 */
	virtual typename TokenPolicy::token_data_t
	toByteReplesentation() const = 0;
};

/**
 * �O���Q�ƂƂ��Č��������ʒu�A�����������Z�b�g�Ƃ����g�[�N��
 * @param TokenPolicyType �g�[�N���̊e��|���V�[���������|���V�[�N���X
 */
template <typename TokenPolicyType>
class ReferenceValueToken : public AbstructToken<TokenPolicyType>
{
private:
	typename TokenPolicyType token_policy_t;
	
	/**
	 * ���������ʒu
	 */
	size_t position;

	/**
	 * �}�b�`��������
	 */
	size_t length;

public:
	virtual typename TokenPolicyType::token_data_t 
	toByteReplesentation() const
	{
		return token_policy_t::buildMatchToken(position, length);
	}
};

/**
 * ������Ȃ����������g�[�N��
 */
template <typename TokenPolicyType>
class RealValueToken : public AbstructToken<TokenPolicyType>
{
private:
	const char character;

public:
	/**
	 * �R���X�g���N�^
	 * @param ch �ێ����镶��
	 */
	RealValueToken(const ch):
		character(ch)
	{}

	/**
	 * �f�X�g���N�^
	 */
	~RealValueToken() throw()
	{}

	/**
	 * �ێ����Ă��镶���̎擾
	 * @return �ێ����Ă��镶��
	 */
	char getCharacter() const
	{
		return character;
	}

	/**
	 * �ێ����Ă��镶���̕ύX
	 * @param ch �V���ɕێ������镶��
	 */
	void setCharacter(const char ch)
	{
		character = ch;
	}

	virtual typename TokenPolicyType::token_data_t 
	toByteReplesentation() const
	{
		return token_policy_t::buildNoMatchToken(character);
	}
};

/**
 * LZSS���k�A���S���Y��
 * @param windowSize �X���C�h���T�C�Y
 * @param tokenSizeOfBits �g�[�N���Ɏg�p�ł���r�b�g�T�C�Y
 * @todo ���Ă̒ʂ�S�R��������Ė����ł��B�Ȃ̂Ŕ�����ł������̂Ŏ����ƃe�X�g�ꎮ��
 * ������D�悵�܂��傤�B
 */
template<size_t windowSize, size_t tokenSizeOfBits>
class Lzss
{
private:
public:
};

#endif /* LZSS_HPP_ */
