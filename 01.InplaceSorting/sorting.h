#ifndef SORTING_H
#define SORTING_H

// Header-only

#include "collvalue.h"

#include <iterator>
#include <utility>

// ----------------------------- Пузырёк (например) --------------------------------------
template <typename Iterator>
void bubble_sort(Iterator begin, Iterator end)
{
    if (begin == end)
    {
        return;
    }

    for (Iterator unsorted_end = end; unsorted_end != begin;)
    {
        --unsorted_end;
        bool swapped = false;

        for (Iterator current = begin; current != unsorted_end; ++current)
        {
            Iterator next = std::next(current);
            if (*next < *current)
            {
                using std::swap;
                swap(*current, *next);
                swapped = true;
            }
        }

        if (!swapped)
        {
            break;
        }
    }
}

// --------------------------- QuickSort (например) ---------------------------------------
template <typename Iterator>
void quick_sort(Iterator begin, Iterator end)
{
    while (end - begin > 1)
    {
        const auto size = end - begin;
        const auto pivot = *(begin + size / 2);
        Iterator less_end = begin;
        Iterator current = begin;
        Iterator greater_begin = end;

        while (current < greater_begin)
        {
            if (*current < pivot)
            {
                using std::swap;
                swap(*less_end, *current);
                ++less_end;
                ++current;
            }
            else if (pivot < *current)
            {
                --greater_begin;
                using std::swap;
                swap(*current, *greater_begin);
            }
            else
            {
                ++current;
            }
        }

        if (less_end - begin < end - greater_begin)
        {
            quick_sort(begin, less_end);
            begin = greater_begin;
        }
        else
        {
            quick_sort(greater_begin, end);
            end = less_end;
        }
    }
}

#endif // SORTING_H
