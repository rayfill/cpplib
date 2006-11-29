#ifndef SCOKETIMPL_HPP_
#define SCOKETIMPL_HPP_

#ifdef WIN32
#	include <Socket/Win32SocketImpl.hpp>
#else
#	include <Socket/PosixSocketImpl.hpp>
#endif /* WIN32 */

#endif /* SCOKETIMPL_HPP_ */
