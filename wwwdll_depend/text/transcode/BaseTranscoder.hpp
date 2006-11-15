#ifndef BASETRANSCODER_HPP_
#define BASETRANSCODER_HPP_

/**
 * �g�����X�R�[�_�i�����ϊ��@�j���N���X
 */
class BaseTranscoder
{
public:
	BaseTranscoder()
	{}

	virtual ~BaseTranscoder(){} = 0;

	virtual char toNativeCodeChar(const wchar_t codePoint) = 0;

	virtual wchar_t toUnicodeChar(const int codePoint) = 0;

	/**
	 * ���o�C�g�������ǂ����̔���
	 */
	virtual bool isLeadedChar(const char codePoint)
	{
		return false;
	}
	
}
#endif /* BASETRANSCODER_HPP_ */
