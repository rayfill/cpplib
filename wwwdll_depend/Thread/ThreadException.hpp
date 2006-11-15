#ifndef THREADEXCEPTION_HPP_
#define THREADEXCEPTION_HPP_

#include <exception>
#include <stdexcept>
#include <string>

/**
 * Thread �N���X�C���X�^���X���s���ɔ��������O
 */
class ThreadException : public std::runtime_error
{
public:
	/**
	 * �f�t�H���g�R���X�g���N�^
	 */ 
	ThreadException() throw()
		: std::runtime_error("thread exception.")
	{}
	
	/**
	 * �f�X�g���N�^
	 */
	virtual ~ThreadException() throw() {}
	
	/**
	 * �R�s�[�R���X�g���N�^
	 * @param te �R�s�[���I�u�W�F�N�g
	 */
	ThreadException(const ThreadException& te) throw()
		: std::runtime_error(te.what())
	{}
	
	/**
	 * �R���X�g���N�^
	 * @param reason ��O�������R������������
	 */
	ThreadException(const char* reason) throw()
		: std::runtime_error(reason)
	{}

	virtual ThreadException* clone() const throw(std::bad_alloc)
	{
		return new ThreadException(*this);
	}
};

/**
 * Thread�̑���Ɋ��荞��ŋN�����O
 */
class InterruptedException : public ThreadException
{
public:
	InterruptedException()
			: ThreadException("InteruptedException raised.")
	{}

	InterruptedException(const char* reason)
			: ThreadException(reason)
	{}

	InterruptedException(const InterruptedException& ie) throw()
			: ThreadException(ie.what())
	{}

	virtual ~InterruptedException() throw()
	{}

	virtual InterruptedException* clone() const throw(std::bad_alloc)
	{
		return new InterruptedException(*this);
	}
};

/**
 * ���삪�^�C���A�E�g�������Ƃ��Ӗ������O
 */
class TimeoutException : public std::runtime_error
{
public:
	/**
	 * �f�t�H���g�R���X�g���N�^
	 */
	TimeoutException() throw()
		: std::runtime_error("timeout exception.")
	{}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~TimeoutException() throw() {}

	/**
	 * �R���X�g���N�^
	 * @param reason ��O�������R������������
	 */
	TimeoutException(const char* reason) throw()
		: std::runtime_error(reason)
	{}
};

#endif /* THREADEXCEPTION_HPP_ */