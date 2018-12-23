#include <algorithm>    // nth_element
#include <array>        // array
#include <chrono>       // high_resolution_clock
#include <iomanip>      // setw
#include <iostream>     // cout
#include <vector>       // vector

using namespace std::chrono;

const unsigned long long MINSIZE = 1ULL << 10; // minimun line size to test:  1KB
const unsigned long long MAXSIZE = 1ULL << 34; // maximun line size to test: 16GB
const unsigned GAP = 12;          // gap for cout columns
const unsigned REP = 3;         // number of repetitions of every test
const unsigned long long STEPS = 1e7;       // steps

int main()
{
	std::cout << "#" 
	          << std::setw(GAP - 1) << "line (B)"
	          << std::setw(GAP    ) << "time (µs)"
	          << std::endl;

	for (unsigned long long size = MINSIZE; size <= MAXSIZE; size *= 2)
	{
		std::vector<duration<double, std::micro>> score(REP);

		for (auto &s: score)
		{
			std::vector<char> bytes(size);

			auto start = high_resolution_clock::now();
			unsigned long long uno=1;
			unsigned long long num_iter_vector = std::max(STEPS*64/size,uno);
			for (unsigned long long i = 0; i < num_iter_vector; ++i)
				for(unsigned long long j=0; j < size; j+=64)
					bytes[j]++;

			// for (unsigned i = 0; i < STEPS; ++i)
			// 	bytes[(i * 64) & (size - 1)]++;

			auto stop = high_resolution_clock::now();

			s = stop - start;
		}

		std::nth_element(score.begin(), 
		                 score.begin() + score.size() / 2, 
		                 score.end());

		std::cout << std::setw(GAP) << size
		          << std::setw(GAP) << std::fixed << std::setprecision(1)
		          << std::setw(GAP) << score[score.size() / 2].count()
		          << std::endl;
	}
}

