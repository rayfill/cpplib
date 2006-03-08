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
 * �������ʂƂ��ĕԂ��g�[�N���̊��B
 */
class AbstructToken
{
public:
	virtual ~AbstructToken() {}
};

/**
 *  �O���Q�ƂƂ��Č������������A�ʒu�A�����������Z�b�g�Ƃ����g�[�N��
 */
class ReferenceValueToken : public AbstructToken
{};

/**
 * ������Ȃ����������g�[�N��
 */
class RealValueToken : public AbstructToken
{};

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
