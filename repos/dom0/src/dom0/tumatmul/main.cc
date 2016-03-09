#include <list>
#include <cmath>
#include <algorithm>
#include <string>
#include <base/printf.h>
#include <os/config.h>

int main(void)
{
	PINF("tumatmul: Hello!\n");

	// Sieve of Erathosthenes
	unsigned int max = 0;
	const Genode::Xml_node& configNode = Genode::config()->xml_node();
	configNode.sub_node("arg1").value<unsigned int>(&max);
	unsigned int sqrt = static_cast<unsigned int>(std::sqrt(max));

	PINF("Calculating primes up to %d.", max);
	std::list<unsigned int> primes;
	for (unsigned int i = 2; i <= max; ++i)
	{
		primes.push_back(i);
	}

	for (auto tester = primes.cbegin(); *tester <= sqrt; ++tester)
	{
		auto testee = tester;
		++testee;
		for (; testee != primes.cend(); ++testee)
		{
			if (*testee % *tester == 0)
			{
				primes.erase(testee);
			}
		}
	}

	std::string primesStr;

	auto it = primes.crbegin();
	for (size_t i = 0; i <= 5 && it != primes.crend(); ++i, ++it)
	{
		primesStr += std::to_string(*it) + ' ';
	}
	PINF("Last 5 primes up to %d: %s", max, primesStr.c_str());
	PINF("Calculated %d primes in total.", primes.size());

	return 0;
}
