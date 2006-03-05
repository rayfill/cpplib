#ifndef LZSS_HPP_
#define LZSS_HPP_

#include <algorithm>

/**
 * �X���C�h���ʒu�w��w���p
 * @param evalValue �]���l
 * @param roundValue ���T�C�Y
 */
template <size_t evalValue, size_t roundValue>
class Round
{
public:
	enum {
		value = (evalValue + roundValue - 1) & ~(roundValue - 1)
	}
};

/**
 * �X���C�h��
 * @param windowSize ���̈�̃T�C�Y
 */
template <size_t windowSize>
class SlideWindow
{
private:
	/**
	 * ���o�b�t�@
	 */
	char windowBuffer[windowSize];

	/**
	 * ���݈ʒu
	 */
	size_t offset;
	
public:
	/**
	 * �����֐�
	 * @param ch ��������
	 * @return ���������ʒu
	 */
	size_t findMatch(char ch)
	{
	
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
