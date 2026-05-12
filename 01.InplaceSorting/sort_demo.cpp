#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "collvalue.h"
#include "sorting.h"

using CollInt = CollectingValue<int>;

// Random data
std::vector<CollInt> generate_data(size_t n)
{
    std::vector<CollInt> data(n);
    std::iota(data.begin(), data.end(), CollInt(0));
    return data;
}

void shuffle_data(std::vector<CollInt>& data)
{
    std::shuffle(data.begin(), data.end(), std::mt19937{std::random_device{}()});
}

template <typename Sorter>
bool run_sort_case(size_t n, const std::string& algo, Sorter sorter)
{
    auto number_data = generate_data(n);
    shuffle_data(number_data);
    CollInt::reset_stats();
    sorter(number_data.begin(), number_data.end());

    if (!std::is_sorted(number_data.begin(), number_data.end()))
    {
        std::cerr << algo << " failed to sort " << n << " values\n";
        return false;
    }

    std::cout << n << "\t" << std::left << std::setw(10) << algo
              << "\t" << CollInt::comps
              << "\t\t" << CollInt::swaps
              << "\t\t" << CollInt::moves << "\n";
    return true;
}

int main()
{
    std::vector<size_t> sizes = {0, 1, 2, 10, 100, 500, 1000, 2000};

    std::cout << "N\tAlgo\t\tComps\t\tSwaps\t\tMoves\n";
    std::cout << "----------------------------------------------------------------\n";

    for (size_t n : sizes)
    {
        bool success = true;
        success = run_sort_case(n, "std::sort", [](auto begin, auto end)
        {
            std::sort(begin, end);
        }) && success;
        success = run_sort_case(n, "Bubble", bubble_sort<std::vector<CollInt>::iterator>) && success;
        success = run_sort_case(n, "QuickSort", quick_sort<std::vector<CollInt>::iterator>) && success;

        std::cout << "--------------------------------------------------------------\n";

        if (!success)
        {
            return 1;
        }
    }

    return 0;
}
