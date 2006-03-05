#ifndef STRINGTRAITS_HPP_
#define STRINGTRAITS_HPP_

#include <string>
#include <cstdlib>
#include <algorithm>

/**
 * Multibyte<->WideChar�R�[�h�ϊ���
 * @param DestType �o�͕�����
 * @param SourceType ���͕�����
 * ���ꎩ�g�̓v���[�X�z���_�ł��B���̂̓e���v���[�g���ꉻ���ꂽ���̂̂ق��ł��B
 * @todo �ϊ�������Win32API�ɈϏ����Ă��邾���Ȃ̂Ńv���b�g�z�[���Ɨ��ɂ��邽�߂�
 * http://www.unicode.org/Public/MAPPINGS/ �ȉ��̃e�[�u������̕ϊ���̍쐬�B
 * ���������ꍇ�A�f�t�H���g�R�[�h�y�[�W���Ȃ��Ȃ�̂ł��̎w����@�̒ǉ��B
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
 * MultiByte -> WideChar �R�[�h�ϊ�����ꉻ
 */
template <>
class CodeConvert<std::wstring, std::string>
{
public:
	/**
	 * �R�[�h�ϊ�����
	 * @param str �}���`�o�C�g������
	 * @return �ϊ����ꂽWideChar������
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
 * WideChar -> MultiByte �R�[�h�ϊ�����ꉻ
 */
template <>
class CodeConvert<std::string, std::wstring>
{
public:
	/**
	 * �R�[�h�ϊ�����
	 * @param str WideChar������
	 * @return �ϊ����ꂽMultiByte������
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
 * �ϊ���w���p
 * @param CharType �o�̓L�����N�^��
 * ���͓̂��ꉻ�ɂ���Ē�`����܂�
 * @todo WideChar -> MultiByte�ł������̂Ń\���̒ǉ�
 */
template <typename CharType>
class StringTraits
{
public:
	/**
	 * �ϊ��֐�
	 * @param str �ϊ���������
	 */
	std::basic_string<CharType> stringTraits(const std::string& str)
	{
		return std::basic_string<CharType>();
	}
};

/**
 * �ϊ���w���p
 * MultiByte -> MultiByte (���ϊ�)
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
 * �ϊ���w���p
 * MultiByte -> MultiByte (Wide�����ϊ�)
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
