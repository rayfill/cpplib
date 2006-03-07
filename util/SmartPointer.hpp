#ifndef SMARTPOINTER_HPP_
#define SMARTPOINTER_HPP_

#include <cassert>
#include <stdexcept>

/**
 * SmartPointer用ポインタ削除時動作ポリシー
 * @param Container コンテナ型
 */
template <typename Container>
class DefaultRemover
{
public:
	/**
	 * 削除ハンドラ
	 * @param pointer 削除オブジェクトのポインタ
	 */
	static void remove(Container* pointer)
	{
		delete pointer;
	}
};

/**
 * 配列削除ポリシー
 * @param Container 削除する配列の要素の型
 */
template <typename Container>
class ArrayRemover
{
public:
	/**
	 * 削除ハンドラ
	 * @param pointer 削除オブジェクト配列の先頭ポインタ
	 */
	static void remove(Container* pointer)
	{
		delete[] pointer;
	}
};

/**
 * 参照カウンタ
 */
struct ReferenceCounter
{
private:
	friend class SmartPointerTest;

	/**
	 * 参照カウント変数
	 */
	int reference;

	/**
	 * 参照カウント取得
	 * @return 現在の参照カウント
	 */
	int getReferenceCount() const throw()
	{
		return reference;
	}

	/// コピー防止用
	ReferenceCounter(const ReferenceCounter&);
	ReferenceCounter operator=(const ReferenceCounter&);
public:
	/**
	 * コンストラクタ
	 */
	ReferenceCounter()
		: reference(0)
	{
		assert(reference == 0);
	}

	/**
	 * 参照カウントの加算
	 * @return 増加後の参照カウント数
	 */
	int addReference()
	{
		return ++reference;
	}
		
	/**
	 * 参照カウントの減算
	 * @return 減算後の参照カウント数
	 */
	int release()
	{
		assert(reference > 0);
		return --reference;
	}
};

/**
 * スマートポインタクラス
 * @param Container コンテナ化するクラス
 * @param RemovePolicy 削除時のハンドリングポリシー
 * @todo MultiThread環境のためのロックポリシーとかも必要かも
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
	 * 所有権の破棄
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
	 * 管理対象ポインタ
	 */
	Pointer pointer;

	/**
	 * 参照カウントホルダ
	 */
	ReferenceCounter* refCount;

public:
	/**
	 * コンストラクタ
	 * @param pointer_ 管理対象となるポインタ
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
	 * コピーコンストラクタ
	 * @param src コピー元オブジェクト
	 */
	SmartPointer(const SmartPointer& src)
		throw() :
		pointer(src.pointer),
		refCount(src.refCount)
	{
		refCount->addReference();
	}

	/**
	 * デストラクタ
	 */
	virtual ~SmartPointer()
	{
		release();
	}

	/**
	 * ポインタの取得
	 */
	Pointer get() const throw()
	{
		return pointer;
	}

	/**
	 * 比較演算
	 * @return 等値なポインタを持つ場合、true
	 */
	bool operator==(const SmartPointer& src) const throw()
	{
		if (this == &src)
			return true;

		return this->pointer == src.pointer;
	}

	/**
	 * 代入演算
	 * @param src コピー元
	 * @return コピー後の自身への参照
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
	 * メンバ呼び出し
	 */
	Pointer operator->() const throw()
	{
		return pointer;
	}
};

/**
 * スマート配列
 * @param Container 要素の型
 * @RemovePolicy メモリ開放ポリシー
 * 要素のデストラクタは例外を投げないこと
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
	 * 開放処理
	 * ただし実際に開放されるのは参照がなくなった時のみ
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
	 * 要素配列へのポインタ
	 */
	Pointer pointer;

	/**
	 * 参照カウンタへのポインタ
	 */
	ReferenceCounter* refCount;
	
public:
	/**
	 * コンストラクタ
	 * @param pointer_ 管理対象の生ポインタ
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
	 * コピーコンストラクタ
	 * @param src コピー元スマート配列
	 */
	SmartArray(const SmartArray& src)
		throw() :
		pointer(src.pointer),
		refCount(src.refCount)
	{
		refCount->addReference();
	}

	/**
	 * デストラクタ
	 */
	virtual ~SmartArray()
	{
		release();
	}

	/**
	 * 生ポインタの取得
	 */
	Pointer get() const throw()
	{
		return pointer;
	}

	/**
	 * 要素へのindexベースアクセス
	 * @param index 要素へのオフセット値(ゼロオリジンベースです)
	 */
	Reference operator[](int index)
	{
		return pointer[index];
	}

	/**
	 * 等価比較演算子
	 * @return 保持しているポインタが同じものなら true
	 */
	bool operator==(const SmartArray& src) const throw()
	{
		if (this == &src)
			return true;

		return this->pointer == src.pointer;
	}

	/**
	 * 代入演算子
	 * @param src コピー元
	 * @return SmartArray コピー後の参照
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
