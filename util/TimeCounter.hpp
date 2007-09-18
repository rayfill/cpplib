#ifndef TIMECOUNTER_HPP_
#define TIMECOUNTER_HPP_

#ifdef WIN32
#	include <util/WinTimeCounter.hpp>
#else
#	include <util/PosixTimeCounter.hpp>
#endif /* WIN32 */

#endif /* TIMECOUNTER_HPP_ */
