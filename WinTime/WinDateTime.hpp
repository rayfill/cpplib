#ifndef WINTIME_HPP_
#define WINTIME_HPP_
#include <windows.h>

class WinTime
{
private:
	/**
	 * 現在時刻を保存する構造体(世界協定時で保存)
	 */
	SYSTEMTIME systemTime;

public:
	
};

typedef WinTime Time;

#endif /* WINTIME_HPP_ */
