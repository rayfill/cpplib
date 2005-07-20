#include <fstream>
#include <iostream>
#include <iterator>
#include <cmath>
#include <Win32/WaveClass.hpp>


int main(int argc, char** argv)
{
	DWORD start = GetTickCount();

	WaveOutDevice<WaveFile::iterator> wod(WAVE_MAPPER);
	WaveFile wavefile(argv[argc - 1]);

	wod.setWaveFormat(wavefile.getWaveFormat());
	wod.setPCMData(wavefile.begin(), wavefile.end());

	wod.open();
	wod.play();
	while(!wod.isPlayEnd()) {
		Sleep(10);
//		std::cout << "playing..." << std::endl;
	}

	wod.close();

	std::cout << "play time at " << ((GetTickCount() - start) / 1000.0)
			  << "sec(s)." << std::endl;
	return 0;
}
