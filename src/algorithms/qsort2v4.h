/******************************************************************************
 * src/algorithms/qsort2v4.h
 *
 * Two-Pass Dual Pivot Quicksort. 
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

namespace qsort2v4 {

template <typename Iterator>
void ptrDualPivotTwoPass(Iterator lo, Iterator hi)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

//    if (lo >= hi) return;
    if (lo + 20 > hi)
    {
	InsertionSort(lo, hi - lo + 1);
	return;
    }

    if (*lo > *hi)
        std::swap(*lo, *hi);

    const value_type p = *lo;
    const value_type q = *hi;

    Iterator i = lo + 1;
    Iterator j = hi - 1;
    Iterator k = hi - 1;
    
    while (i < j)
    {
	while (*i < p)
	    i++;
	while (*j > p && i < j)
	    j--;
	if (i < j)
	{
	    std::swap(*i, *j);
	    i++;
	    j--;
	}
    }

    if (i == j && *j < p)
	i++;

    if (i < hi)
    {
	j = i; 
	
	while (j < k)
	{
	    while (*j < q)
	    {
		j++;
	    }
	    while (*k > q && j < k)
	    {
		k--;
	    }
	    if (j < k)
	    {
		std::swap(*j, *k);
		j++;
		k--;
	    }
	}
    }

    if (j == k && q < *j)
	k--;

    std::swap(*lo, *(i - 1));
    std::swap(*hi, *(k + 1));

    ptrDualPivotTwoPass(lo, i - 2);
    ptrDualPivotTwoPass(i, k);
    ptrDualPivotTwoPass(k + 2, hi);
}

template <typename ValueType>
void qsort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    ptrDualPivotTwoPass(input, input + n-1);
}

CONTESTANT_REGISTER_ALL(qsort, "qsort2v4", "Quicksort, dual pivot, two pass approach")

} // namespace qsort2v4
