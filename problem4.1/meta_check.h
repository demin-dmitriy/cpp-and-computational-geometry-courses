
//is<a, b>::equal := (a == b)
template<class a, class b> 
struct is
{
	static const bool equal = false;		
};

template<class a>
struct is<a, a>
{
	static const bool equal = true;
};

//condition == true => ctIF<...>::res := a; else ctIF<...>::res := b
template<bool condition, class a, class b>
struct ctIF;

template<class a, class b>
struct ctIF<true, a, b>
{
	typedef a res;
};

template<class a, class b>
struct ctIF<false, a, b>
{
	typedef b res;
};