#include "meta_big_int.h"
#define get(x, i) at<typename x::digits, i>::val

using namespace meta_big_int_names;

typedef make_num5(false, 4, 2, 7, 9, 1) a;

typedef make_num2(false, 2, 7) small;

typedef make_num9(false, 9, 8, 8, 4, 2, 5, 6, 8, 4) big;

//test sum:
typedef sum<a, small>::res sum1;
typedef sum<small, a>::res sum2;
typedef sum<big, a>::res sum3;

//42791 + 27 = 42818
const int sum1res[] = {get(sum1, 0), get(sum1, 1), get(sum1, 2), get(sum1, 3), get(sum1, 4)};
const int sum2res[] = {get(sum2, 0), get(sum2, 1), get(sum2, 2), get(sum2, 3), get(sum2, 4)};
//988425684 + 42791 = 988468475
const int sum3res[] = {get(sum3, 0), get(sum3, 1), get(sum3, 2), get(sum3, 3), get(sum3, 4), get(sum3, 5), get(sum3, 6), get(sum3, 7), get(sum3, 8)};
//ok

//test subtract
typedef subtract<a, small>::res subtract1;
typedef subtract<small, a>::res subtract2;
typedef subtract<big, a>::res subtract3;
//42791 - 27 = 42764
const int subtract1res[] = {get(subtract1, 0), get(subtract1, 1), get(subtract1, 2), get(subtract1, 3), get(subtract1, 4)};
const bool subtract1sign = subtract1::negative;
//27 - 42791 = -42764
const int subtract2res[] = {get(subtract2, 0), get(subtract2, 1), get(subtract2, 2), get(subtract2, 3), get(subtract2, 4)};
const bool subtract2sign = subtract2::negative;
//988425684 - 42791 = 988382893
const int subtract3res[] = {get(subtract3, 0), get(subtract3, 1), get(subtract3, 2), get(subtract3, 3), get(subtract3, 4), get(subtract3, 5), get(subtract3, 6), get(subtract3, 7), get(subtract3, 8)};
const bool subtract3sign = subtract3::negative;
//ok

//test mult:
typedef mult<a, small>::res mult1;
typedef mult<small, a>::res mult2;
typedef mult<big, a>::res mult3;

//42791 * 27 = 1155357
const int mult1res[] = {get(mult1, 0), get(mult1, 1), get(mult1, 2), get(mult1, 3), get(mult1, 4), get(mult1, 5), get(mult1, 6)};
const int mult2res[] = {get(mult2, 0), get(mult2, 1), get(mult2, 2), get(mult2, 3), get(mult2, 4), get(mult2, 5), get(mult2, 6)};
//988425684 * 42791 = 42295723444044
const int mult3res[] = {get(mult3, 0), get(mult3, 1), get(mult3, 2), get(mult3, 3), get(mult3, 4), get(mult3, 5), get(mult3, 6), get(mult3, 7), get(mult3, 8), get(mult3, 9), get(mult3, 10), get(mult3, 11), get(mult3, 12), get(mult3, 13)};
//ok

//test divmod:
typedef divmod<a, small>::div divmod1;
typedef divmod<small, a>::div divmod2;
typedef divmod<big, a>::div divmod3;

//42791 / 27 = 1584
const int divmod1res[] = {get(divmod1, 0), get(divmod1, 1), get(divmod1, 2), get(divmod1, 3)};
//27 / 42791 = 0
const int divmod2res[] = {get(divmod2, 0)};
//988425684 / 42791 = 23098
const int divmod3res[] = {get(divmod3, 0), get(divmod3, 1), get(divmod3, 2), get(divmod3, 3), get(divmod3, 4)};
//ok

int main()
{
	return 0;
}