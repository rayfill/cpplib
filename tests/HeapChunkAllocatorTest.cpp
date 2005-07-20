#include <iostream>
#include <support/HeapChunkAllocator.hpp>

#include <cassert>
#include <typeinfo>

class HeapChunkAllocatorTest
{
public:
	void test()
	{
		typedef HeapChunkAllocator<unsigned short> HA;
		HA p, q;

		assert(p == q);
		assert(!(p != q));

		HA::pointer ptr = p.allocate(4097);
		p.construct(ptr, 15);
		assert(*ptr == 15);

		p.destroy(ptr);
	}
};

class HeapManagerTest
{
public:
	void test()
	{
		HeapManager<4096, 32> manager;

		assert(manager.heaps.size() == 32);
		assert(manager.osHeap.size() == 1);

		void* p, *q, *r, *s, *t;
		p = q = r = s = t = NULL;
		p = manager.checkout(15);
		q = manager.checkout(16);
		assert(manager.heaps.size() == 1);
		assert(manager.osHeap.size() == 1);
		assert(p < q);

		manager.release(p, 15);
		assert(manager.heaps.size() == 16);
		assert(manager.osHeap.size() == 1);

		r = manager.checkout(15);
		assert(p == r);
		assert(manager.heaps.size() == 1);
		assert(manager.osHeap.size() == 1);

		manager.release(r, 15);
		manager.release(q, 16);
		assert(manager.heaps.size() == 32);
		assert(manager.osHeap.size() == 1);
	
		p = manager.checkout(18);
		assert(manager.heaps.size() == 14);
		assert(manager.osHeap.size() == 1);

		q = manager.checkout(15);
		assert(manager.heaps.size() == 14 + 32 - 15);
		assert(manager.osHeap.size() == 2);

		p = manager.checkout(1);
		manager.release(p, 1);
		assert(manager.heaps.size() == 14 + 32 - 15);
		assert(manager.osHeap.size() == 2);

		for (int i = 0; i < 10000; ++i)
		{
			q = manager.checkout(1);
			manager.release(q, 1);
			assert(manager.heaps.size() == 14 + 32 - 15);
			assert(manager.osHeap.size() == 2);
		}
		
	}
};

int main()
{
	HeapManagerTest test;
	HeapChunkAllocatorTest test2;
	test2.test();
	test.test();
	return 0;
};
