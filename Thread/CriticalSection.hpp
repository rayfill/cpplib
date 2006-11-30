#ifndef CRITICALSECTION_HPP_
#define CRITICALSECTION_HPP_

#ifdef _WIN32
#	include <WinThread/WinCriticalSection.hpp>
#else
#	error "not implemented yet."
#	include <PosixThread/PosixCriticalSection.hpp>
#endif /* WIN32 */

#endif /* CRITICALSECTION_HPP_ */
