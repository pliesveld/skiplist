#include <random>
#include <cmath>
#include <ratio>

#define MIN(x,y) ((x) < (y) ? (x) : (y))

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> uniform_dist(0,1);

template<intmax_t T1,intmax_t T2>
int randomLevel(int n, const std::ratio<T1,T2> &r)
{
	
	int newLevel = 1;
	while(uniform_dist(gen) < (double)r.num / r.den && newLevel < n)
		++newLevel;

	return MIN(newLevel,n);
}


