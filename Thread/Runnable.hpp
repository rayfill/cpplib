#ifndef RUNNABLE_HPP_
#define RUNNABLE_HPP_

#include <Thread/ThreadException.hpp>

/**
 * ���s�G���g���|�C���g�C���^�t�F�[�X
 */
class Runnable
{
public:
	/**
	 * ���s�O�̑O����
	 */
	virtual void prepare() throw() = 0;
		

	/**
	 * ���s��̌㏈��
	 */
	virtual void dispose() throw() = 0;

	/// ���[�J�[�p�G���g���|�C���g�B�I�[�o�[���C�h���Ďg�p����B
	virtual unsigned run() throw(ThreadException) = 0;

};

#endif /* RUNNABLE_HPP_ */
