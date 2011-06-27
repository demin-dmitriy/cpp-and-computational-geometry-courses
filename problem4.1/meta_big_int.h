#include "meta_list.h"

#define make_num1(negative, a1)      meta_big_int<make_list1(a1), negative>
#define make_num2(negative, a1, a2)  meta_big_int<make_list2(a1, a2), negative>
#define make_num3(negative, a1, a2, a3)  meta_big_int<make_list3(a1, a2, a3), negative>
#define make_num4(negative, a1, a2, a3, a4)  meta_big_int<make_list4(a1, a2, a3, a4), negative>
#define make_num5(negative, a1, a2, a3, a4, a5)  meta_big_int<make_list5(a1, a2, a3, a4, a5), negative>
#define make_num6(negative, a1, a2, a3, a4, a5, a6)  meta_big_int<make_list6(a1, a2, a3, a4, a5, a6), negative>
#define make_num7(negative, a1, a2, a3, a4, a5, a6, a7)  meta_big_int<make_list7(a1, a2, a3, a4, a5, a6, a7), negative>
#define make_num8(negative, a1, a2, a3, a4, a5, a6, a7, a8)  meta_big_int<make_list8(a1, a2, a3, a4, a5, a6, a7, a8), negative>
#define make_num9(negative, a1, a2, a3, a4, a5, a6, a7, a8, a9)  meta_big_int<make_list9(a1, a2, a3, a4, a5, a6, a7, a8, a9), negative>
#define make_num10(negative, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) meta_big_int<make_list10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10), negative>


namespace meta_big_int_names
{
	using namespace meta_list_names;
	const int base = 10;

	template<class digit_list, bool sign_negative>
	struct meta_big_int
	{
		typedef digit_list digits; //digits are in reverse order
		static const bool negative = sign_negative;
	};

	//-------------------------------------------------
	template<class a, class b> // a < b => -1; a == b => 0; a > b => 1. Will not check sign field.
	struct compare_abs 
	{
	private:
		struct smaller
		{
			enum
			{
				val = -1
			};
		};
		struct equal
		{
						enum
		{
			val = 0
		};
		};
		struct bigger
		{
			enum
			{
				val = +1
			};
		};

		template<class list_a, class list_b, class last_not_equal>
		struct helper
		{
			typedef typename ctIF<
			//if 
			(list_a::val) == (list_b::val),
			//then
				last_not_equal,
			//else
				typename ctIF<
				//if
				(list_a::val) < (list_b::val),
				//then
					smaller,
				//else
					bigger
				>::res
			>::res current;

			typedef typename helper<typename list_a::prev, typename list_b::prev, current>::res res;  
		};

		template<class a, class last_not_equal>
		struct helper<a, null, last_not_equal>
		{
			typedef bigger res;
		};
		template<class b, class last_not_equal>
		struct helper<null, b, last_not_equal>
		{
			typedef smaller res;
		};
		template<class last_not_equal>
		struct helper<null, null, last_not_equal>
		{
			typedef last_not_equal res;
		};

	public:
		enum
		{
			res = helper<a::digits, b::digits, equal>::res::val
		};
	};

	//-------------------------------------------------
	template<class a, class b> //sum_abs<a, b>::res := |a| + |b|
	struct sum_abs //doesn't check sign field
	{
	private:
		typedef typename ctIF<(info<typename a::digits>::size) < (info<typename b::digits>::size), a, b>::res small;
		typedef typename ctIF<(info<typename a::digits>::size) >= (info<typename b::digits>::size), a, b>::res big;
		static const int size_small = info<small::digits>::size;
		static const int size_big = info<big::digits>::size;
	
		template<int pos, int carry, int size_big>
		struct helper_carry
		{
			static const int tmp = at<big::digits, pos>::val + carry;
			typedef meta_list<typename helper_carry<pos + 1, tmp / base, size_big>::res, tmp % base> res;
		};

		template<int carry, int size_big>
		struct helper_carry<size_big, carry, size_big>
		{
			typedef typename ctIF<carry != 0, meta_list<null, carry>, null>::res res;
		};

		template<int pos, int carry, int size_small>
		struct helper
		{
			static const int tmp = at<small::digits, pos>::val + at<big::digits, pos>::val + carry;
			typedef meta_list<typename helper<pos + 1, tmp / base, size_small>::res ,tmp % base> res;
		};

		template<int carry, int size_small>
		struct helper<size_small, carry, size_small>
		{
			typedef typename helper_carry<size_small, carry, size_big>::res res;
		};

	public:
		typedef meta_big_int<typename helper<0, 0, size_small>::res, a::negative> res;
	};

	//-------------------------------------------------
	template<class a, class b> //subtract<a, b>::res := |a| - |b|
	struct subtract_abs //doesn't check sign field
	{
	private:
		typedef typename ctIF<(info<typename a::digits>::size) < (info<typename b::digits>::size), a, b>::res small;
		typedef typename ctIF<(info<typename a::digits>::size) >= (info<typename b::digits>::size), a, b>::res big;
		static const int size_small = info<small::digits>::size;
		static const int size_big = info<big::digits>::size;
	
		template<int pos, int carry, int size_big>
		struct helper_carry
		{
			static const int tmp = base + at<big::digits, pos>::val - carry;
			typedef typename helper_carry<pos + 1, ((tmp < base) ? (1) : (0)), size_big>::res tail;
			typedef typename ctIF<is<tail, null>::equal && tmp % base == 0, null, meta_list<tail, tmp % base>>::res res;
		};

		template<int carry, int size_big>
		struct helper_carry<size_big, carry, size_big>
		{
			typedef typename ctIF<carry != 0, meta_list<null, carry>, null>::res res;
		};

		template<int pos, int carry, int size_small>
		struct helper
		{
	
			static const int tmp = base + at<big::digits, pos>::val - at<small::digits, pos>::val - carry;
			typedef typename helper<pos + 1, ((tmp < base) ? (1) : (0)), size_small>::res tail;
			typedef typename ctIF<(is<tail, null>::equal && tmp % base == 0), null,  meta_list<tail, tmp % base>>::res res;
		};

		template<int carry, int size_small>
		struct helper<size_small, carry, size_small>
		{
			typedef typename helper_carry<size_small, carry, size_big>::res res;
		};

	public:
		typedef meta_big_int<typename helper<0, 0, size_small>::res, a::negative> res; //здесь надо исправить растановку знака
	};

	//-------------------------------------------------
	template<class a, class b> //sum<a, b>::res := a + b
	struct sum
	{
		typedef typename ctIF<
		//if
		(a::negative == b::negative),
		//then
			typename sum_abs<a, b>::res,
		//else
			typename ctIF<
			//if
			(compare_abs<a, b>::res == -1),
			//then
				meta_big_int<typename subtract_abs<b, a>::res::digits, b::negative>,
			//else
				meta_big_int<typename subtract_abs<a, b>::res::digits, a::negative>
			>::res
		>::res res;
	};

	//-------------------------------------------------
	template<class a, class b> //subtract<a, b>::res := a - b
	struct subtract
	{
		typedef typename sum<a, meta_big_int<typename b::digits, !b::negative>>::res res;
	};

	//-------------------------------------------------
	template<class a, class b> //mult<a, b>::res := a * b
	struct mult
	{
	private:
		template<class list, int digit, int list_size, int pos = 0, int carry = 0>
		struct mult_digit
		{
			static const int tmp = at<list, pos>::val * digit + carry;
			typedef meta_list<typename mult_digit<list, digit, list_size, pos + 1, tmp / base>::res, tmp % base> tmp_res;
			typedef typename ctIF<digit == 0, meta_list<null, 0>, tmp_res>::res res;
		};

		template<class list, int carry, int digit, int list_size>
		struct mult_digit<list, digit, list_size, list_size, carry>
		{
			typedef typename ctIF<carry == 0, null, meta_list<null, carry>>::res res;
		};

		static const int size_b = info<b::digits>::size;
		static const int size_a = info<a::digits>::size;
		template<int pos, class current_res, int size_b>
		struct helper
		{
			typedef typename mult_digit<typename a::digits, at<typename b::digits, pos>::val, size_a>::res tmp_summand;
			typedef typename add_zeroes<tmp_summand, pos>::res summand;
			typedef typename sum<meta_big_int<current_res, false>, meta_big_int<summand, false>>::res::digits tmp_res;
			typedef typename helper<pos + 1, tmp_res, size_b>::res res;
		};

		template<class current_res, int size_b>
		struct helper<size_b, current_res, size_b>
		{
			typedef current_res res;
		};

	public:
		typedef typename meta_big_int<typename helper<0, meta_list<null, 0>, size_b>::res, (a::negative ^ b::negative)> res; //исправить растоновку знака
	};

	//-------------------------------------------------
	template<class a, class b> //divmod<a, b>::div := a / b; divmod<a, b>::mid := a % b 
	struct divmod
	{
	private:
		template<class devidend, class devider, int quot>
		struct simple_div
		{
			typedef typename mult<devider, meta_big_int<meta_list<null, quot>, false>>::res mult_tmp;
			static const bool stop = (compare_abs<mult_tmp, devidend>::res == 1);
			enum
			{
				res = (stop) ? (quot - 1) : (simple_div<devidend, devider, quot + 1>::res)
			};
		};

		template<class devidend, class devider>
		struct simple_div<devidend, devider, base>
		{
			enum
			{
				res = base - 1
			};
		};

		template<class rest_list, int pos>
		struct helper
		{
			typedef typename push_front<rest_list, at<typename a::digits, pos>::val>::res new_rest1; //just one more digit from devidend
			enum
			{
				current_q = simple_div<meta_big_int<new_rest1, false>, b, 0>::res
			};
			typedef typename subtract<meta_big_int<new_rest1, false>, typename mult<b, meta_big_int<meta_list<null, current_q>, false> >::res>::res::digits new_rest2; //will be transfed to next iteration
			typedef meta_list<typename helper<new_rest2, pos - 1>::div, current_q> div;
			typedef typename helper<new_rest2, pos - 1>::mod mod;
		};

		template<class rest_list>
		struct helper<rest_list, -1>
		{
			typedef rest_list mod;
			typedef null div;
		};

		template<class a, int pos>
		struct reverse
		{
			
			typedef typename reverse<a, pos - 1>::res tmp;
			typedef typename ctIF<
			//if
			(is<tmp, null>::equal && at<a, pos>::val == 0),
			//then
				null,
			//else
				meta_list<tmp, at<a, pos>::val>
			>::res res ;
		};

		template<class a>
		struct reverse<a, -1>
		{
			typedef null res;
		};

		typedef typename helper<null, info<typename a::digits>::size - 1>::div reversed_div;
		typedef typename reverse<reversed_div, info<reversed_div>::size - 1>::res normal_div;
	public:
		typedef meta_big_int<typename ctIF<is<normal_div, null>::equal, meta_list<null, 0>, normal_div>::res, (a::negative ^ b::negative)> div;
		typedef meta_big_int<typename helper<null, info<typename a::digits>::size - 1>::mod, (a::negative)> mod;
	};
}