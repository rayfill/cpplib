#ifndef BARRIERSYNC_HPP_
#define BARRIERSYNC_HPP_

#ifdef WIN32
#	include <WinThread/WinBarrier.hpp>
#else
#	include <PosixThread/PosixBarrier.hpp>
#endif /* WIN32 */

#endif /* BARRIERSYNC_HPP_ */
