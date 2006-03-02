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
					   bool isAutoReset = true) const throw()
	{
		return ::CreateEvent(NULL,
							 static_cast<BOOL>(isAutoReset),
							 FALSE,
							 eventName);
							 
	}

	/**
	 * デフォルトコンストラクタ
	 */
	WinEvent();

public:
	/**
	 * コンストラクタ
	 * @param eventName イベントを識別する名前
	 */
	WinEvent(const std::string& eventName) throw(): event()
	{
		event = createEvent(eventName.c_str());
		assert(event);
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
	 * イベントシグナルの瞬間的にon/offする
	 * @see waitEventArrive()
	 */
	void pulseEvent() throw()
	{
		assert(event != 0);

		BOOL result = ::PulseEvent(event);

		assert(result != FALSE);
	}

	/**
	 * イベントシグナルのリセット
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
		else
			assert(false);
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
};

typedef WinEvent Event;

#endif /* WINEVENT_HPP_ */
