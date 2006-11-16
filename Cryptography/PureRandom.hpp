#ifndef PURERANDOM_HPP_
#define PURERANDOM_HPP_

/**
 * 乱数インターフェース
 */
class PureRandom
{
public:
	PureRandom() {}
	virtual ~PureRandom() {}
	/// バイト乱数の取得
	virtual unsigned char getRandom() = 0;
};

#endif /* PURERANDOM_HPP_ */
