#ifndef CALICULATE_HPP_
#define CALICULATE_HPP_

template <class Type>
Type Max(Type lhs, Type rhs)
{
	return lhs > rhs ? lhs : rhs;
}

template <typename Type>
Type Min(Type lhs, Type rhs)
{
	return lhs < rhs ? lhs : rhs;
}
#endif /* CALICULATE_HPP_ */
