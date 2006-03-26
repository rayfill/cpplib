#ifndef WINEVENT_HPP_
#define WINEVENT_HPP_
#include <windows.h>
#include <string>
#include <cassert>

/**
 * Win32 イベントクラス
 */
class WinEvent
{
private:
	/**
	 * イベントハンドル
	 */
	HANDLE event;

	/**
	 * イベントハンドルの作成
	 * @param eventName イベント名
	 * @param isAutoReset イベントがonになった場合、
	 * イベントを自動リセットするか？
	 * @return 作成されたイベントハンドル
	 */
	HANDLE createEvent(const char* eventName,
					   bool isAutoReset) const throw()
	{
		HANDLE hEvent = ::CreateEvent(NULL,
									  static_cast<BOOL>(!isAutoReset),
									  FALSE,
									  eventName);

		
		return hEvent;
							 
	}

public:
	/**
	 * コンストラクタ
	 * @param eventName イベントを識別する名前
	 * @param isAutoReset イベント検地後にシグナル停止にするかのフラグ
	 * @todo explicit つけてもなぜか自動型変換されてchar*引数ひとつだともうひとつの
	 * コンストラクタが呼ばれる・・・
	 * @note とりあえずデフォルト引数削るので対応。
	 */
	explicit WinEvent(const std::string& eventName, bool isAutoReset) throw()
			: event()
	{
		event = createEvent(eventName.c_str(), isAutoReset);
		assert(event != NULL);
	}

	/**
	 * コンストラクタ
	 * @param isAutoReset イベント検地後にシグナル停止にするかのフラグ
	 * @note 名前なしイベントを作成する
	 */
	explicit WinEvent(bool isAutoReset) throw()
			: event()
	{
		event = createEvent(NULL, isAutoReset);
		assert(event != NULL);
	}

	/**
	 * デストラクタ
	 */
	virtual ~WinEvent() throw()
	{
		CloseHandle(event);
		event = static_cast<HANDLE>(0);
	}

	/**
	 * イベントシグナルの待機しているものたちを全てブロック開放した後に
	 * 非シグナルに切り替える
	 * @see waitEventArrive()
	 */
	void pulseEvent() throw()
	{
		assert(event != 0);

		BOOL result = ::PulseEvent(event);

		assert(result != FALSE);
	}

	/**
	 * イベントシグナルのセット
	 */
	void setEvent() throw()
	{
		assert(event != 0);

		BOOL result = ::SetEvent(event);

		assert(result != FALSE);
	}

	/**
	 * イベントシグナルのリセット
	 */
	void resetEvent() throw()
	{
		assert(event != 0);

		BOOL result = ::ResetEvent(event);

		assert(result != FALSE);
	}

	/**
	 * イベントの到達検査
	 * @param milliseconds イベント待ちの最大時間。デフォルトではノンブロック
	 * @return 待ち時間中にイベントが到達すればtrue
	 */
	bool isEventArrived(DWORD milliseconds = 0)
	{
		assert(event != 0);

		DWORD result = WaitForSingleObject(event, milliseconds);

		if (result == WAIT_OBJECT_0)
			return true;
		else if (result == WAIT_TIMEOUT)
			return false;
		else if (result == WAIT_ABANDONED ||
				 result == WAIT_FAILED)
			assert(false);
		else
			assert(false && !"unknown result");
	}

	/**
	 * イベント待ちブロッキング
	 * イベントが到着するまで実行をブロックする。その間CPUは使わない
	 */
	void waitEventArrive()
	{
		assert(event != 0);

		this->isEventArrived(INFINITE);
	}

	/**
	 * イベントハンドルの取得
	 * @return イベントハンドル
	 */
	HANDLE getHandle() const throw()
	{
		return event;
	}
};

typedef WinEvent Event;

#endif /* WINEVENT_HPP_ */
