#ifndef PURERANDOM_HPP_
#define PURERANDOM_HPP_

/**
 * �����C���^�[�t�F�[�X
 */
class PureRandom
{
public:
	PureRandom() {}
	virtual ~PureRandom() {}
	/// �o�C�g�����̎擾
	virtual unsigned char getRandom() = 0;
};

#endif /* PURERANDOM_HPP_ */
