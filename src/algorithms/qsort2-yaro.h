/******************************************************************************
 * src/algorithms/qsort2v1.h
 *
 * Sorting using Quicksort Version 
 *
 ******************************************************************************
 * Copyright (C) 2014 Sebastian Wild <wild@cs.uni-kl.de>
 * Copyright (C) 2014 Timo Bingmann <tb@panthema.net>
 * Copyright (C) 2014 Martin Aumüller <martin.aumueller@tu-ilmenau.de>
 * 
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

namespace qsort2v1 {

template <typename Iterator>
void ptrDualPivotYaroslavskiy(Iterator lo, Iterator hi)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    //if (lo >= hi) return;
    if (lo + 20 > hi) 
    {
        InsertionSort(lo, hi - lo + 1);
        return;
    }

    if (*lo > *hi)
        std::swap(*lo, *hi);

    const value_type p = *lo;
    const value_type q = *hi;

    Iterator l = lo + 1;
    Iterator g = hi - 1;
    Iterator k = l;

    while (k <= g)
    {
        if (*k < p)
        {
            std::swap(*k, *l);
            ++l;
        }
        else if (*k >= q)
        {
            while (*g > q)  
            {
                --g;
            }

            if (k < g)
            {
                if (*g < p)
                {
                    rotations::rotate3(*g, *k, *l);
                    ++l;
                }
                else
                {
                    std::swap(*k, *g);
                }
                --g;
            }
        }
        ++k;
    }
    --l;
    ++g;
    std::swap(*lo, *l);
    std::swap(*hi, *g);

    ptrDualPivotYaroslavskiy(lo, l - 1);
    ptrDualPivotYaroslavskiy(l + 1, g - 1);
    ptrDualPivotYaroslavskiy(g + 1, hi);
}

template <typename ValueType>
void qsort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    ptrDualPivotYaroslavskiy(input, input + n-1);
}

CONTESTANT_REGISTER_ALL(qsort, "qsort2-yaro", "Quicksort, dual pivot, Yaroslavskiy")

} // namespace qsort2v1
