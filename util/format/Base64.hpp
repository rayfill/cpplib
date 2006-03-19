#ifndef BASE64_HPP_
#define BASE64_HPP_

#include <vector>
#include <string>
#include <cassert>

/**
 * base64 エン/デコーダ
 * @see http://www.ietf.org/rfc/rfc3548.txt
 * @todo ストリーム対応版、templateベースのイテレータ版のメソッドの用意
 */
class Base64
{
	friend class Base64Test;

private:
	/**
	 * 変換テーブル文字列取得
	 * @return 変換テーブル文字列へのポインタ。
	 * 静的に存在するのでコピーをとる必要はない。
	 * @note '=' が65番目にいるのはmodulo64で0と等値になるのを利用して
	 * デコード時に特別処理をしないようにするため。
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
		// デバッグを容易にするため未割り当ては0xccにマップしている
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
	 * エンコード
	 * @param data 変換元となるデータ
	 * @return 変換後の文字列
	 */
	static std::string encode(const std::vector<char>& data)
	{
		typedef std::vector<char>::size_type size_type;

		const char* table = getBase64Table();

		// 変換元データの長さ
		const size_type length = data.size();
		// 変換先文字列の長さ
		const size_type resultLength = (data.size() + 2 & ~3) / 3 * 4;

		std::string result;
		result.reserve(resultLength);

		for (size_type offset = 0; offset < length; ++offset)
		{
			// 3バイト単位にラップアラウンドする
			switch (offset % 3)
			{
				// 0バイト目
				case 0:
				{
					// xxxxxxoo|oooooooo|oooooooo
					result.push_back(table[(data[offset] >> 2) & 0x3f]);

					// ......xx|xxxxoooo|oooooooo
					result.push_back(table[(data[offset] << 4) & 0x30]);
				}
				break;

				// 1バイト目
				case 1:
				{
					// ......xx|xxxxoooo|oooooooo
					result[result.size() - 1] |= ((data[offset] >> 4) & 0x0f);

					// ........|....xxxx|xxoooooo
					result.push_back(table[(data[offset] << 2) & 0x3c]);
				}
				break;

				// 2バイト目
				case 2:
				{
					// ........|....xxxx|xxoooooo
					result[result.size() - 1] |= ((data[offset] >> 6) & 0x03);

					// ........|........|..xxxxxx
					result.push_back(table[data[offset] & 0x3f]);
				}
				break;

				// ハンドリングチェック用
				default:
					assert(false); // unreached case.
			}
		}

		// パディング文字の埋め込み
		while (result.length() < resultLength)
			result.push_back('=');

		return result;
	}

	/**
	 * デコード
	 * @param base64String base64変換された文字列
	 * @return 復号後のデータ
	 */
	static std::vector<char> decode(const std::string& base64String)
	{
		assert((base64String.size() % 4) == 0);

		typedef std::vector<char>::size_type size_type;

		const size_type resultLength =
			base64String.length() / 4 * 3;
		const size_type sourceLength =
			base64String.length();

		std::vector<char> result;
		result.reserve(resultLength);

		for (size_type offset = 0; offset < sourceLength; ++offset)
		{
			if (base64String[offset] == '=')
				break;

			switch (offset % 4)
			{
				case 0:
				{
					
				}
				break;

				case 1:
				{
				}
				break;

				case 2:
				{
				}
				break;

				case 3:
				{
				}
				break;

				default:
					assert(false); // not reached.
			}
		}
	}

	
};

#endif /* BASE64_HPP_ */
