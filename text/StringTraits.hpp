#ifndef STRINGTRAITS_HPP_
#define STRINGTRAITS_HPP_

#include <string>
#include <cstdlib>
#include <algorithm>

/**
 * Multibyte<->WideCharコード変換器
 * @param DestType 出力文字種
 * @param SourceType 入力文字種
 * これ自身はプレースホルダです。実体はテンプレート特殊化されたもののほうです。
 * @todo 変換処理をWin32APIに委譲しているだけなのでプラットホーム独立にするために
 * http://www.unicode.org/Public/MAPPINGS/ 以下のテーブルからの変換器の作成。
 * そうした場合、デフォルトコードページがなくなるのでその指定方法の追加。
 */
template <typename DestType, typename SourceType>
class CodeConvert
{
public:
	DestType codeConvert(const SourceType&)
	{
		return DestType();
	}
};

/**
 * MultiByte -> WideChar コード変換器特殊化
 */
template <>
class CodeConvert<std::wstring, std::string>
{
public:
	/**
	 * コード変換処理
	 * @param str マルチバイト文字列
	 * @return 変換されたWideChar文字列
	 */
	std::wstring codeConvert(const std::string& str)
	{
		const size_t translatedLength =
			std::mbstowcs(NULL ,str.c_str(),str.length()+1);

		wchar_t* translated = NULL;

		translated = new wchar_t[translatedLength];
		std::mbstowcs(translated, str.c_str(), translatedLength);
		std::wstring result(translated, translatedLength);
		delete[] translated;

		return result;
	}
};


/**
 * WideChar -> MultiByte コード変換器特殊化
 */
template <>
class CodeConvert<std::string, std::wstring>
{
public:
	/**
	 * コード変換処理
	 * @param str WideChar文字列
	 * @return 変換されたMultiByte文字列
	 */
	std::string codeConvert(const std::wstring& str)
	{
		const size_t translatedLength =
			wcstombs(NULL, str.c_str(), str.length() + 1);

		char* translated = NULL;

		translated = new char[translatedLength];
		wcstombs(translated, str.c_str(), translatedLength);
		std::string result(translated, translatedLength);
		delete[] translated;

		return result;
	}
};

/**
 * 変換器ヘルパ
 * @param CharType 出力キャラクタ種
 * 実体は特殊化によって定義されます
 * @todo WideChar -> MultiByte版が無いのでソレの追加
 */
template <typename CharType>
class StringTraits
{
public:
	/**
	 * 変換関数
	 * @param str 変換元文字列
	 */
	std::basic_string<CharType> stringTraits(const std::string& str)
	{
		return std::basic_string<CharType>();
	}
};

/**
 * 変換器ヘルパ
 * MultiByte -> MultiByte (無変換)
 */
template <>
class StringTraits<char>
{
public:
	std::basic_string<char> stringTraits(const std::string& str)
	{
		return str;
	}
};

/**
 * 変換器ヘルパ
 * MultiByte -> MultiByte (Wide文字変換)
 */
template <>
class StringTraits<wchar_t>
{
public:
	std::basic_string<wchar_t> stringTraits(const std::string& str)
	{
		return CodeConvert<std::wstring, std::string>().codeConvert(str);
	}
};

#endif /* STRINGTRAITS_HPP_ */
