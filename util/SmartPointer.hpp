#ifndef SMARTPOINTER_HPP_
#define SMARTPOINTER_HPP_

#include <cassert>
#include <stdexcept>

/**
 * SmartPointer�p�|�C���^�폜������|���V�[
 */
template <typename Container>
class DefaultRemover
{
public:
	/**
	 * �폜�n���h��
	 * @arg pointer �폜�I�u�W�F�N�g�̃|�C���^
	 */
	static void remove(Container* pointer)
	{
		delete pointer;
	}
};

template <typename Container>
class ArrayRemover
{
public:
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

	int getReferenceCount() const throw()
	{
		return reference;
	}

	/// �R�s�[�h�~�p
	ReferenceCounter(const ReferenceCounter&);
	ReferenceCounter operator=(const ReferenceCounter&);
public:
	ReferenceCounter()
		: reference(0)
	{
		assert(reference == 0);
	}

	int addReference()
	{
		return ++reference;
	}
		
	int release()
	{
		assert(reference > 0);
		return --reference;
	}
};

/**
 * �X�}�[�g�|�C���^�N���X
 * @arg Container �R���e�i������N���X
 * @arg RemovePolicy �폜���̃n���h�����O�|���V�[
 */
template <typename Container,
	class RemovePolicy = DefaultRemover<Container> >
class SmartPointer
{
public:
	typedef Container* Pointer;

private:
	friend class SmartPointerTest;

	void release()
	{
		if (refCount->release() == 0)
		{
			RemovePolicy::remove(pointer);
			delete refCount;
		}
	}

	Pointer pointer;
	ReferenceCounter* refCount;

public:
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

	SmartPointer(const SmartPointer& src)
		throw() :
		pointer(src.pointer),
		refCount(src.refCount)
	{
		refCount->addReference();
	}

	virtual ~SmartPointer()
	{
		release();
	}

	Pointer get() const throw()
	{
		return pointer;
	}

	bool operator==(const SmartPointer& src) const throw()
	{
		if (this == &src)
			return true;

		return this->pointer == src.pointer;
	}

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

	Pointer operator->() const throw()
	{
		return pointer;
	}
};

template <typename Container,
	class RemovePolicy = ArrayRemover<Container> >
class SmartArray
{
	friend class SmartPointerTest;

public:
	typedef Container* Pointer;
	typedef Container& Reference;

private:
	void release()
	{
		if (refCount->release() == 0)
		{
			RemovePolicy::remove(pointer);
			delete refCount;
		}
	}

	Pointer pointer;
	ReferenceCounter* refCount;
	
public:
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

	SmartArray(const SmartArray& src)
		throw() :
		pointer(src.pointer),
		refCount(src.refCount)
	{
		refCount->addReference();
	}

	virtual ~SmartArray()
	{
		release();
	}

	Pointer get() const throw()
	{
		return pointer;
	}

	Reference operator[](int index)
	{
		return pointer[index];
	}

	bool operator==(const SmartArray& src) const throw()
	{
		if (this == &src)
			return true;

		return this->pointer == src.pointer;
	}

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
