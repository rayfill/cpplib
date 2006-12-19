#ifndef TRANSFER_HPP_
#define TRANSFER_HPP_

/**
 * 矩形ビットマップ転送ファンクタ
 * @param sourceType 転送元のビットマップイテレータ型
 * @param destinationType 転送先のビットマップイテレータ型
 * 同型の場合、C++のインライン展開と最適化により高速なコピーができます。
 * 型が違う場合でも正しいコピーはできますが、速度は落ちます。
 * @todo MMX, SSE[1-3]を使ったより高速なバージョンとか
 */
template <typename sourceType, typename destinationType>
class BlockTransfer
{
private:
	typedef sourceType SourceType;
	typedef destinationType DestinationType;

	const SourceType& source;
	DestinationType& destination;

	template <typename SrcIterator,
		typename DestIterator>
	void transferFragment(SrcIterator head, SrcIterator tail,
						DestIterator destHead)
	{
		for (; head != tail; ++head, ++destHead)
			destHead->setColor(head->getColor());
	}

public:
	BlockTransfer(const sourceType& src, destinationType& dest) throw()
		: source(src), destination(dest)
	{}

	~BlockTransfer() throw()
	{}

	template <typename pointType> 
	void transfer(
		const geometry::Rectangle<pointType>& sourceRect,
		const geometry::Point<pointType>& destPoint)
		throw(std::invalid_argument)
	{
		typedef pointType point_t;
		if (geometry::Rectangle<point_t>
			(0, 0, source.getWidth(), source.getHeight()).
			getIntersection(sourceRect) != sourceRect)
			throw std::invalid_argument("source rectangle is out of range.");

		const geometry::Rectangle<point_t>
			destRect(
				0,
				0,
				destination.getWidth(),
				destination.getHeight());

		const geometry::Rectangle<point_t>
			destTargetRect(
				destPoint.getX(),
				destPoint.getY(),
				destPoint.getX() + sourceRect.getWidth(),
				destPoint.getY() + sourceRect.getHeight());
		if (!destRect.isCollision(destTargetRect))
			return;

		geometry::Rectangle<point_t> transferRect = 
			destRect.getProjection(destTargetRect);
		
		const geometry::Point<point_t> projectionMove =
			sourceRect.getLeftTop() - destPoint;
		const geometry::Rectangle<point_t>
			srcTransferRect(transferRect + projectionMove);

		// srcTransferRect -> transferRect copy.
		// line transter
		for (point_t srcHeight = srcTransferRect.getTop(),
				 destHeight = transferRect.getTop();
			 srcHeight != srcTransferRect.getBottom();
			 ++srcHeight, ++destHeight)
			transferFragment(
				source.getPixel(srcTransferRect.getLeft(), srcHeight),
				source.getPixel(srcTransferRect.getRight(), srcHeight),
				destination.getPixel(transferRect.getLeft(), destHeight));
	}
};

/**
 * 透過色付ビットマップ転送
 * @param sourceType 転送元のビットマップイテレータ型
 * @param destinationType 転送先のビットマップイテレータ型
 */
template <class sourceType, class destinationType>
class TransparencyTransfer
{
private:
	typedef sourceType SourceType;
	typedef destinationType DestinationType;

	const SourceType& source;
	DestinationType& destination;
	const Color transparentColor;

	template <typename SrcIterator,
		typename DestIterator>
	void transferFragment(SrcIterator head, SrcIterator tail,
						DestIterator destHead)
	{
		for (; head != tail; ++head, ++destHead)
		{
			if (head->getColor() != transparentColor)
			destHead->setColor(head->getColor());
		}
	}

public:
	TransparencyTransfer(const sourceType& src,
						 destinationType& dest,
						 Color color) throw()
		: source(src), destination(dest), transparentColor(color)
	{}

	~TransparencyTransfer() throw()
	{}

	template <typename pointType> 
	void transfer(
		const geometry::Rectangle<pointType>& sourceRect,
		const geometry::Point<pointType>& destPoint)
		throw(std::invalid_argument)
	{
		typedef pointType point_t;
		if (geometry::Rectangle<point_t>
			(0, 0, source.getWidth(), source.getHeight()).
			getIntersection(sourceRect) != sourceRect)
			throw std::invalid_argument("source rectangle is out of range.");

		const geometry::Rectangle<point_t>
			destRect(
				0,
				0,
				destination.getWidth(),
				destination.getHeight());

		const geometry::Rectangle<point_t>
			destTargetRect(
				destPoint.getX(),
				destPoint.getY(),
				destPoint.getX() + sourceRect.getWidth(),
				destPoint.getY() + sourceRect.getHeight());
		if (!destRect.isCollision(destTargetRect))
			return;

		geometry::Rectangle<point_t> transferRect = 
			destRect.getProjection(destTargetRect);
		
		const geometry::Point<point_t> projectionMove =
			sourceRect.getLeftTop() - destPoint;
		const geometry::Rectangle<point_t>
			srcTransferRect(transferRect + projectionMove);

		// srcTransferRect -> transferRect copy.
		// line transter
		for (point_t srcHeight = srcTransferRect.getTop(),
				 destHeight = transferRect.getTop();
			 srcHeight != srcTransferRect.getBottom();
			 ++srcHeight, ++destHeight)
			transferFragment(
				source.getPixel(srcTransferRect.getLeft(), srcHeight),
				source.getPixel(srcTransferRect.getRight(), srcHeight),
				destination.getPixel(transferRect.getLeft(), destHeight));
	}
};

/**
 * 排他的論理和転送
 * @todo MMXやMMX2などを使ったblitterの実装。
 * そのためにコンストラクタと転送関数分けたんだし・・・
 * あと各種論理演算系転送とか。
 * 転送範囲計算を別ポリシーとして分割したほうがいいかも。
 * そうすればすっきりするし、DirtyAreaとかとの兼ね合いもうまくいくかも。
 */
template <class sourceType, class destinationType>
class ExclusiveOrTransfer
{
private:
	typedef sourceType SourceType;
	typedef destinationType DestinationType;

	/**
	 * 転送元
	 */
	const SourceType& source;

	/**
	 * 転送先
	 */
	DestinationType& destination;
	
	/**
	 * マスクカラー
	 */
	const Color exclusiveColor;

	/**
	 * 一列分の転送
	 */
	template <typename SrcIterator,
		typename DestIterator>
	void transferFragment(SrcIterator head, SrcIterator tail,
						DestIterator destHead)
	{
		for (; head != tail; ++head, ++destHead)
		{
			destHead->setColor(head->getColor() ^ exclusiveColor);
		}
	}

public:
	/**
	 * コンストラクタ
	 * @param src 転送元
	 * @param dest 転送先
	 * @param color マスクカラー
	 */
	ExclusiveOrTransfer(const sourceType& src,
						 destinationType& dest,
						 Color color) throw()
		: source(src), destination(dest), exclusiveColor(color)
	{}

	/**
	 * デストラクタ
	 */
	~ExclusiveOrTransfer() throw()
	{}

	/**
	 * 転送関数
	 * @param sourceRect 転送元矩形範囲
	 * @param destPoint 転送先左上位置
	 */
	template <typename pointType> 
	void transfer(
		const geometry::Rectangle<pointType>& sourceRect,
		const geometry::Point<pointType>& destPoint)
		throw(std::invalid_argument)
	{
		typedef pointType point_t;
		if (geometry::Rectangle<point_t>
			(0, 0, source.getWidth(), source.getHeight()).
			getIntersection(sourceRect) != sourceRect)
			throw std::invalid_argument("source rectangle is out of range.");

		const geometry::Rectangle<point_t>
			destRect(
				0,
				0,
				destination.getWidth(),
				destination.getHeight());

		const geometry::Rectangle<point_t>
			destTargetRect(
				destPoint.getX(),
				destPoint.getY(),
				destPoint.getX() + sourceRect.getWidth(),
				destPoint.getY() + sourceRect.getHeight());
		if (!destRect.isCollision(destTargetRect))
			return;

		geometry::Rectangle<point_t> transferRect = 
			destRect.getProjection(destTargetRect);
		
		const geometry::Point<point_t> projectionMove =
			sourceRect.getLeftTop() - destPoint;
		const geometry::Rectangle<point_t>
			srcTransferRect(transferRect + projectionMove);

		// srcTransferRect -> transferRect copy.
		// line transter
		for (point_t srcHeight = srcTransferRect.getTop(),
				 destHeight = transferRect.getTop();
			 srcHeight != srcTransferRect.getBottom();
			 ++srcHeight, ++destHeight)
			transferFragment(
				source.getPixel(srcTransferRect.getLeft(), srcHeight),
				source.getPixel(srcTransferRect.getRight(), srcHeight),
				destination.getPixel(transferRect.getLeft(), destHeight));
	}
};

#endif /* TRANSFER_HPP_ */
