#include <iostream>
#include "geometry.h"

using namespace std;

class random_generator{
   int seed;

   static const int a = 1664525;
   static const int c = 1013904223;
public:
   random_generator(int seed) 
      :seed(seed)
   {}

   // return random integer in range 0 .. 2^31-1 
   int get_int() 
   {
      return (seed = seed * a + c) & 0x7fffffff;
   }

   double get_double() 
   {
      const double lo = -1000;
      const double hi = 1000;
      const double scale = 1. / (1 << 30);
      return (lo + (hi - lo) * scale * (get_int() & 0x3fffffff));
   }
};

int main(int argc, char ** argv)
{   
	int n, type;
	
	cin >> n >> type;
	int res = 0;
	int p = 1;

	if (type == 1) {
		int seed;
		cin >> seed;
		random_generator rand(seed);
		for (int i = 0; i < n; ++i, p *= 239)
		{
			double x1 = rand.get_double();
			double y1 = rand.get_double();
			double x2 = rand.get_double();
			double y2 = rand.get_double();
			double x3 = rand.get_double();
			double y3 = rand.get_double();
			double x4 = rand.get_double();
			double y4 = rand.get_double();
			
			if (is_intersect(point(x1, y1), point(x2, y2), point(x3, y3), point(x4, y4)))
			{
				res += p;
			}
		}
	}
	else
	{	
		for (int i = 0; i < n; ++i, p *= 239)
		{
			double x1, y1, x2, y2, x3, y3, x4, y4;
			cin >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> x4 >> y4;
			if (is_intersect(point(x1, y1), point(x2, y2), point(x3, y3), point(x4, y4)))
			{
				res += p;
			}
	   }
   }
   cout << res << "\n";
   return 0;
}
