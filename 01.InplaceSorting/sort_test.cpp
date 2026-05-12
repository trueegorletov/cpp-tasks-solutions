#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <vector>

#include "sorting.h"

using Sorter = std::function<void(std::vector<int>::iterator, std::vector<int>::iterator)>;

class SortingTest : public ::testing::TestWithParam<Sorter>
{
protected:
    void assert_sorts(std::vector<int> data)
    {
        std::vector<int> expected = data;
        std::sort(expected.begin(), expected.end());

        GetParam()(data.begin(), data.end());

        EXPECT_EQ(data, expected);
    }
};

TEST_P(SortingTest, SortsEmptyAndSingleElementRanges)
{
    assert_sorts({});
    assert_sorts({42});
}

TEST_P(SortingTest, SortsCommonIntegerCases)
{
    assert_sorts({3, 1, 2});
    assert_sorts({5, 4, 3, 2, 1});
    assert_sorts({1, 2, 3, 4, 5});
    assert_sorts({4, 1, 4, 2, 4, 3, 1});
    assert_sorts({0, -5, 8, -1, 3, 3, -5});
}

TEST_P(SortingTest, SortsSubrangeInPlace)
{
    std::vector<int> data = {100, 5, 3, 4, 1, 2, 200};
    GetParam()(data.begin() + 1, data.end() - 1);

    EXPECT_EQ(data, (std::vector<int> {100, 1, 2, 3, 4, 5, 200}));
}

INSTANTIATE_TEST_SUITE_P(
    InplaceAlgorithms,
    SortingTest,
    ::testing::Values(
        bubble_sort<std::vector<int>::iterator>,
        quick_sort<std::vector<int>::iterator>
    )
);
