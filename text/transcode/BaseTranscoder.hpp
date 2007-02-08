#ifndef BASETRANSCODER_HPP_
#define BASETRANSCODER_HPP_

/**
 * トランスコーダ（文字変換機）基底クラス
 */
template<typename UTFType = wchar_t>
class BaseTranscoder
{
public:
	typedef UTFType utf_t;

	BaseTranscoder()
	{}

	virtual ~BaseTranscoder()
	{}

	virtual std::basic_string<char>
	toNativeCode(const std::basic_string<utf_t>& unicodeStr) = 0;

	virtual std::basic_string<utf_t>
	toUnicode(const std::basic_string<char>& nativeStr) = 0;
};
#endif /* BASETRANSCODER_HPP_ */
