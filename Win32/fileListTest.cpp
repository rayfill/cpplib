#include <iostream>
#include "FileList.hpp"

int main(int argc, char** argv)
{
	FileList list("*.*");

	for (FileList::iterator itor = list.begin();
		 itor != list.end();
		 ++itor)
	{
		std::cout << itor->cFileName << (itor.isDirectory() ? "\\" : "") << std::endl;
	}

	std::cout << std::endl;
	std::cout << "-----------file recursive list--------" << std::endl;
	FileRecursiveList recursiveList(argv[argc-1]);
	for (FileRecursiveList::iterator itor = recursiveList.begin();
		 itor != recursiveList.end();
		 ++itor)
	{
		std::cout << itor->cFileName;
		if (itor->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//		if (itor.isDirectory())
			std::cout << "\\";

		std::cout << std::endl;
	}

	return 0;
}
