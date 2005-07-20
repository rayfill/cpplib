#ifndef PURERANDOM_HPP_
#define PURERANDOM_HPP_

class PureRandom
{
public:
	PureRandom() {}
	virtual ~PureRandom() {}
	virtual unsigned char getRandom() = 0;
};

#endif /* PURERANDOM_HPP_ */
