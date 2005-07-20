#ifndef THREAD_HPP_
#define THREAD_HPP_

#ifdef WIN32
#	include <WinThread/WinThread.hpp>
#else
#	include <PThread/PosixThread.hpp>
#endif /* WIN32 */

#endif /* THREAD_HPP_ */
