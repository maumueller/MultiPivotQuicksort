/******************************************************************************
 * src/algorithms/qsort1is.h
 *
 * Sorting using Quicksort, single pivot, version 2, with base case networks
 *
 ******************************************************************************
 * Copyright (C) 2014 Martin Aumüller <maau@itu.dk>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <algorithm>

namespace qsortksample {

template <typename Iterator>
void sort(Iterator lo, Iterator hi)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    value_type pivot = *hi;
    
    if (lo + 16 >= hi)
        return;

    Iterator i = lo, j = i;

    while (j < hi && pivot < *j)
        j++;

    if (j == hi)
    {
        std::swap(*i, *hi);
        ptrQuickSortLR(i + 1, hi);
        return;
    }
    std::swap(*i, *j);
    j++;
    while (j < hi)
    {
        auto tmp = *j;
        auto smaller = (tmp < pivot);
        i += smaller;
        auto delta = smaller * (j - i);
        auto s = i + delta;
        auto t = j - delta;
        *s = *i;
        *t = tmp;
        j++;
    }
    std::swap(*(++i), *hi);

    ptrQuickSortLR(lo, i - 1);
    ptrQuickSortLR(i + 1, hi);
}

template <typename ValueType>
void qsort1()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    sort(input, input + n-1);
}

CONTESTANT_REGISTER_ALL(qsort1, "qsortksample", "Quicksort, k pivots, sample")

} // namespace qsort1is
