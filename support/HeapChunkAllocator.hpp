#ifndef HEAPCHUNKALLOCATOR_HPP_
#define HEAPCHUNKALLOCATOR_HPP_

#include <deque>
#include <vector>
#include <algorithm>
#include <util/Singleton.hpp>

template <size_t AllocHeapSize,
	size_t DefaultAllocCount>
class HeapManager
{
	friend class HeapManagerTest;

private:
	std::deque<void*> heaps;
	std::vector<void*> osHeap;
	const size_t HeapUnit;

	void alloc()
	{
		osHeap.push_back(new char[HeapUnit]());
		void* head = osHeap.back();
		for (size_t count = 0; count < DefaultAllocCount; ++count)
			heaps.push_back(
				reinterpret_cast<char*>(head) +
				AllocHeapSize * count);
	}
	void free()
	{
		for (std::vector<void*>::iterator itor = osHeap.begin();
			 itor != osHeap.end();
			 ++itor)
			delete [] reinterpret_cast<char*>(*itor);
	}
	
	HeapManager(const HeapManager&);
	HeapManager& operator=(HeapManager&);
public:
	HeapManager():
		heaps(), osHeap(),
		HeapUnit(AllocHeapSize * DefaultAllocCount)
	{
		alloc();
	}

	~HeapManager()
	{
		free();
	}
	
	void* checkout(size_t count)
	{
		if (count > DefaultAllocCount)
			throw std::invalid_argument(
				"allocate request memory too large.");

		char* p = NULL;
		if (heaps.size() < count)
			alloc();

		if (count == 1)
		{
			p = reinterpret_cast<char*>(heaps.front());
			heaps.pop_front();
			return p;
		}

		std::sort(heaps.begin(), heaps.end());
		for (size_t index = 0; index < heaps.size(); ++index)
		{
			p = reinterpret_cast<char*>(heaps[index]);
			char* q = p;
			for (size_t chainIndex = index + 1;
				 chainIndex < heaps.size();
				 ++chainIndex)
			{
				// search chaining heap memory block.
				if (AllocHeapSize != 
					(reinterpret_cast<char*>(heaps[chainIndex]) - q))
				{
					index = chainIndex - 1;
					break;
				}
				q = reinterpret_cast<char*>(heaps[chainIndex]);

				if ((chainIndex - index) == (count - 1))
					break;
			}

			if ((q - p) ==
				static_cast<ptrdiff_t>(AllocHeapSize * (count - 1)))
			{
				// hit chaining heap memory block.
				heaps.erase(heaps.begin() + index,
							heaps.begin() + index + count);
				return p;
			}
		}
		throw std::bad_alloc();
	}
	
	void release(void* p, size_t count = 1)
	{
		for (size_t index = count; index > 0; --index)
		{
			heaps.push_front(
				reinterpret_cast<char*>(p) + AllocHeapSize * (index - 1));
		}
	}
};

template <typename Types,
	size_t AllocHeapSize = 1024 * 16,
	size_t DefaultAllocCount = 32>
class HeapChunkAllocator
{
	friend class HeapChunkAllocatorTest;

private:
	typedef Singleton<HeapManager<AllocHeapSize, DefaultAllocCount> > HeapMan;

public:
	typedef Types* pointer;
	typedef const Types* const_pointer;
	typedef Types& reference;
	typedef const Types& const_reference;
	typedef Types value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	template <typename U>
	struct rebind
	{
		typedef
		HeapChunkAllocator<U, AllocHeapSize, DefaultAllocCount>
		other; 
	};

	pointer address(reference r)
	{
		return &r;
	}
	const_pointer address(const_reference s)
	{
		return &s;
	}

	pointer allocate(size_type n, const void* = 0) throw(std::bad_alloc)
	{
		if (n == 0)
			return NULL;
		
		const size_t appendSize =
			((sizeof(Types) * n) % AllocHeapSize) == 0 ? 0 : 1;

		return reinterpret_cast<pointer>(
			HeapMan::get()->checkout(
				static_cast<size_t>((n / AllocHeapSize) + appendSize)));
	}

	void deallocate(pointer p, size_type n)
	{
		if (n == 0)
			return;

		const size_t appendSize =
			((sizeof(Types) * n) % AllocHeapSize) == 0 ? 0 : 1;
		
		HeapMan::get()->release(
			p, static_cast<size_t>((n /AllocHeapSize) + appendSize));
	}

	size_type max_size() const
	{
		// == 2GB / object_size
		return 2UL * 1024 * 1024 * 1024 / sizeof(Types);
	}

	bool operator==(const HeapChunkAllocator&) const
	{
		return true;
	}

	bool operator!=(const HeapChunkAllocator& dest) const
	{
		return !this->operator==(dest);
	}

	HeapChunkAllocator() throw()
	{}

	~HeapChunkAllocator() throw()
	{}

	HeapChunkAllocator(const HeapChunkAllocator&) throw()
	{}

	void construct(pointer p, const Types& value)
	{
		// placement new.
		new (p) Types(value);
	}

	void destroy(pointer p)
	{
		p->~Types();
	}
};

#endif /* HEAPCHUNKALLOCATOR_HPP_ */
