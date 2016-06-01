/******************************************************************************
 * src/algorithms/qsort1is.h
 *
 * Sorting using Quicksort, single pivot, version 2, with base case networks
 *
 ******************************************************************************
 * Copyright (C) 2014 Sebastian Wild <wild@cs.uni-kl.de>
 * Copyright (C) 2014 Timo Bingmann <tb@panthema.net>
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

namespace qsort1is {

template <typename Iterator>
void ptrQuickSortLR(Iterator lo, Iterator hi)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

//    if (lo >= hi) return;

    if (lo + 10 > hi)
        return InsertionSort(lo, hi - lo + 1);

    value_type pivot = *hi;

    Iterator i = lo - 1, j = hi;

    while (1)
    {
        do { i++; } while (*i < pivot);

        do { j--; } while (*j > pivot);

        if (LIKELY(j > i))
            std::swap(*i, *j);
        else
            break;
    }

    std::swap(*i, *hi);

    ptrQuickSortLR(lo, i-1);
    ptrQuickSortLR(i+1, hi);
}

template <typename ValueType>
void qsort1()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    ptrQuickSortLR(input, input + n-1);
}

CONTESTANT_REGISTER_ALL(qsort1, "qsort1is", "Quicksort, single pivot, insertion-sort")

} // namespace qsort1is
