#include <cppunit/extensions/helpermacros.h>
#include <math/Geometry.hpp>
#include <iostream>

using namespace geometry;

class GeometryTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(GeometryTest);
	CPPUNIT_TEST(pointTest);
	CPPUNIT_TEST(pointOpTest);
	CPPUNIT_TEST(RectangleCtorTest);
	CPPUNIT_TEST(boundaryTest);
	CPPUNIT_TEST(slideTest);
	CPPUNIT_TEST(intersectionTest);
	CPPUNIT_TEST(unionTest);
	CPPUNIT_TEST(projectionTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void projectionTest()
	{
		const Rectangle<int> source1(-50, -50, 50, 50);
		Rectangle<int> dest = source1.getProjection(Rectangle<int>(0, 0, 30, 30));
		CPPUNIT_ASSERT(dest.getLeft() == 50);
		CPPUNIT_ASSERT(dest.getTop() == 50);
		CPPUNIT_ASSERT(dest.getRight() == 80);
		CPPUNIT_ASSERT(dest.getBottom() == 80);

		dest = source1.getProjection(Rectangle<int>(-100, -100, 110, 110));
		CPPUNIT_ASSERT(dest.getLeft() == 0);
		CPPUNIT_ASSERT(dest.getTop() == 0);
		CPPUNIT_ASSERT(dest.getRight() == 100);
		CPPUNIT_ASSERT(dest.getBottom() == 100);

		dest = source1.getProjection(Rectangle<int>(-50, -50, 50, 50));
		CPPUNIT_ASSERT(dest.getLeft() == 0);
		CPPUNIT_ASSERT(dest.getTop() == 0);
		CPPUNIT_ASSERT(dest.getRight() == 100);
		CPPUNIT_ASSERT(dest.getBottom() == 100);
	}

	void unionTest()
	{
		const Rectangle<int> source1(0, 0, 10, 10);
		const Rectangle<int> source2(10, 10, 20, 20);
		const Rectangle<int> source3(11, 11, 20, 20);

		Rectangle<int> dest = source1.getUnion(source2);
		CPPUNIT_ASSERT(dest.getLeft() == 0);
		CPPUNIT_ASSERT(dest.getTop() == 0);
		CPPUNIT_ASSERT(dest.getRight() == 20);
		CPPUNIT_ASSERT(dest.getBottom() == 20);

		dest = source1.getUnion(source3);
		CPPUNIT_ASSERT(dest.getLeft() == 0);
		CPPUNIT_ASSERT(dest.getTop() == 0);
		CPPUNIT_ASSERT(dest.getRight() == 20);
		CPPUNIT_ASSERT(dest.getBottom() == 20);
	}

	void intersectionTest()
	{
		const Rectangle<int> source1(0, 0, 10, 10);
		const Rectangle<int> source2(10, 10, 20, 20);
		const Rectangle<int> source3(11, 11, 20, 20);

		Rectangle<int> dest = source1.getIntersection(source2);
		CPPUNIT_ASSERT(dest.getLeft() == 10);
		CPPUNIT_ASSERT(dest.getTop() == 10);
		CPPUNIT_ASSERT(dest.getRight() == 10);
		CPPUNIT_ASSERT(dest.getBottom() == 10);
		
		CPPUNIT_ASSERT_THROW(source1.getIntersection(source3),
							 std::invalid_argument);
	}

	void slideTest()
	{
		Rectangle<int> bounds(0, 0, 120, 240);
		Rectangle<int> testerRect(0, 0, 120, 240);
		Point<int> point(0, 0);

		CPPUNIT_ASSERT(bounds.isCollision(point));
		CPPUNIT_ASSERT(bounds.isCollision(testerRect));

		bounds.slide(120, 240);
		CPPUNIT_ASSERT(bounds.isCollision(point) == false);
		CPPUNIT_ASSERT(bounds.isCollision(testerRect));

		bounds.slide(1, 1);
		CPPUNIT_ASSERT(bounds.isCollision(point) == false);
		CPPUNIT_ASSERT(bounds.isCollision(testerRect) == false);

		bounds.slide(-121, -241);
		CPPUNIT_ASSERT(bounds.isCollision(point));
		CPPUNIT_ASSERT(bounds.isCollision(testerRect));

		CPPUNIT_ASSERT(bounds.leftTop == Point<int>(0, 0));
		CPPUNIT_ASSERT(bounds.rightBottom == Point<int>(120, 240));
	}

	void boundaryTest()
	{
		Rectangle<int> bounds(0, 0, 120, 240);
		Rectangle<int> rectangle1(0, 0, 120, 240);
		Rectangle<int> rectangle2(1, 1, 1, 1);
		Rectangle<int> rectangle3(120, 0, 120, 240);
		Rectangle<int> rectangle4(150, 120, 240, 150);

		Point<int> pos1(0, 0);
		Point<int> pos2(0, 240);
		Point<int> pos3(120, 240);
		Point<int> pos4(120, 0);
		Point<int> pos5(-1, 0);

		CPPUNIT_ASSERT(bounds.isCollision(pos1) == true);
		CPPUNIT_ASSERT(bounds.isCollision(pos2) == true);
		CPPUNIT_ASSERT(bounds.isCollision(pos3) == true);
		CPPUNIT_ASSERT(bounds.isCollision(pos4) == true);
		CPPUNIT_ASSERT(bounds.isCollision(pos5) == false);

		CPPUNIT_ASSERT(bounds.isCollision(rectangle1));
		CPPUNIT_ASSERT(bounds.isCollision(rectangle2));
		CPPUNIT_ASSERT(bounds.isCollision(rectangle3));
		CPPUNIT_ASSERT(bounds.isCollision(rectangle4) == false);

		Rectangle<int> rect1(0, 0, 100, 100);
		Rectangle<int> rect2(-50, -50, 50, 50);
		Rectangle<int> rect3(50, -50, 150, 50);
		Rectangle<int> rect4(-50, 50, 50, 150);
		Rectangle<int> rect5(50, 50, 150, 150);
		Rectangle<int> rect6(50, -50, 50, 150);

		CPPUNIT_ASSERT(rect1.isCollision(rect2));
		CPPUNIT_ASSERT(rect1.isCollision(rect3));
		CPPUNIT_ASSERT(rect1.isCollision(rect4));
		CPPUNIT_ASSERT(rect1.isCollision(rect5));

		CPPUNIT_ASSERT(rect1.isCollision(rect6));
	}

	void RectangleCtorTest()
	{
		CPPUNIT_ASSERT_THROW(Rectangle<int>(3, 2, 1, 4),
							 std::invalid_argument);
		CPPUNIT_ASSERT_THROW(Rectangle<int>(1, 4, 3, 2),
							 std::invalid_argument);

		Rectangle<int> rect(1, 1, 1, 1);
		CPPUNIT_ASSERT(rect.leftTop.x == 1);
		CPPUNIT_ASSERT(rect.leftTop.y == 1);
		CPPUNIT_ASSERT(rect.rightBottom.x == 1);
		CPPUNIT_ASSERT(rect.rightBottom.y == 1);

		Rectangle<int> rect2(1, 2, 3, 4);
		CPPUNIT_ASSERT(rect != rect2);
		CPPUNIT_ASSERT(!(rect == rect2));
		rect = rect2;
		CPPUNIT_ASSERT(rect == rect2);
		CPPUNIT_ASSERT(!(rect != rect2));

		CPPUNIT_ASSERT(rect.leftTop == Point<int>(1, 2));
		CPPUNIT_ASSERT(rect.rightBottom == Point<int>(3, 4));
	}

	void pointOpTest()
	{
		if (1) // operator== and !=
		{
			Point<int> x(1, 2);
			Point<int> y(3, 4);

			CPPUNIT_ASSERT(x == x);
			CPPUNIT_ASSERT(x != y);
		}

		if (1) // operator + and +=
		{
			Point<int> x(1, 2);
			Point<int> y(3, 4);

			CPPUNIT_ASSERT((x + y) == Point<int>(1+3, 2+4));
			x += y;
			CPPUNIT_ASSERT(x == Point<int>(1+3, 2+4));
		}

		if (1) // operator - and -=
		{
			Point<int> x(1, 2);
			Point<int> y(3, 4);

			CPPUNIT_ASSERT((x - y) == Point<int>(1-3, 2-4));
			x -= y;
			CPPUNIT_ASSERT(x == Point<int>(1-3, 2-4));
		}

		if (1) // operator * and *=
		{
			Point<int> x(1, 2);

			CPPUNIT_ASSERT((x * 5) == Point<int>(1*5, 2*5));
			x *= 5;
			CPPUNIT_ASSERT(x == Point<int>(1*5, 2*5));
		}

		if (1) // operator / and /=
		{
			Point<int> x(1, 2);

			CPPUNIT_ASSERT((x / 5) == Point<int>(1/5, 2/5));
			x /= 5;
			CPPUNIT_ASSERT(x == Point<int>(1/5, 2/5));

			CPPUNIT_ASSERT_THROW(x / 0, ZeroDivideException);
			CPPUNIT_ASSERT_THROW(x /= 0, ZeroDivideException);
		}

		if (1) // operator =
		{
			Point<int> x(1, 2);
			Point<int> y(3, 4);

			CPPUNIT_ASSERT(x != y);
			x = y;
			CPPUNIT_ASSERT(x == y);
		}
	}

	void pointTest()
	{
		if (1) 
		{
			Point<int> pos;

			CPPUNIT_ASSERT(pos.getX() == 0);
			CPPUNIT_ASSERT(pos.getY() == 0);

			pos.setX(1);
			CPPUNIT_ASSERT(pos.getX() == 1);
			pos.setY(5);
			CPPUNIT_ASSERT(pos.getY() == 5);
		}

		if (1)
		{
			Point<int> pos(10, -20);

			CPPUNIT_ASSERT(pos.getX() == 10);
			CPPUNIT_ASSERT(pos.getY() == -20);
		}
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( GeometryTest );
