#include "meta_ctassert.h"
#include "meta_check.h"

#define make_list1(a1) meta_list<null, a1>
#define make_list2(a1, a2) meta_list<make_list1(a1), a2>
#define make_list3(a1, a2, a3) meta_list<make_list2(a1, a2), a3>
#define make_list4(a1, a2, a3, a4) meta_list<make_list3(a1, a2, a3), a4>
#define make_list5(a1, a2, a3, a4, a5) meta_list<make_list4(a1, a2, a3, a4), a5>
#define make_list6(a1, a2, a3, a4, a5, a6) meta_list<make_list5(a1, a2, a3, a4, a5), a6>
#define make_list7(a1, a2, a3, a4, a5, a6, a7) meta_list<make_list6(a1, a2, a3, a4, a5, a6), a7>
#define make_list8(a1, a2, a3, a4, a5, a6, a7, a8) meta_list<make_list7(a1, a2, a3, a4, a5, a6, a7), a8>
#define make_list9(a1, a2, a3, a4, a5, a6, a7, a8, a9) meta_list<make_list8(a1, a2, a3, a4, a5, a6, a7, a8), a9>
#define make_list10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) meta_list<make_list9(a1, a2, a3, a4, a5, a6, a7, a8, a9), a10>


namespace meta_list_names
{
	template<class tail_list, int value>
	struct meta_list
	{
		typedef tail_list prev;
		enum
		{
			val = value
		};
	};

	struct null;

	template<class list, int index>
	struct at
	{
		ctassert<!is<list, null>::equal> array_index_out_of_bounds;
		enum
		{
			val = at<list::prev, index - 1>::val
		};
		typedef typename at<typename list::prev, index - 1>::element element;
	};

	template<class list>
	struct at<list, 0>
	{
		ctassert<!is<list, null>::equal> array_index_out_of_bounds;
		enum
		{
			val = list::val
		};
		typedef list element;
	};

	//get size:
	template<class list>
	struct info
	{
	private:
		template<class L, int size>
		struct helper
		{
			enum
			{
				res = helper<L::prev, size + 1>::res
			};
		};

		template<int size>
		struct helper<null, size>
		{
			enum
			{
				res = size
			};
		};

	public:
		enum
		{
			size = helper<list, 0>::res
		};
	};

	template<class list, int value>
	struct push_front
	{
		typedef meta_list<list, value> res;
	};

	template<class a, int count>
	struct add_zeroes
	{
		typedef typename push_front<typename add_zeroes<a, count - 1>::res, 0>::res res;
	};

	template<class a>
	struct add_zeroes<a, 0>
	{
		typedef a res;
	};
}