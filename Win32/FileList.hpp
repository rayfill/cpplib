#ifndef FILELIST_HPP_
#define FILELIST_HPP_

#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

struct FindList
{};

class FileList
{
private:
	const std::basic_string<TCHAR> findString;
	bool directoryRecursive;

public:

	class iterator
	{
	private:
		HANDLE hFind;
		WIN32_FIND_DATA data;

		bool next()
		{
			assert(hFind != INVALID_HANDLE_VALUE);
			
			if (!FindNextFile(hFind, &data))
			{
				FindClose(hFind);
				hFind = INVALID_HANDLE_VALUE;
				return false;
			}
			return true;
		}

		iterator& operator=(const iterator& source) const throw();

	public:
		iterator() throw()
			: hFind(INVALID_HANDLE_VALUE),
			  data()
			 {}

		iterator(const iterator& source)
			: hFind(source.hFind),
			  data(source.data)
		{
		}

		iterator(const std::basic_string<TCHAR>& target) throw()
			: hFind(INVALID_HANDLE_VALUE),
			  data()
		{
			hFind = FindFirstFile(target.c_str(),
								  &data);
		}

		~iterator()
		{
			if (hFind != INVALID_HANDLE_VALUE)
				FindClose(hFind);
		}

		bool operator==(const iterator& rhs) const throw()
		{
			return this->hFind == rhs.hFind;
		}

		bool operator!=(const iterator& rhs) const throw()
		{
			return this->hFind != rhs.hFind;
		}
		
		const iterator& operator++() throw()
		{
			next();
			return *this;
		}

		const iterator operator++(int) throw()
		{
			iterator result = *this;
			this->next();
			return result;
		}

		WIN32_FIND_DATA operator*() throw()
		{
			return data;
		}

		WIN32_FIND_DATA* operator->() throw()
		{
			return &data;
		}
	};

	FileList(const std::basic_string<TCHAR>& findKey,
			 bool directoryRecursive_ = true) throw()
		: findString(findKey),
		  directoryRecursive(directoryRecursive_)
	{
	}

	virtual ~FileList() throw()
	{
	}

	iterator begin() const throw()
	{
		return iterator(findString.c_str());
	}

	iterator end() const throw()
	{
		return iterator();
	}
};

class FileRecursiveList
{
private:
	std::vector<WIN32_FIND_DATA> fileLists;

public:
	typedef std::vector<WIN32_FIND_DATA>::iterator iterator;

	iterator begin()
	{
		return fileLists.begin();
	}

	iterator end()
	{
		return fileLists.end();
	}

	FileRecursiveList(const std::basic_string<TCHAR>& targetDirectory):
		fileLists()
	{
		fileEntries(targetDirectory);
	}

	void fileEntries(const std::basic_string<TCHAR>& target)
	{
		FileList files(target + _T("\\*.*"));

		for (FileList::iterator itor = files.begin();
			 itor != files.end();
			 ++itor)
		{
			if (std::basic_string<TCHAR>(itor->cFileName) == "." ||
				std::basic_string<TCHAR>(itor->cFileName) == "..")
				continue;

			std::basic_string<TCHAR> fullpathFilename(target + "\\" +
													  itor->cFileName);
			assert(fullpathFilename.length() < MAX_PATH);

			std::copy(fullpathFilename.c_str(),
					  fullpathFilename.c_str() + fullpathFilename.length() + 1,
					  itor->cFileName);
			fileLists.push_back(*itor);
			
			if (itor->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				fileEntries(std::basic_string<TCHAR>(itor->cFileName));
		}
	}

	
};

#endif /* FILELIST_HPP_ */
