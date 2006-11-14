#ifndef AES_HPP_
#define AES_HPP_

#include <limits>
#include <stdexcept>
#include <Cryptography/AESConstant.hpp>

/**
 * AES(Advanced Encrypt Standard)実装
 * @note リトルエンディアンCPU上しか考慮してません
 * @todo AESConstantがでかすぎるので対数表を使った方法への切り替え
 * @param keyLength キーの長さ(bit長)
 * @param numberOfBlock 暗号処理のword長(1wordは4bytes)。って本来AESは固定。
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
			byteSub(static_cast<byte>((val) & 0xff)));
	}

	word invertWordSub(const word val) const
	{
		return make4BytesToWord(
			invertByteSub(static_cast<byte>((val >> 24) & 0xff)),
			invertByteSub(static_cast<byte>((val >> 16) & 0xff)),
			invertByteSub(static_cast<byte>((val >>  8) & 0xff)),
			invertByteSub(static_cast<byte>((val) & 0xff)));
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
	 * @param data 暗号文
	 * @return 復号された文
	 */
	std::vector<byte>
	decrypt(const std::vector<byte>& data)
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
	 * @param key 暗復号に使用する鍵
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
	 * @param data 平文
	 * @return 暗号文
	 */
	std::vector<byte>
	encrypt(const std::vector<byte>& data)
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
