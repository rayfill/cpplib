#ifndef BASETRANSCODER_HPP_
#define BASETRANSCODER_HPP_

/**
 * トランスコーダ（文字変換機）基底クラス
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
	 * 多バイト文字かどうかの判別
	 */
	virtual bool isLeadedChar(const char codePoint)
	{
		return false;
	}
	
}
#endif /* BASETRANSCODER_HPP_ */
