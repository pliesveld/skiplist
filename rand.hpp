#include <random>
#include <cmath>
#include <ratio>

#define MIN(x,y) ((x) < (y) ? (x) : (y))

template<intmax_t T1,intmax_t T2>
int randomLevel(int n, const std::ratio<T1,T2> &r)
{
	std::random_device rd;
	std::uniform_real_distribution<> uniform_dist(0,1);
	
	int newLevel = 1;
	while(uniform_dist(rd) < (double)r.num / r.den)
		++newLevel;

	return MIN(newLevel,n);
}


