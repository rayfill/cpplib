#ifndef MUTEX_HPP_
#define MUTEX_HPP_

#ifdef WIN32
#	include <WinThread/WinMutex.hpp>
#else
#	include <PosixThread/PosixMutex.hpp>
#endif /* WIN32 */

#endif /* MUTEX_HPP_ */
