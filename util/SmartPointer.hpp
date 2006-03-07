#ifndef SMARTPOINTER_HPP_
#define SMARTPOINTER_HPP_

#include <cassert>
#include <stdexcept>

/**
 * SmartPointer�p�|�C���^�폜������|���V�[
 * @param Container �R���e�i�^
 */
template <typename Container>
class DefaultRemover
{
public:
	/**
	 * �폜�n���h��
	 * @param pointer �폜�I�u�W�F�N�g�̃|�C���^
	 */
	static void remove(Container* pointer)
	{
		delete pointer;
	}
};

/**
 * �z��폜�|���V�[
 * @param Container �폜����z��̗v�f�̌^
 */
template <typename Container>
class ArrayRemover
{
public:
	/**
	 * �폜�n���h��
	 * @param pointer �폜�I�u�W�F�N�g�z��̐擪�|�C���^
	 */
	static void remove(Container* pointer)
	{
		delete[] pointer;
	}
};

/**
 * �Q�ƃJ�E���^
 */
struct ReferenceCounter
{
private:
	friend class SmartPointerTest;

	/**
	 * �Q�ƃJ�E���g�ϐ�
	 */
	int reference;

	/**
	 * �Q�ƃJ�E���g�擾
	 * @return ���݂̎Q�ƃJ�E���g
	 */
	int getReferenceCount() const throw()
	{
		return reference;
	}

	/// �R�s�[�h�~�p
	ReferenceCounter(const ReferenceCounter&);
	ReferenceCounter operator=(const ReferenceCounter&);
public:
	/**
	 * �R���X�g���N�^
	 */
	ReferenceCounter()
		: reference(0)
	{
		assert(reference == 0);
	}

	/**
	 * �Q�ƃJ�E���g�̉��Z
	 * @return ������̎Q�ƃJ�E���g��
	 */
	int addReference()
	{
		return ++reference;
	}
		
	/**
	 * �Q�ƃJ�E���g�̌��Z
	 * @return ���Z��̎Q�ƃJ�E���g��
	 */
	int release()
	{
		assert(reference > 0);
		return --reference;
	}
};

/**
 * �X�}�[�g�|�C���^�N���X
 * @param Container �R���e�i������N���X
 * @param RemovePolicy �폜���̃n���h�����O�|���V�[
 * @todo MultiThread���̂��߂̃��b�N�|���V�[�Ƃ����K�v����
 */
template <typename Container,
	class RemovePolicy = DefaultRemover<Container> >
class SmartPointer
{
public:
	typedef Container* Pointer;

private:
	friend class SmartPointerTest;

	/**
	 * ���L���̔j��
	 */
	void release()
	{
		if (refCount->release() == 0)
		{
			RemovePolicy remover;
			remover.remove(pointer);
			delete refCount;
		}
	}

	/**
	 * �Ǘ��Ώۃ|�C���^
	 */
	Pointer pointer;

	/**
	 * �Q�ƃJ�E���g�z���_
	 */
	ReferenceCounter* refCount;

public:
	/**
	 * �R���X�g���N�^
	 * @param pointer_ �Ǘ��ΏۂƂȂ�|�C���^
	 */
	SmartPointer(Pointer pointer_)
		throw(std::bad_alloc, std::invalid_argument) :
		pointer(pointer_),
		refCount(new ReferenceCounter())
	{
		if (pointer_ == NULL)
		{
			delete refCount;
			throw std::invalid_argument("pointer is null.");
		}

		refCount->addReference();
	}

	/**
	 * �R�s�[�R���X�g���N�^
	 * @param src �R�s�[���I�u�W�F�N�g
	 */
	SmartPointer(const SmartPointer& src)
		throw() :
		pointer(src.pointer),
		refCount(src.refCount)
	{
		refCount->addReference();
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~SmartPointer()
	{
		release();
	}

	/**
	 * �|�C���^�̎擾
	 */
	Pointer get() const throw()
	{
		return pointer;
	}

	/**
	 * ��r���Z
	 * @return ���l�ȃ|�C���^�����ꍇ�Atrue
	 */
	bool operator==(const SmartPointer& src) const throw()
	{
		if (this == &src)
			return true;

		return this->pointer == src.pointer;
	}

	/**
	 * ������Z
	 * @param src �R�s�[��
	 * @return �R�s�[��̎��g�ւ̎Q��
	 */
	SmartPointer& operator=(const SmartPointer& src) throw()
	{
		if (this == &src)
			return *this;

		release();

		this->refCount = src.refCount;
		refCount->addReference();
		this->pointer = src.pointer;

		return *this;
	}

	/**
	 * �����o�Ăяo��
	 */
	Pointer operator->() const throw()
	{
		return pointer;
	}
};

/**
 * �X�}�[�g�z��
 * @param Container �v�f�̌^
 * @RemovePolicy �������J���|���V�[
 * �v�f�̃f�X�g���N�^�͗�O�𓊂��Ȃ�����
 */
template <typename Container,
	class RemovePolicy = ArrayRemover<Container> >
class SmartArray
{
	friend class SmartPointerTest;

public:
	typedef Container* Pointer;
	typedef Container& Reference;

private:
	/**
	 * �J������
	 * ���������ۂɊJ�������͎̂Q�Ƃ��Ȃ��Ȃ������̂�
	 */
	void release()
	{
		if (refCount->release() == 0)
		{
			RemovePolicy::remove(pointer);
			delete refCount;
		}
	}

	/**
	 * �v�f�z��ւ̃|�C���^
	 */
	Pointer pointer;

	/**
	 * �Q�ƃJ�E���^�ւ̃|�C���^
	 */
	ReferenceCounter* refCount;
	
public:
	/**
	 * �R���X�g���N�^
	 * @param pointer_ �Ǘ��Ώۂ̐��|�C���^
	 */
	SmartArray(Pointer pointer_)
		throw(std::bad_alloc, std::invalid_argument) :
		pointer(pointer_),
		refCount(new ReferenceCounter())
	{
		if (pointer_ == NULL)
		{
			delete refCount;
			throw std::invalid_argument("pointer is null.");
		}

		refCount->addReference();
	}

	/**
	 * �R�s�[�R���X�g���N�^
	 * @param src �R�s�[���X�}�[�g�z��
	 */
	SmartArray(const SmartArray& src)
		throw() :
		pointer(src.pointer),
		refCount(src.refCount)
	{
		refCount->addReference();
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~SmartArray()
	{
		release();
	}

	/**
	 * ���|�C���^�̎擾
	 */
	Pointer get() const throw()
	{
		return pointer;
	}

	/**
	 * �v�f�ւ�index�x�[�X�A�N�Z�X
	 * @param index �v�f�ւ̃I�t�Z�b�g�l(�[���I���W���x�[�X�ł�)
	 */
	Reference operator[](int index)
	{
		return pointer[index];
	}

	/**
	 * ������r���Z�q
	 * @return �ێ����Ă���|�C���^���������̂Ȃ� true
	 */
	bool operator==(const SmartArray& src) const throw()
	{
		if (this == &src)
			return true;

		return this->pointer == src.pointer;
	}

	/**
	 * ������Z�q
	 * @param src �R�s�[��
	 * @return SmartArray �R�s�[��̎Q��
	 */
	SmartArray& operator=(const SmartArray& src) throw()
	{
		if (this == &src)
			return *this;

		release();

		this->refCount = src.refCount;
		refCount->addReference();
		this->pointer = src.pointer;

		return *this;
	}
};

#endif /* SMARTPOINTER_HPP_ */
