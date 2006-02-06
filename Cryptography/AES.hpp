#ifndef AES_HPP_
#define AES_HPP_

#include <limits>
#include <stdexcept>

/**
 * ラインダール暗号
 * @TODO まだ実装が完全ではありません
 */
template <size_t cipherBitsSize = 128, size_t keyBitsSize = 128>
class Rijndael
{
	friend class RijndaelTest;

private:
	const int numberOfKey;
	const int numberOfBlock;

	/**
	 * ラインダール代数
	 * ラインダール暗号のベースになります
	 */
	class RijndaelNumber
	{
		friend class RijndaelTest;

	private:
		unsigned char number;

		unsigned char modulus(unsigned short source) const
		{
			if (source > 0xff)
				return
					static_cast<unsigned char>((source ^ 0x1B) & 0xff);
			else
				return
					static_cast<unsigned char>(source);
		}

		unsigned char xtime(const unsigned char value) const
		{
			return modulus(static_cast<unsigned short>(value) << 1);
		}

	public:
		RijndaelNumber(const unsigned char value)
			: number(value)
		{}

		RijndaelNumber(const RijndaelNumber& value)
			: number(value.number)
		{}

		~RijndaelNumber()
		{}

		RijndaelNumber invert() const
		{
			if (this->number == 0)
				return RijndaelNumber(static_cast<unsigned char>(0));

			for (int i = 1; i < 256; ++i)
			{
				if (RijndaelNumber(*this).
					operator*=(RijndaelNumber(
								   static_cast<unsigned char>(i))).
					number == 1)
					return RijndaelNumber(static_cast<unsigned char>(i));
			}

			throw std::logic_error("逆元が見つかりません。ロジックエラー");
		}

		RijndaelNumber& operator+=(const RijndaelNumber& rhs)
		{
			this->number ^= rhs.number;
			return *this;
		}

		RijndaelNumber operator+(const RijndaelNumber& rhs)
		{
			return RijndaelNumber(*this).operator+=(rhs);
		}

		RijndaelNumber& operator<<=(const int shiftCount)
		{
			if (shiftCount >= 256)
				throw std::logic_error("shift count over than 8.");

			for (int count = 0; count < shiftCount; ++count)
				this->number = xtime(this->number);

			return *this;
		}

		RijndaelNumber& RotateLeft()
		{
			number = static_cast<unsigned char>(
				(((number << 1) & 0xfe) | ((number >> 7) & 0x01)) & 0xff);
			return *this;
		}

		RijndaelNumber& RotateRight()
		{
			number = static_cast<unsigned char>(
				(((number >> 1) & 0x7f) | ((number << 7) & 0x80)) & 0xff);
			return *this;
		}

		RijndaelNumber& operator*=(const RijndaelNumber& rhs)
		{
			unsigned char& result = this->number;
			unsigned char temp = this->number;

			result = 0;
			for (unsigned int offset = 0;
				 offset < (sizeof(unsigned char) * 8);
				 ++offset)
			{
				if (rhs.number & (1 << offset))
				{
					result ^= temp;
				}
				temp = xtime(temp);
			}

			return *this;
		}

		RijndaelNumber operator*(const RijndaelNumber& rhs) const
		{
			return RijndaelNumber(*this).operator*=(rhs);
		}

		RijndaelNumber& operator=(const RijndaelNumber& rhs)
		{
			this->number = rhs.number;
			return *this;
		}

		bool operator==(const RijndaelNumber& target) const
		{
			return this->number == target.number;
		}

		bool operator!=(const RijndaelNumber& target) const
		{
			return this->number != target.number;
		}

		bool operator<(const RijndaelNumber& target) const
		{
			return this->number < target.number;
		}

		bool operator<=(const RijndaelNumber& target) const
		{
			return !(this->number > target.number);
		}

		bool operator>(const RijndaelNumber& target) const
		{
			return this->number > target.number;
		}

		bool operator>=(const RijndaelNumber& target) const
		{
			return !(this->number < target.number);
		}

	};

	/**
	 * 暗号処理のラウンド数取得
	 */
	const int getNumberOfRounds() const
	{
		if (numberOfBlock > numberOfKey)
			return numberOfBlock + 6;
		else
			return numberOfKey + 6;
	}

	/**
	 * SBox処理を施したラインダール数の取得
	 * @params value 処理するラインダール数
	 * @return 処理されたラインダール数
	 */
	const RijndaelNumber
	getSBoxValue(const RijndaelNumber& value) const
	{
		const Rijndael<>::RijndaelNumber constant(0x63);
		Rijndael<>::RijndaelNumber rn(value);
		rn = rn.invert();
		Rijndael<>::RijndaelNumber result(rn);
		rn.RotateLeft();
		result += rn;
		rn.RotateLeft();
		result += rn;
		rn.RotateLeft();
		result += rn;
		rn.RotateLeft();
		result += rn;
		result += constant;

		return result;
	}

	/**
	 * SBox処理されたラインダール数から元のラインダール数を求める
	 * @TODO 未実装
	 */
	const RijndaelNumber
	getInvertSBoxValue(const RijndaelNumber& value) const
	{
	}

public:
	/**
	 * コンストラクタ
	 */
	Rijndael() throw()
		: numberOfKey(keyBitsSize / 8),
		  numberOfBlock(cipherBitsSize / 8)
	{
	}

	/**
	 * デストラクタ
	 */
	virtual ~Rijndael()
	{
	}

	/**
	 * 暗号化処理
	 * @params plaintext 平文バイト列
	 * @params key 暗号鍵
	 * @return 暗号処理が施されたバイト列
	 */
	std::vector<unsigned char>
	encrypt(const std::vector<unsigned char>& plaintext,
			const std::vector<unsigned char>& key)
	{
		std::vector<RijndaelNumber> state(numberOfBlock*4);
		std::vector<RijndaelNumber> expandedKey(4);

		if (plaintext.size() != numberOfBlock * 4 ||
			key.size() != numberOfKey * 4)
			throw std::runtime_error(
				"plain text or password length is invalid.");

		// copy plaintext to state
		for (size_t offset = 0; offset < plaintext.size(); ++offset)
			state[offset % 4][offset / 4] = RijndaelNumber(plaintext[offset]);

		// key expansion.
	}

private:
	RijndaelNumber ByteSub(const RijndaelNumber& rhs) const
	{
		reutrn (rhs * RijndaelNumber(0x8f)) + RijndaelNumber(0x63);
	}


	public:
};

#include <Cryptography/AESConstant.hpp>

/**
 * AES(Advanced Encrypt Standard)実装
 * リトルエンディアンCPU上しか考慮してません
 */
template
<size_t keyLength = 128,
size_t numberOfBlock = 4>
class AES
{
	friend class AESTest;

public:
	typedef unsigned char byte;
	typedef unsigned int word;

private:

	const int numberOfKey;

	/// 鍵スケジューリング処理後の鍵
	std::vector<word> scheduledKey;

	byte modulus(const word source) const
	{
		if (source > 0xff)
			return
				static_cast<unsigned char>((source ^ 0x1B) & 0xff);
		else
			return
				static_cast<unsigned char>(source);
	}

	byte xtime(const byte value) const
	{
		return modulus(static_cast<const word>(value) << 1);
	}

	word rotateByte(const word value)
	{
		return make4BytesToWord(
			static_cast<byte>(value & 0xff),
			static_cast<byte>((value >> 24) & 0xff),
			static_cast<byte>((value >> 16) & 0xff),
			static_cast<byte>((value >> 8) & 0xff));
	}

	const int getNumberOfRounds() const
	{
		return numberOfKey + 6;
	}

	void makeWordTo4Bytes(const word value, byte* bytes)
	{
		bytes[0] = static_cast<byte>((value) & 0xff);
		bytes[1] = static_cast<byte>((value >>  8) & 0xff);
		bytes[2] = static_cast<byte>((value >> 16) & 0xff);
		bytes[3] = static_cast<byte>((value >> 24) & 0xff);
	}

	word make4BytesToWord(const byte a,
						  const byte b,
						  const byte c,
						  const byte d) const
	{
		return 
			static_cast<word>(d << 24) |
			static_cast<word>(c << 16) |
			static_cast<word>(b << 8) |
			static_cast<word>(a);
	}

	void invertShiftRow(std::vector<byte>& state)
	{
		byte exchangeTemp;
		// 0, 4, 8, c 無変換

		// 1, 5, 9, d 1シフト
		// d, 1, 5, 9 結果
		exchangeTemp = state[1];
		state[1] = state[13];
		state[13] = state[9];
		state[9] = state[5];
		state[5] = exchangeTemp;
		
		// 2, 6, a, e 2シフト
		// a, e, 2, 6 結果
		exchangeTemp = state[2];
		state[2] = state[10];
		state[10] = exchangeTemp;
		exchangeTemp = state[14];
		state[14] = state[6];
		state[6] = exchangeTemp;

		// 3, 7, b, f 3シフト
		// 7, b, f, 3 結果
		exchangeTemp = state[3];
		state[3] = state[7];
		state[7] = state[11];
		state[11] = state[15];
		state[15] = exchangeTemp;
	}

	void shiftRow(std::vector<byte>& state)
	{
		byte exchangeTemp;
		// 0, 4, 8, c 無変換

		// 1, 5, 9, d 1シフト
		// 5, 9, d, 1 結果
		exchangeTemp = state[1];
		state[1] = state[5];
		state[5] = state[9];
		state[9] = state[13];
		state[13] = exchangeTemp;
		
		// 2, 6, a, e 2シフト
		// a, e, 2, 6 結果
		exchangeTemp = state[2];
		state[2] = state[10];
		state[10] = exchangeTemp;
		exchangeTemp = state[14];
		state[14] = state[6];
		state[6] = exchangeTemp;

		// 3, 7, b, f 3シフト
		// f, 3, 7, b 結果
		exchangeTemp = state[3];
		state[3] = state[15];
		state[15] = state[11];
		state[11] = state[7];
		state[7] = exchangeTemp;
	}

	byte multiply(const byte lhs, const byte rhs) const
	{
		return AESConstant::productTable[lhs*256+rhs];
	}

	void mixColumn(std::vector<byte>& state) const
	{
		const unsigned int roundSize = numberOfBlock;
		std::vector<byte> newState(state.size());
		state.swap(newState);

		for (unsigned int i = 0; i < 4; ++i)
		{
			for (unsigned int j = 0; j < 4; ++j)
			{
				state[i*numberOfBlock+j] = 
					multiply(0x02,
							 newState[i*numberOfBlock+j]) ^
					multiply(0x03,
							 newState[i*numberOfBlock+((j+1)%roundSize)]) ^
					multiply(0x01,
							 newState[i*numberOfBlock+((j+2)%roundSize)]) ^
					multiply(0x01,
							 newState[i*numberOfBlock+((j+3)%roundSize)]);
			}
		}
	}

	void invertMixColumn(std::vector<byte>& state)
	{
		const unsigned int roundSize = numberOfBlock;
		std::vector<byte> newState(state.size());
		state.swap(newState);

		for (unsigned int i = 0; i < 4; ++i)
		{
			for (unsigned int j = 0; j < 4; ++j)
			{
				state[i*numberOfBlock+j] = 
					multiply(0x0e,
							 newState[i*numberOfBlock+j]) ^
					multiply(0x0b,
							 newState[i*numberOfBlock+((j+1)%roundSize)]) ^
					multiply(0x0d,
							 newState[i*numberOfBlock+((j+2)%roundSize)]) ^
					multiply(0x09,
							 newState[i*numberOfBlock+((j+3)%roundSize)]);
			}
		}
	}

	std::vector<word> getScheduledKey(const std::vector<byte>& key)
	{
		std::vector<word> 
			scheduledKey(numberOfBlock*(getNumberOfRounds()+1));
		
		for (int i =  0; i < numberOfKey; ++i)
			scheduledKey[i] =
				make4BytesToWord(key[4*i+0], key[4*i+1],
								 key[4*i+2], key[4*i+3]); 

		byte roundCoefficient = 1;
		for (unsigned int i = numberOfKey;
			 i < scheduledKey.size();
			 ++i)
		{
			word temp = scheduledKey[i - 1];
			if ((i % numberOfKey) == 0)
			{
				temp =
					wordSub(rotateByte(temp)) ^ 
					make4BytesToWord(roundCoefficient, 0, 0, 0);
				roundCoefficient = xtime(roundCoefficient);
			}
			else if (numberOfKey > 6 &&
					 (i % numberOfKey) == 4)
				temp = wordSub(temp);

			scheduledKey[i] =
				scheduledKey[i - numberOfKey] ^ temp;
		}

		return scheduledKey;
	}

	void addRoundKey(std::vector<byte>& state,
					 std::vector<word>& scheduledKey,
					 const int round)
	{
		byte applyRoundKey[numberOfBlock*4];
		makeWordTo4Bytes(scheduledKey[round*4], applyRoundKey);
		makeWordTo4Bytes(scheduledKey[round*4+1], applyRoundKey+4);
		makeWordTo4Bytes(scheduledKey[round*4+2], applyRoundKey+8);
		makeWordTo4Bytes(scheduledKey[round*4+3], applyRoundKey+12);

		for (unsigned int i = 0; i < state.size(); ++i)
			state[i] = state[i] ^ applyRoundKey[i];
	}

	word wordSub(const word val) const
	{
		return make4BytesToWord(
			byteSub(static_cast<byte>((val >> 24) & 0xff)),
			byteSub(static_cast<byte>((val >> 16) & 0xff)),
			byteSub(static_cast<byte>((val >>  8) & 0xff)),
			byteSub(static_cast<byte>((val      ) & 0xff)));
	}

	word invertWordSub(const word val) const
	{
		return make4BytesToWord(
			invertByteSub(static_cast<byte>((val >> 24) & 0xff)),
			invertByteSub(static_cast<byte>((val >> 16) & 0xff)),
			invertByteSub(static_cast<byte>((val >>  8) & 0xff)),
			invertByteSub(static_cast<byte>((val      ) & 0xff)));
	}

	void byteSub(std::vector<byte>& state)
	{
		for (std::vector<byte>::iterator itor = state.begin();
			 itor != state.end();
			 ++itor)
			*itor = byteSub(*itor);
	}

	void invertByteSub(std::vector<byte>& state)
	{
		for (std::vector<byte>::iterator itor = state.begin();
			 itor != state.end();
			 ++itor)
			*itor = invertByteSub(*itor);
	}

	byte byteSub(const byte val) const
	{
		return AESConstant::SBox[val];
	}

	byte invertByteSub(const byte val) const
	{
		return AESConstant::invertSBox[val];
	}

public:
	AES() throw():
		numberOfKey(keyLength/32),
		scheduledKey()
	{}

	~AES()
	{}

	/**
	 * 暗号処理ブロックサイズの取得
	 * @return 処理ブロックサイズ
	 */
	size_t getBlockSize() const
	{
		return numberOfBlock * 4;
	}

	/**
	 * 復号処理
	 * @params data 暗号文
	 * @return 復号された文
	 */
	std::vector<byte>
	decrypt(std::vector<byte>& data)
	{
		if (data.size() != 16)
			throw std::runtime_error(
				"too not match length decrypt data.\n"
				"AES's data length is 4 word(single word is 4 octet).\n");

		// data to state.
		std::vector<byte> state(data);

		// last round.
		addRoundKey(state, scheduledKey, getNumberOfRounds());
		invertByteSub(state);
		invertShiftRow(state);
		
		// second to last - 1 round.
		for (int i = getNumberOfRounds()-1; i > 0; --i)
		{
			addRoundKey(state, scheduledKey, i);
			invertMixColumn(state);
			invertByteSub(state);
			invertShiftRow(state);
		}

		// first round.
		addRoundKey(state, scheduledKey, 0);

		// return decrypted data.
		return state;
	}

	/**
	 * 暗復号に使用する鍵の登録
	 * @params key 暗復号に使用する鍵
	 */
	void setKey(const std::vector<byte>& key)
	{
		if (static_cast<const int>(key.size() / 4) !=
			numberOfKey)
			throw std::runtime_error(
				"鍵データ長が正しくありません");

		// 鍵スケジュール処理
		scheduledKey = getScheduledKey(key);
	}

	/**
	 * 暗号化処理
	 * @params data 平文
	 * @return 暗号文
	 */
	std::vector<byte>
	encrypt(std::vector<byte>& data)
	{
		if (data.size() != 16)
			throw std::runtime_error(
				"引数 dataの長さが正しくありません\n"
				"AES の平文データ長は4ワード(1ワードは4オクテット)です.\n");

		// データから状態に変換
		std::vector<byte> state(data);

		// 第一ラウンド処理
		addRoundKey(state, scheduledKey, 0);
		
		// 第2から最終ひとつ手前までのラウンド処理
		for (int i = 1; i < getNumberOfRounds(); ++i)
		{
			byteSub(state);
			shiftRow(state);
			mixColumn(state);
			addRoundKey(state, scheduledKey, i);
		}

		// 最終ラウンド
		byteSub(state);
		shiftRow(state);
		addRoundKey(state, scheduledKey, getNumberOfRounds());

		return state;
	}
};

#endif /* AES_HPP_ */
