#include <climits>
namespace sort_list
{
	//number list
	template<int num, class L>
	struct list
	{
		typedef L tail;
		enum
		{
			value = num
		};
	};

	struct null;

	//maximum in interval [a, b]
	template <class L, int a, int b>
	struct interval
	{
		enum
		{
			res = interval<L::tail, ((b > L::value && L::value > a) ? L::value : a), b>::res
		};
	};

	template<int a, int b>
	struct interval<null, a, b>
	{
		enum
		{
			res = a
		};
	};


	//minimum in list
	template<class L, int n>
	struct min
	{
		enum
		{
			res = min<L::tail, ((n < L::value) ? n : L::value)>::res
		};
	};

	template<int n>
	struct min<null, n>
	{
		enum
		{
			res = n
		};
	};

	//maximum in L less than limit
	template<class L, int limit>
	struct maxless
	{
		enum
		{
			res = interval<L, INT_MIN, limit>::res
		};
	};

	template <class L>
	struct sort
	{
		template <class L, int limit, int stop>
		struct sort_help
		{
			enum
			{
				sub_max = maxless<L, limit>::res,
		
			};
			typedef typename sort_help<typename L, sub_max, stop>::res tmp;
			typedef list<sub_max, tmp> res; 
		};

		template <class L, int stop>
		struct sort_help<L, stop, stop>
		{
			typedef null res; 
		};
		typedef sort_help<L, INT_MAX, min<L, INT_MAX>::res > res;
	};

	//индексация
	template <class L, int index>
	struct at
	{
		enum
		{
			res = at<L::tail, index - 1>::res
		};
	};

	template <class L>
	struct at<L, 0>
	{
		enum
		{
			res = L::value
		};
	};

	//размер
	template <class L>
	struct size
	{
		template <class L, int n>
		struct size_help
		{
			enum
			{
				res = size_help<L::tail, n + 1>::res
			};
		};

		template <int n>
		struct size_help<null, n>
		{
			enum
			{
				res = n + 1
			};
		};

		enum
		{
			res = size_help<L::tail, 0>::res
		};
	};
}

