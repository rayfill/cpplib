#ifndef BASE_NOISE_HPP_
#define BASE_NOISE_HPP_

#ifdef WIN32
#	include <Win32/Noise.hpp>
#else
#	include <Posix/Noise.hpp>
#endif /* WIN32 */

#endif /* BASE_NOISE_HPP_ */
