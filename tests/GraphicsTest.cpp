#include <cppunit/extensions/helpermacros.h>
#include <Graphics/Graphics.hpp>

class GraphicsTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(GraphicsTest);
	CPPUNIT_TEST(OrderListTest);
	CPPUNIT_TEST_SUITE_END();
public:
	void OrderListTest()
	{
		OrderList<int> orderList;
		int a, b, c, d, e;
		orderList.add(&a);
		orderList.add(&b);
		orderList.add(&c);
		orderList.add(&d);
		orderList.add(&e);

		// current state is front -> e, d, c, b, a -> back
		CPPUNIT_ASSERT(orderList[0] == &e);
		CPPUNIT_ASSERT(orderList[1] == &d);
		CPPUNIT_ASSERT(orderList[2] == &c);
		CPPUNIT_ASSERT(orderList[3] == &b);
		CPPUNIT_ASSERT(orderList[4] == &a);

		orderList.moveToTop(&c);
		CPPUNIT_ASSERT(orderList[0] == &c);
		CPPUNIT_ASSERT(orderList[1] == &e);
		CPPUNIT_ASSERT(orderList[2] == &d);
		CPPUNIT_ASSERT(orderList[3] == &b);
		CPPUNIT_ASSERT(orderList[4] == &a);

		orderList.moveToTop(orderList.getBottom());
		CPPUNIT_ASSERT(orderList[0] == &a);
		CPPUNIT_ASSERT(orderList[1] == &c);
		CPPUNIT_ASSERT(orderList[2] == &e);
		CPPUNIT_ASSERT(orderList[3] == &d);
		CPPUNIT_ASSERT(orderList[4] == &b);

		orderList.cyclic();
		CPPUNIT_ASSERT(orderList[0] == &c);
		CPPUNIT_ASSERT(orderList[1] == &e);
		CPPUNIT_ASSERT(orderList[2] == &d);
		CPPUNIT_ASSERT(orderList[3] == &b);
		CPPUNIT_ASSERT(orderList[4] == &a);
		CPPUNIT_ASSERT(orderList.itemList.size() == 5);

		orderList.cyclic(false);
		CPPUNIT_ASSERT(orderList[0] == &a);
		CPPUNIT_ASSERT(orderList[1] == &c);
		CPPUNIT_ASSERT(orderList[2] == &e);
		CPPUNIT_ASSERT(orderList[3] == &d);
		CPPUNIT_ASSERT(orderList[4] == &b);
		CPPUNIT_ASSERT(orderList.itemList.size() == 5);

		
		OrderList<int>::iterator itor = orderList.begin();
		++itor;
		++itor;
		// itor is &e

		orderList.moveToTop(*itor);
		CPPUNIT_ASSERT(orderList[0] == &e);
		CPPUNIT_ASSERT(orderList[1] == &a);
		CPPUNIT_ASSERT(orderList[2] == &c);
		CPPUNIT_ASSERT(orderList[3] == &d);
		CPPUNIT_ASSERT(orderList[4] == &b);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( GraphicsTest );

