#include <random>
#include <cmath>
#include <ratio>

#define MIN(x,y) ((x) < (y) ? (x) : (y))


struct _rng_
{
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<> uniform_dist;

	_rng_() : rd(), gen(rd()), uniform_dist(0,1) { }

	inline double operator()() { return uniform_dist(gen); }
};

static _rng_ _skip_list_rng;

template<intmax_t ML>
struct _level_gen
{
	const double p_nlevel;
	constexpr
	_level_gen(const double &_p) : p_nlevel(_p) { }

	int randomLevel()
	{
		int newLevel = 1;
		while( _skip_list_rng() < p_nlevel && newLevel < ML)
			++newLevel;
		
		return newLevel;
	}
};


