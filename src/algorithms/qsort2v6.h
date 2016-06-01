/******************************************************************************
 * src/algorithms/qsort2v5.h
 *
 * Sorting using Dual Pivot Quicksort, larger pivot first.
 *
 ******************************************************************************
 * Copyright (C) 2014 Martin Aumueller <martin.aumueller@tu-ilmenau.de>
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

namespace qsort2v6
{

template <typename Iterator>
void ptrDualPivotLargerFirst(Iterator lo, Iterator hi)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

//    if (lo >= hi) return;
    if (lo + 20 > hi)
    {
	   InsertionSort(lo, hi - lo + 1);
	   return;
    }

    if (*lo > *hi)
        rotations::swap(*lo, *hi);

    const value_type p = *lo;
    const value_type q = *hi;

    Iterator i = lo + 1;
    Iterator k = i;
    Iterator j = i;
   
    while (k < hi)
    {
        while (q < *k)
        {
            k++;
        }

        if (*k > p)
        {
            rotations::swap(*j, *k);
        }
        else
        {
            rotations::rotate3(*k, *j, *i);
            i++;
        }
        j++, k++;
    }

    rotations::swap(*lo, *(i - 1));
    rotations::swap(*hi, *j);

    ptrDualPivotLargerFirst(lo, i - 2);
    ptrDualPivotLargerFirst(i, j - 1);
    ptrDualPivotLargerFirst(j + 1, hi);
}

template <typename ValueType>
void qsort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    ptrDualPivotLargerFirst(input, input + n - 1);
}

CONTESTANT_REGISTER_ALL(qsort, "qsort2v6", "Quicksort, dual pivot, larger pivot first, easy layout")

} // namespace qsort2v6
