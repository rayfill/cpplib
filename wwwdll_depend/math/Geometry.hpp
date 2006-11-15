#ifndef GEOMETRY_HPP_
#define GEOMETRY_HPP_

#include <stdexcept>
#include <ostream>
#include <math/ArithmeticException.hpp>

namespace geometry
{
	template <typename UnitType = int>
	class Point 
	{
		friend class GeometryTest;

	public:
		typedef UnitType BaseType;

	private:
		UnitType x;
		UnitType y;

	public:
		Point():
			x(0), y(0)
		{}

		Point(const UnitType& x_, const UnitType& y_):
			x(x_), y(y_)
		{}

		Point(const Point& source):
			x(source.x), y(source.y)
		{}

		bool operator==(const Point& source) const
		{
			if (this->x == source.x &&
				this->y == source.y)
				return true;

			return false;
		}

		bool operator!=(const Point& source) const
		{
			return ! this->operator==(source);
		}

		Point& operator=(const Point& source)
		{
			if (this != &source)
			{
				this->x = source.x;
				this->y = source.y;
			}

			return *this;
		}

		Point& operator+=(const Point& source)
		{
			this->x += source.x;
			this->y += source.y;

			return *this;
		}

		Point operator+(const Point& source) const
		{
			return Point(this->x + source.x, this->y + source.y);
		}

		Point& operator-=(const Point& source)
		{
			this->x -= source.x;
			this->y -= source.y;

			return *this;
		}

		Point operator-(const Point& source) const
		{
			return Point(this->x - source.x, this->y - source.y);
		}

		Point& operator*=(const BaseType& multiply)
		{
			this->x *= multiply;
			this->y *= multiply;

			return *this;
		}

		Point operator*(const BaseType& source) const
		{
			return Point(this->x * source, this->y * source);
		}

		Point& operator/=(const BaseType& divisor) throw(ZeroDivideException)
		{
			if (divisor == BaseType(0))
				throw ZeroDivideException();

			this->x /= divisor;
			this->y /= divisor;

			return *this;
		}

		Point operator/(const BaseType& divisor) const throw(ZeroDivideException)
		{
			if (divisor == BaseType(0))
				throw ZeroDivideException();

			return Point(this->x / divisor, this->y / divisor);
		}

		~Point() throw()
		{}

		UnitType getX() const
		{
			return x;
		}

		UnitType getY() const
		{
			return y;
		}

		void setX(const UnitType& newX)
		{
			x = newX;
		}

		void setY(const UnitType& newY)
		{
			y = newY;
		}
	};

	template <typename UnitType = int>
	class Rectangle
	{
		friend class GeometryTest;

	public:
		typedef UnitType BaseType;

	private:
		Point<BaseType> leftTop;
		Point<BaseType> rightBottom;

	public:
		Rectangle() throw()
			: leftTop(), rightBottom()
		{}

		Rectangle(const BaseType& left, const BaseType& top,
				  const BaseType& right, const BaseType& bottom)
			throw(std::invalid_argument):
			leftTop(left, top), rightBottom(right, bottom)
		{
			if (left > right ||
				top > bottom)
				throw std::invalid_argument("argument invalid.");
		}

		Rectangle(const Point<BaseType>& leftTop_,
				  const Point<BaseType>& rightBottom_)
			throw(std::invalid_argument):
			leftTop(leftTop_), rightBottom(rightBottom_)
		{
			if (leftTop.getX() > rightBottom.getX() ||
				leftTop.getY() > rightBottom.getY())
				throw std::invalid_argument("argument invalid.");
		}

		Rectangle(const Rectangle& source) throw():
			leftTop(source.leftTop), rightBottom(source.rightBottom)
		{}

		Rectangle& operator=(const Rectangle& source) throw()
		{
			if (this != &source)
			{
				this->leftTop = source.leftTop;
				this->rightBottom = source.rightBottom;
			}

			return *this;
		}

		~Rectangle() throw()
		{}

		UnitType getTop() const throw()
		{
			return this->leftTop.getY();
		}

		UnitType getLeft() const throw()
		{
			return this->leftTop.getX();
		}

		Point<UnitType> getLeftTop() const throw()
		{
			return this->leftTop;
		}

		UnitType getBottom() const throw()
		{
			return this->rightBottom.getY();
		}

		UnitType getRight() const throw()
		{
			return this->rightBottom.getX();
		}

		Point<UnitType> getRightBottom() const throw()
		{
			return this->rightBottom;
		}

		Rectangle& operator+=(const Rectangle& source) throw()
		{
			if (this->leftTop.getX() > source.leftTop.getX())
				this->leftTop.getX(source.leftTop.getX());

			if (this->leftTop.getY() > source.leftTop.getY())
				this->leftTop.getY(source.leftTop.getY());

			if (this->rightBottom.getX() > source.rightBottom.getX())
				this->rightBottom.getX(source.rightBottom.getX());

			if (this->rightBottom.getY() > source.rightBottom.getY())
				this->rightBottom.setY(source.rightBottom.getY());

			return *this;
		}

		Rectangle operator+(const Rectangle& source) const throw()
		{
			return Rectangle(*this) += source;
		}

		Rectangle& operator+=(const Point<UnitType>& source) throw()
		{
			this->leftTop.setX(this->leftTop.getX() + source.getX());
			this->leftTop.setY(this->leftTop.getY() + source.getY());
			this->rightBottom.setX(this->rightBottom.getX() + source.getX());
			this->rightBottom.setY(this->rightBottom.getY() + source.getY());

			return *this;
		}

		Rectangle operator+(const Point<UnitType>& source) const throw()
		{
			return Rectangle(*this) += source;
		}

		bool operator==(const Rectangle& source) const throw()
		{
			return (this->leftTop == source.leftTop &&
					this->rightBottom == source.rightBottom);
		}

		bool operator!=(const Rectangle& source) const throw()
		{
			return !((*this) == source);
		}

		Rectangle& slide(const BaseType& xMove, const BaseType& yMove) throw()
		{
			this->leftTop += Point<BaseType>(xMove, yMove);
			this->rightBottom += Point<BaseType>(xMove, yMove);

			return *this;
		}

		Rectangle& slide(const Point<BaseType>& move) throw()
		{
			this->leftTop += move;
			this->rightBottom += move;

			return *this;
		}

		Rectangle slide(const BaseType& xMove, const BaseType& yMove) const throw()
		{
			return Rectangle(*this).slide(xMove, yMove);
		}

		Rectangle slide(const Point<BaseType>& move) const throw()
		{
			return this->slide(move.getX(), move.getY());
		}

		// on the edge is inner.
		bool isCollision(const Point<BaseType>& position) const throw()
		{
			return 
				(this->leftTop.getX() <= position.getX() &&
				 this->leftTop.getY() <= position.getY() &&
				 this->rightBottom.getX() >= position.getX() &&
				 this->rightBottom.getY() >= position.getY());
		}

		bool isCollision(const Rectangle& source) const throw()
		{
			const BaseType left =
				this->getLeft() > source.getLeft() ? this->getLeft() : source.getLeft();
			const BaseType top =
				this->getTop() > source.getTop() ? this->getTop() : source.getTop();
			const BaseType right =
				this->getRight() < source.getRight() ? this->getRight() : source.getRight();
			const BaseType bottom = 
				this->getBottom() < source.getBottom() ? this->getBottom() : source.getBottom();

			if (left > right ||
				top > bottom)
				return false;

			return true;
		}

		Rectangle getProjection(const Rectangle& target) const
		{
			Rectangle conflictArea = this->getIntersection(target);
			conflictArea.slide(-1 * this->getLeft(), -1 * this->getTop());

			return conflictArea;
		}

		Rectangle getUnion(const Rectangle& target) const
		{
			typedef UnitType number_t;

			return Rectangle(
				this->getLeft() < target.getLeft() ?
				this->getLeft() : target.getLeft(),
				this->getTop() < target.getTop() ?
				this->getTop() : target.getTop(),
				this->getRight() > target.getRight() ?
				this->getRight() : target.getRight(),
				this->getBottom() > target.getBottom() ?
				this->getBottom() : target.getBottom());
		}

		Rectangle getIntersection(const Rectangle& target)	const
		{
			typedef UnitType number_t;

			return Rectangle(
				this->getLeft() > target.getLeft() ?
				this->getLeft() : target.getLeft(),
				this->getTop() > target.getTop() ?
				this->getTop() : target.getTop(),
				this->getRight() < target.getRight() ?
				this->getRight() : target.getRight(),
				this->getBottom() < target.getBottom() ?
				this->getBottom() : target.getBottom());
		}

		UnitType getWidth() const throw()
		{
			return this->getRight() - this->getLeft();
		}

		UnitType getHeight() const throw()
		{
			return this->getBottom() - this->getTop();
		}

		friend std::ostream& 
		operator<<(std::ostream& out, const Rectangle<UnitType>& self)
		{
			return 
				out << "(" <<
				self.getLeft() << ", " <<
				self.getTop() << ", " <<
				self.getRight() << ", " <<
				self.getBottom() << ")";
		}
	};
}
#endif /* GEOMETRY_HPP_ */
