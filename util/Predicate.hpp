#ifndef PREDICATE_HPP_
#define PREDICATE_HPP_

#include <algorithm>

namespace Predicate
{
	class IgnoreCaseComparator
	{
	private:
		static const std::string lowercase(const std::string& value)
		{
			std::string result(value);

			for (std::string::iterator itor = result.begin();
				 itor != result.end(); ++itor)
			{
				if (*itor >= 'A' &&
					*itor <= 'Z')
					*itor += ('a' - 'A'); // lower case.
			}
			return result;
		}
	public:
		bool operator()(const std::string& lhs, const std::string& rhs)
		{
			return std::less<std::string>()(lowercase(lhs), lowercase(rhs));
		}
	};

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
