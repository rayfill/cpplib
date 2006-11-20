#include <cppunit/extensions/HelperMacros.h>
#include <util/SmartPointer.hpp>
#include <assert.h>

class Callbacker
{
private:
	Callbacker(const Callbacker&);
	Callbacker& operator=(const Callbacker&);

public:
	bool* flags;

	void operator()(bool* pBool)
	{
		assert(*pBool == false);
		(*pBool) = true;
	}

	Callbacker(bool* flags_)
		: flags(flags_)
	{}

	virtual ~Callbacker()
	{
		(*this)(flags);
	}
};

class SmartPointerTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(SmartPointerTest);
	CPPUNIT_TEST(referenceTest);
	CPPUNIT_TEST(arrayReferenceTest);
	CPPUNIT_TEST(operatorEqualTest);
	CPPUNIT_TEST(arrayOperatorEqualTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void operatorEqualTest()
	{
		bool isDeleted = false;
		bool isDeleted2 = false;
		{
			Callbacker* p = new Callbacker(&isDeleted);
			Callbacker* p2 = new Callbacker(&isDeleted2);

			SmartPointer<Callbacker> sp(p);
			{
				SmartPointer<Callbacker> sp2(p2);
				sp = sp2;

				CPPUNIT_ASSERT(isDeleted == true);
				CPPUNIT_ASSERT(isDeleted2 == false);
			}
			CPPUNIT_ASSERT(isDeleted2 == false);
		}
		CPPUNIT_ASSERT(isDeleted2 == true);
	}

	void referenceTest()
	{
		bool isDeleted = false;

		{
			Callbacker* p = new Callbacker(&isDeleted);
			SmartPointer<Callbacker> sp(p);
			CPPUNIT_ASSERT(sp.refCount->getReferenceCount() == 1);

			{
				SmartPointer<Callbacker> sp2 = sp;
				CPPUNIT_ASSERT(sp.refCount == sp2.refCount);
				CPPUNIT_ASSERT(sp2.refCount->getReferenceCount() == 2);

				CPPUNIT_ASSERT(sp2.get() == p);
			}
			CPPUNIT_ASSERT(isDeleted == false);
			CPPUNIT_ASSERT(sp.get() == p);
		}
		CPPUNIT_ASSERT(isDeleted == true);
	}

	void arrayOperatorEqualTest()
	{
		bool isDeleted = false;
		bool isDeleted2 = false;
		{
			Callbacker* p = new Callbacker(&isDeleted);
			Callbacker* p2 = new Callbacker(&isDeleted2);

			SmartArray<Callbacker, DefaultRemover<Callbacker> > sp(p);
			{
				SmartArray<Callbacker, DefaultRemover<Callbacker> > sp2(p2);
				sp = sp2;

				CPPUNIT_ASSERT(isDeleted == true);
				CPPUNIT_ASSERT(isDeleted2 == false);
			}
			CPPUNIT_ASSERT(isDeleted2 == false);
		}
		CPPUNIT_ASSERT(isDeleted2 == true);
	}

	void arrayReferenceTest()
	{
		bool isDeleted = false;

		{
			Callbacker* p = new Callbacker(&isDeleted);
			SmartArray<Callbacker, DefaultRemover<Callbacker> > sp(p);
			CPPUNIT_ASSERT(sp.refCount->getReferenceCount() == 1);

			{
				SmartArray<Callbacker, DefaultRemover<Callbacker> > sp2 = sp;
				CPPUNIT_ASSERT(sp.refCount == sp2.refCount);
				CPPUNIT_ASSERT(sp2.refCount->getReferenceCount() == 2);

				CPPUNIT_ASSERT(sp2.get() == p);
			}
			CPPUNIT_ASSERT(sp.refCount->getReferenceCount() == 1);
			CPPUNIT_ASSERT(isDeleted == false);
			CPPUNIT_ASSERT(sp.get() == p);
		}
		CPPUNIT_ASSERT(isDeleted == true);
	}
};

class ScopedPointerTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(ScopedPointerTest);
	CPPUNIT_TEST(referenceTest);
	CPPUNIT_TEST(operatorEqualTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void operatorEqualTest()
	{
		bool isDeleted = false;
		bool isDeleted2 = false;
		{
			Callbacker* p = new Callbacker(&isDeleted);
			Callbacker* p2 = new Callbacker(&isDeleted2);

			ScopedPointer<Callbacker> sp(p);
			{
				ScopedPointer<Callbacker> sp2(p2);
				//sp = sp2; // compile time error.

				CPPUNIT_ASSERT(isDeleted == false);
				CPPUNIT_ASSERT(isDeleted2 == false);
			}
			CPPUNIT_ASSERT(isDeleted == false);
			CPPUNIT_ASSERT(isDeleted2 == true);
			
		}
		CPPUNIT_ASSERT(isDeleted == true);
	}

	void referenceTest()
	{
		bool isDeleted = false;

		{
			Callbacker* p = new Callbacker(&isDeleted);
			ScopedPointer<Callbacker> sp(p);
			{
				CPPUNIT_ASSERT(sp.get() == p);
			}
			CPPUNIT_ASSERT(isDeleted == false);
			CPPUNIT_ASSERT(sp.get() == p);
		}
		CPPUNIT_ASSERT(isDeleted == true);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( SmartPointerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( ScopedPointerTest );
