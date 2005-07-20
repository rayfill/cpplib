#ifndef ENDIAN_HPP_
#define ENDIAN_HPP_
#include <util/SmartPointer.hpp>

/**
 * �ϊ��N���X�̊��N���X
 **/
class Endian
{
protected:
	/**
	 * �ϊ��w���p�֐�
	 * @arg convertValue �ϊ�����ϐ��̃|�C���^
	 * @arg valueSize �ϊ�����ϐ��̐�߂�o�C�g��
	 **/
	void convert(void* convertValue, const size_t valueSize) const
	{
		for (size_t count = 0; count < (valueSize / 2); ++count)
			std::swap(reinterpret_cast<unsigned char*>
					  (convertValue)[count],
					  reinterpret_cast<unsigned char*>
					  (convertValue)[valueSize-1-count]);
	}

public:
	virtual ~Endian() throw()
	{}

	/**
	 * �����̃G���f�B�A���^����BigEndian�ւ̕ϊ�
	 * @arg value �ϊ�����ϐ��ւ̃|�C���^
	 * @arg size �ϊ�����ϐ��̐�߂�o�C�g��
	 **/
	virtual void toBig(void* value, const size_t size) const = 0;

	/**
	 * �����̃G���f�B�A���^����LittleEndian�ւ̕ϊ�
	 * @arg value �ϊ�����ϐ��ւ̃|�C���^
	 * @arg size �ϊ�����ϐ��̐�߂�o�C�g��
	 **/
	virtual void toLittle(void* value, const size_t size) const = 0;

	/**
	 * BigEndian���玩���̃G���f�B�A���^�ւ̕ϊ�
	 * @arg value �ϊ�����ϐ��ւ̃|�C���^
	 * @arg size �ϊ�����ϐ��̐�߂�o�C�g��
	 **/
	virtual void fromBig(void* value, const size_t size) const = 0;

	/**
	 * BigEndian���玩���̃G���f�B�A���^�ւ̕ϊ�
	 * @arg value �ϊ�����ϐ��ւ̃|�C���^
	 * @arg size �ϊ�����ϐ��̐�߂�o�C�g��
	 **/
	virtual void fromLittle(void* value, const size_t size) const = 0;
};

/**
 * ���ϊ��G���f�B�A���R���o�[�^
 **/
class NoconvertEndian : public Endian
{
	virtual void toBig(void*, const size_t) const 
	{}
	virtual void toLittle(void*, const size_t) const
	{}
	virtual void fromBig(void*, const size_t) const
	{}
	virtual void fromLittle(void*, const size_t) const
	{}
};

/**
 * �r�b�O�G���f�B�A����\���ϊ��N���X
 **/
class BigEndian : public Endian
{
	virtual void toBig(void* /*convertValue*/, const size_t /*valueSize*/) const
	{
		// corresponding endian. no action.
	}
	virtual void toLittle(void* convertValue, const size_t valueSize) const
	{
		// big endian to little endian conversion.
		convert(convertValue, valueSize);
	}

	virtual void fromBig(void*, const size_t) const
	{
		// corresponding endian. no action.
	}
	
	virtual void fromLittle(void* convertValue, const size_t valueSize) const
	{
		// little endian to big endian convertsion.
		convert(convertValue, valueSize);
	}
};

/**
 * ���g���G���f�B�A����\���ϊ��N���X
 **/
class LittleEndian : public Endian
{
	virtual void toBig(void* convertValue, const size_t valueSize) const
	{
		// little endian to big endian conversion.
		convert(convertValue, valueSize);
	}
	virtual void toLittle(void*, const size_t) const
	{
		// corresponding endian. no action.
	}

	virtual void fromBig(void* convertValue, const size_t valueSize) const
	{
		// little endian to big endian convertsion.
		convert(convertValue, valueSize);
	}
	
	virtual void fromLittle(void*, const size_t) const
	{
		// corresponding endian. no action.
	}
};

/**
 * �ϊ��T�|�[�g�N���X�B�}�V���̃G���f�B�A����F�����A�����ŃX�^�u�ϊ�
 * �𐶐��A���ق��z������
 **/
class EndianConverter
{
	friend class EndianTest;

protected:
	/**
	 * �G���f�B�A���ϊ��X�^�u
	 */
	SmartPointer<const Endian> endian;

	/**
	 * ���g���G���f�B�A������
	 * @return �}�V�������g���G���f�B�A���Ȃ�true
	 * @exception std::logic_error �G���f�B�A��������ł��Ȃ������ꍇ
	 */
	static bool isLittleEndian() throw (std::logic_error)
	{
		const int endianTest = 1;
		if (*reinterpret_cast<const unsigned char*>(&endianTest) == 1)
			return true;
		else if (reinterpret_cast<const unsigned char*>
				 (&endianTest)[sizeof(const int)-1] == 1)
			return false;
		else
			// unknown endian.
			throw std::logic_error("unknown endian cpu machine.");
	}

	/**
	 * �r�b�O�G���f�B�A������
	 * @return �}�V�����r�b�O�G���f�B�A���Ȃ�true
	 * @exception std::logic_error �G���f�B�A��������ł��Ȃ������ꍇ
	 */
	static bool isBigEndian() throw (std::logic_error)
	{
		return !isLittleEndian();
	}

	/**
	 * �G���f�B�A���𔻒肵�A�K�؂ȕϊ��X�^�u�𐶐�����B
	 **/
	static Endian* machineAdaptEndianFactory()
	{
		if (isLittleEndian())
			return new LittleEndian();
		else
			return new BigEndian();
	}

public:
	/**
	 * �R���X�g���N�^
	 */
	EndianConverter():
		endian(machineAdaptEndianFactory())
	{}

	/**
	 * �G���f�B�A���w��R���X�g���N�^
	 */
	EndianConverter(const Endian* endian_):
		endian(endian_)
	{}

	/**
	 * ������Z�q
	 */
	EndianConverter& operator=(const EndianConverter& source)
	{
		if (this != &source)
			this->endian = source.endian;
		return *this;
	}

	/**
	 * �R�s�[�R���X�g���N�^
	 */
	EndianConverter(const EndianConverter& source):
		endian(source.endian)
	{
	}

	virtual ~EndianConverter() throw()
	{}

	/**
	 * ���g���G���f�B�A���֕ϊ�
	 */
	char toLittle(char value) const
	{
		endian->toLittle(&value, sizeof(value));
		return value;
	}

	unsigned char toLittle(unsigned char value) const
	{
		endian->toLittle(&value, sizeof(value));
		return value;
	}

	short toLittle(short value) const
	{
		endian->toLittle(&value, sizeof(value));
		return value;
	}

	unsigned short toLittle(unsigned short value) const
	{
		endian->toLittle(&value, sizeof(value));
		return value;
	}

	wchar_t toLittle(wchar_t value) const
	{
		endian->toLittle(&value, sizeof(value));
		return value;
	}

	int toLittle(int value) const
	{
		endian->toLittle(&value, sizeof(value));
		return value;
	}

	unsigned int toLittle(unsigned int value) const
	{
		endian->toLittle(&value, sizeof(value));
		return value;
	}

	long toLittle(long value) const
	{
		endian->toLittle(&value, sizeof(value));
		return value;
	}

	unsigned long toLittle(unsigned long value) const
	{
		endian->toLittle(&value, sizeof(value));
		return value;
	}

	long long toLittle(long long value) const
	{
		endian->toLittle(&value, sizeof(value));
		return value;
	}

	unsigned long long toLittle(unsigned long long value) const
	{
		endian->toLittle(&value, sizeof(value));
		return value;
	}

	// to big convert.
	char toBig(char value) const
	{
		endian->toBig(&value, sizeof(value));
		return value;
	}

	unsigned char toBig(unsigned char value) const
	{
		endian->toBig(&value, sizeof(value));
		return value;
	}

	short toBig(short value) const
	{
		endian->toBig(&value, sizeof(value));
		return value;
	}

	unsigned short toBig(unsigned short value) const
	{
		endian->toBig(&value, sizeof(value));
		return value;
	}

	wchar_t toBig(wchar_t value) const
	{
		endian->toBig(&value, sizeof(value));
		return value;
	}

	int toBig(int value) const
	{
		endian->toBig(&value, sizeof(value));
		return value;
	}

	unsigned int toBig(unsigned int value) const
	{
		endian->toBig(&value, sizeof(value));
		return value;
	}

	long toBig(long value) const
	{
		endian->toBig(&value, sizeof(value));
		return value;
	}

	unsigned long toBig(unsigned long value) const
	{
		endian->toBig(&value, sizeof(value));
		return value;
	}

	long long toBig(long long value) const
	{
		endian->toBig(&value, sizeof(value));
		return value;
	}

	unsigned long long toBig(unsigned long long value) const
	{
		endian->toBig(&value, sizeof(value));
		return value;
	}


	char fromLittle(char value) const
	{
		endian->fromLittle(&value, sizeof(value));
		return value;
	}

	unsigned char fromLittle(unsigned char value) const
	{
		endian->fromLittle(&value, sizeof(value));
		return value;
	}

	short fromLittle(short value) const
	{
		endian->fromLittle(&value, sizeof(value));
		return value;
	}

	unsigned short fromLittle(unsigned short value) const
	{
		endian->fromLittle(&value, sizeof(value));
		return value;
	}

	wchar_t fromLittle(wchar_t value) const
	{
		endian->fromLittle(&value, sizeof(value));
		return value;
	}

	int fromLittle(int value) const
	{
		endian->fromLittle(&value, sizeof(value));
		return value;
	}

	unsigned int fromLittle(unsigned int value) const
	{
		endian->fromLittle(&value, sizeof(value));
		return value;
	}

	long fromLittle(long value) const
	{
		endian->fromLittle(&value, sizeof(value));
		return value;
	}

	unsigned long fromLittle(unsigned long value) const
	{
		endian->fromLittle(&value, sizeof(value));
		return value;
	}

	long long fromLittle(long long value) const
	{
		endian->fromLittle(&value, sizeof(value));
		return value;
	}

	unsigned long long fromLittle(unsigned long long value) const
	{
		endian->fromLittle(&value, sizeof(value));
		return value;
	}

	// from big convert.
	char fromBig(char value) const
	{
		endian->fromBig(&value, sizeof(value));
		return value;
	}

	unsigned char fromBig(unsigned char value) const
	{
		endian->fromBig(&value, sizeof(value));
		return value;
	}

	short fromBig(short value) const
	{
		endian->fromBig(&value, sizeof(value));
		return value;
	}

	unsigned short fromBig(unsigned short value) const
	{
		endian->fromBig(&value, sizeof(value));
		return value;
	}

	wchar_t fromBig(wchar_t value) const
	{
		endian->fromBig(&value, sizeof(value));
		return value;
	}

	int fromBig(int value) const
	{
		endian->fromBig(&value, sizeof(value));
		return value;
	}

	unsigned int fromBig(unsigned int value) const
	{
		endian->fromBig(&value, sizeof(value));
		return value;
	}

	long fromBig(long value) const
	{
		endian->fromBig(&value, sizeof(value));
		return value;
	}

	unsigned long fromBig(unsigned long value) const
	{
		endian->fromBig(&value, sizeof(value));
		return value;
	}

	long long fromBig(long long value) const
	{
		endian->fromBig(&value, sizeof(value));
		return value;
	}

	unsigned long long fromBig(unsigned long long value) const
	{
		endian->fromBig(&value, sizeof(value));
		return value;
	}

};

class UserDefinedEndianConverter : public EndianConverter
{
private:
	bool isSourceLittle;

public:
	UserDefinedEndianConverter(const UserDefinedEndianConverter& source)
		: EndianConverter(source),
		  isSourceLittle(source.isSourceLittle)
	{
	}

	UserDefinedEndianConverter&
	operator=(const UserDefinedEndianConverter& source)
	{
		if (this == &source)
			return *this;

		EndianConverter::operator=(source);
		isSourceLittle = source.isSourceLittle;

		return *this;
	}

	UserDefinedEndianConverter(const bool isSourceLittle_)
		: EndianConverter(),
		  isSourceLittle(isSourceLittle_)
	{
	}

	// to big convert.
	char to(char value) const 
	{
		if (isSourceLittle)
			endian->toLittle(&value, sizeof(value));
		else
			endian->toBig(&value, sizeof(value));
		return value;
	}

	unsigned char to(unsigned char value) const
	{
		if (isSourceLittle)
			endian->toLittle(&value, sizeof(value));
		else
			endian->toBig(&value, sizeof(value));
		return value;
	}

	short to(short value) const
	{
		if (isSourceLittle)
			endian->toLittle(&value, sizeof(value));
		else
			endian->toBig(&value, sizeof(value));
		return value;
	}

	unsigned short to(unsigned short value) const
	{
		if (isSourceLittle)
			endian->toLittle(&value, sizeof(value));
		else
			endian->toBig(&value, sizeof(value));
		return value;
	}

	wchar_t to(wchar_t value) const
	{
		if (isSourceLittle)
			endian->toLittle(&value, sizeof(value));
		else
			endian->toBig(&value, sizeof(value));
		return value;
	}

	int to(int value) const
	{
		if (isSourceLittle)
			endian->toLittle(&value, sizeof(value));
		else
			endian->toBig(&value, sizeof(value));
		return value;
	}

	unsigned int to(unsigned int value) const
	{
		if (isSourceLittle)
			endian->toLittle(&value, sizeof(value));
		else
			endian->toBig(&value, sizeof(value));
		return value;
	}

	long to(long value) const
	{
		if (isSourceLittle)
			endian->toLittle(&value, sizeof(value));
		else
			endian->toBig(&value, sizeof(value));
		return value;
	}

	unsigned long to(unsigned long value) const
	{
		if (isSourceLittle)
			endian->toLittle(&value, sizeof(value));
		else
			endian->toBig(&value, sizeof(value));
		return value;
	}

	long long to(long long value) const
	{
		if (isSourceLittle)
			endian->toLittle(&value, sizeof(value));
		else
			endian->toBig(&value, sizeof(value));
		return value;
	}

	unsigned long long to(unsigned long long value) const
	{
		if (isSourceLittle)
			endian->toLittle(&value, sizeof(value));
		else
			endian->toBig(&value, sizeof(value));
		return value;
	}

	char from(char value) const
	{
		if (isSourceLittle)
			endian->fromLittle(&value, sizeof(value));
		else
			endian->fromBig(&value, sizeof(value));
		return value;
	}

	unsigned char from(unsigned char value) const
	{
		if (isSourceLittle)
			endian->fromLittle(&value, sizeof(value));
		else
			endian->fromBig(&value, sizeof(value));
		return value;
	}

	short from(short value) const
	{
		if (isSourceLittle)
			endian->fromLittle(&value, sizeof(value));
		else
			endian->fromBig(&value, sizeof(value));
		return value;
	}

	unsigned short from(unsigned short value) const
	{
		if (isSourceLittle)
			endian->fromLittle(&value, sizeof(value));
		else
			endian->fromBig(&value, sizeof(value));
		return value;
	}

	wchar_t from(wchar_t value) const
	{
		if (isSourceLittle)
			endian->fromLittle(&value, sizeof(value));
		else
			endian->fromBig(&value, sizeof(value));
		return value;
	}

	int from(int value) const
	{
		if (isSourceLittle)
			endian->fromLittle(&value, sizeof(value));
		else
			endian->fromBig(&value, sizeof(value));
		return value;
	}

	unsigned int from(unsigned int value) const
	{
		if (isSourceLittle)
			endian->fromLittle(&value, sizeof(value));
		else
			endian->fromBig(&value, sizeof(value));
		return value;
	}

	long from(long value) const
	{
		if (isSourceLittle)
			endian->fromLittle(&value, sizeof(value));
		else
			endian->fromBig(&value, sizeof(value));
		return value;
	}

	unsigned long from(unsigned long value) const
	{
		if (isSourceLittle)
			endian->fromLittle(&value, sizeof(value));
		else
			endian->fromBig(&value, sizeof(value));
		return value;
	}

	long long from(long long value) const
	{
		if (isSourceLittle)
			endian->fromLittle(&value, sizeof(value));
		else
			endian->fromBig(&value, sizeof(value));
		return value;
	}

	unsigned long long from(unsigned long long value) const
	{
		if (isSourceLittle)
			endian->fromLittle(&value, sizeof(value));
		else
			endian->fromBig(&value, sizeof(value));
		return value;
	}
};




#endif /* ENDIAN_HPP_ */
