#ifndef BASE64_HPP_
#define BASE64_HPP_

#include <vector>
#include <string>
#include <cassert>

/**
 * base64 �G��/�f�R�[�_
 * @see http://www.ietf.org/rfc/rfc3548.txt
 * @todo �X�g���[���Ή��ŁAtemplate�x�[�X�̃C�e���[�^�ł̃��\�b�h�̗p��
 */
class Base64
{
	friend class Base64Test;

private:
	/**
	 * �ϊ��e�[�u��������擾
	 * @return �ϊ��e�[�u��������ւ̃|�C���^�B
	 * �ÓI�ɑ��݂���̂ŃR�s�[���Ƃ�K�v�͂Ȃ��B
	 * @note '=' ��65�Ԗڂɂ���̂�modulo64��0�Ɠ��l�ɂȂ�̂𗘗p����
	 * �f�R�[�h���ɓ��ʏ��������Ȃ��悤�ɂ��邽�߁B
	 */
	static const char* getBase64Table()
	{
		static const char* base64Table =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/"
			"=";

		return base64Table;
	}

	static const char* getFromBase64Table()
	{
		// �f�o�b�O��e�Ղɂ��邽�ߖ����蓖�Ă�0xcc�Ƀ}�b�v���Ă���
		static const char fromBase64Table[] = {
			// 0x00
			0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 
			0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
			// 0x10
			0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
			0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
			// 0x20
			0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
			0xcc, 0xcc, 0xcc, 0x3e, 0xcc, 0xcc, 0xcc, 0x3f,
			// 0x30
			0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
			0x3c, 0x3d, 0xcc, 0xcc, 0xcc, 0x00, 0xcc, 0xcc,
			// 0x40
			0xcc, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
			0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
			// 0x50
			0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
			0x17, 0x18, 0x19, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
			// 0x60
			0xcc, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
			0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
			// 0x70
			0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
			0x31, 0x32, 0x33, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc
		};

		return fromBase64Table;
	}

public:
	/**
	 * �G���R�[�h
	 * @param data �ϊ����ƂȂ�f�[�^
	 * @return �ϊ���̕�����
	 */
	static std::string encode(const std::vector<char>& data)
	{
		typedef std::vector<char>::size_type size_type;

		const char* table = getBase64Table();

		// �ϊ����f�[�^�̒���
		const size_type length = data.size();
		// �ϊ��敶����̒���
		const size_type resultLength = (data.size() + 2 & ~3) / 3 * 4;

		std::string result;
		result.reserve(resultLength);

		for (size_type offset = 0; offset < length; ++offset)
		{
			// 3�o�C�g�P�ʂɃ��b�v�A���E���h����
			switch (offset % 3)
			{
				// 0�o�C�g��
				case 0:
				{
					// xxxxxxoo|oooooooo|oooooooo
					result.push_back(table[(data[offset] >> 2) & 0x3f]);

					// ......xx|xxxxoooo|oooooooo
					result.push_back(table[(data[offset] << 4) & 0x30]);
				}
				break;

				// 1�o�C�g��
				case 1:
				{
					// ......xx|xxxxoooo|oooooooo
					result[result.size() - 1] |= ((data[offset] >> 4) & 0x0f);

					// ........|....xxxx|xxoooooo
					result.push_back(table[(data[offset] << 2) & 0x3c]);
				}
				break;

				// 2�o�C�g��
				case 2:
				{
					// ........|....xxxx|xxoooooo
					result[result.size() - 1] |= ((data[offset] >> 6) & 0x03);

					// ........|........|..xxxxxx
					result.push_back(table[data[offset] & 0x3f]);
				}
				break;

				// �n���h�����O�`�F�b�N�p
				default:
					assert(false); // unreached case.
			}
		}

		// �p�f�B���O�����̖��ߍ���
		while (result.length() < resultLength)
			result.push_back('=');

		return result;
	}

	/**
	 * �f�R�[�h
	 * @param base64String base64�ϊ����ꂽ������
	 * @return ������̃f�[�^
	 */
	static std::vector<char> decode(const std::string& base64String)
	{
		assert((base64String.size() % 4) == 0);

		typedef std::vector<char>::size_type size_type;

		const size_type resultLength =
			base64String.length() / 4 * 3;
		const size_type sourceLength =
			base64String.length();
		const char* reverseTable = getFromBase64Table();


		std::vector<char> result;
		result.reserve(resultLength);

		for (size_type offset = 0; offset < sourceLength; ++offset)
		{
			const bool isContinuable =
				(offset + 1 < sourceLength) && base64String[offset+1] != '=';

			switch (offset % 4)
			{
				case 0:
				{
					// oooooo..|........|........
					result.push_back(
						(reverseTable[base64String[offset]] << 2) & 0xfc);
				}
				break;

				case 1:
				{
					// xxxxxxoo|oooo....|........
					result.back() |=
						((reverseTable[base64String[offset]] >> 4) & 0x03);
					if (isContinuable)
						result.push_back(
							(reverseTable[base64String[offset]] << 4) & 0xf0);
				}
				break;

				case 2:
				{
					// xxxxxxxx|xxxxoooo|oo......
					result.back() |=
						((reverseTable[base64String[offset]] >> 2) & 0x0f);
					if (isContinuable)
						result.push_back(
							(reverseTable[base64String[offset]] << 6) & 0xa0);
				}
				break;

				case 3:
				{
					// xxxxxxxx|xxxxxxxx|xxoooooo
					result.back() |=
						(reverseTable[base64String[offset]] & 0x3f);
				}
				break;

				default:
					assert(false); // not reached.
			}

			if (isContinuable == false)
				break;
		}

		return result;
	}

	
};

#endif /* BASE64_HPP_ */
