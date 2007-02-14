#ifndef PREDICATE_HPP_
#define PREDICATE_HPP_

namespace Predicate
{
	template <typename T>
	struct Remover
	{
		void operator()(T* pointer)
		{
			delete pointer;
		}
	};

	template <typename T>
	struct ArrayRemover
	{
		void operator()(T* pointer)
		{
			delete[] pointer;
		}
	};
};

#endif /* PREDICATE_HPP_ */
