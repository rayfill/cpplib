#ifndef NOTIFICATION_HPP_
#define NOTIFICATION_HPP_

#include <exception>
#include <stdexcept>
#include <list>

class Observable;

/**
 * Observableクラスからの通知を受け取ることを宣言するインタフェース
 */
class Observer
{
	friend class Observable;
protected:
public:
	/**
	 * 通知受け取りのためのインタフェース
	 * @arg notifier 通知元 Observable クラスのポインタ
	 */
	virtual void notify(Observable* notifier) = 0;

	/**
	 * デフォルトデストラクタ
	 */
	virtual ~Observer() {}

};

/**
 * Observable クラスに通知をするクラスが実装するインタフェース
 */
class Observable
{
private:
	typedef std::list<Observer*> ServerList;

	/**
	 * 通知を受け取る Observer クラスのリスト
	 */
	ServerList servers;

protected:
	/**
	 * 内部で使用する検索用ファンクタ
	 */
	class Finder : public std::unary_function<Observer*, bool>
	{
	private:
		/**
		 * 検索対象の Observer クラスのポインタ
		 */
		const Observer* server;

		/**
		 * 生成不可能のためのプライベートデフォルトコンストラクタ
		 */
		Finder() throw() {}

	protected:
		/**
		 * 内部使用のためのコピーコンストラクタ
		 */
		Finder(const Observer* server_) throw(): server(server_) {}

	public:
		/**
		 * ファクトリメソッド
		 * @arg 検索対象となる Observer クラスのポインタ
		 */
		static Finder create(Observer* server_) throw()
		{
			Finder result(server_);
			return result;
		}

		/**
		 * ファンクタメソッド
		 * @arg target 比較対象となる Observer クラスのポインタ
		 * @return 比較結果. 等しい場合:true, それ以外:false
		 */
		bool operator()(Observer* target) throw()
		{
			return target == server;
		}
	};

public:
	/**
	 * コンストラクタ
	 */
	Observable() throw():
		servers()
	{
	}

	/**
	 * デストラクタ
	 */
	virtual ~Observable() {}

	/**
	 * 通知の実行
	 */
	void update()
	{
		for(ServerList::iterator itor = servers.begin();
			itor != servers.end();
			++itor)
		{
			(*itor)->notify(this);
		}
	}

	/**
	 * Observer のアタッチ
	 * @arg server 接続する Observer クラスのポインタ
	 * @exception std::bad_alloc() メモリが足りなかった場合
	 */
	void attachObserver(Observer* server) throw(std::bad_alloc)
	{
		servers.push_back(server);
	}

	/**
	 * オブザーバの切断
	 * @arg detachServer 切断する Observer クラスのポインタ
	 */
	void detachObserver(Observer* detachServer) throw()
	{
		servers.remove_if(Finder::create(detachServer));
	}

	/**
	 * 接続されているクライアントの数の取得
	 */
	const size_t serverCount() throw()
	{
		return servers.size();
	}
};

#endif /* NOTIFICATION_HPP_ */
