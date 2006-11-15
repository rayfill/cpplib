#ifndef EVENT_HPP_
#define EVENT_HPP_

#ifdef WIN32
#	include <WinThread/WinEvent.hpp>
#else
#	include <PosixThread/PosixEvent.hpp>
#endif /* WIN32 */

#endif /* EVENT_HPP_ */
