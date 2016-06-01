/******************************************************************************
 * src/algorithms/qsort2v2.h
 *
 * Sorting using Dual Pivot Quicksort counting approach
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

namespace qsort2v2 {

template <typename Iterator>
void ptrDualPivotCounting(Iterator lo, Iterator hi)
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

    int d = 0;
	
    Iterator i = lo + 1;
    Iterator k = hi - 1;
    Iterator j = i;

    while (j <= k)
    {
	if (d > 0)
	{
	    if (*j <  p)
	    {
		std::swap(*i, *j);
		i++;
		j++;
		d++;
	    }
	    else
	    {
		if (*j < q)
		    j++;
		else
		{
		    std::swap(*j, *k);
		    k--;
		    d--;
		}
	    }
	}
	else
	{
	    while (*k > q)
	    {
		k--;
		d--;
	    }
	    if (j <= k)
	    {
		if (*k < p)
		{
		    rotations::rotate3(*k, *j, *i);
		    i++;
		    d++;
		}
		else
		{
		    std::swap(*j, *k);
		}
		j++;
	    }
	}
    }

    std::swap(*lo, *(i - 1));
    std::swap(*hi, *(k + 1));

    ptrDualPivotCounting(lo, i - 2);
    ptrDualPivotCounting(i, k);
    ptrDualPivotCounting(k + 2, hi);
}

template <typename ValueType>
void qsort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    ptrDualPivotCounting(input, input + n - 1);
}

CONTESTANT_REGISTER_ALL(qsort, "qsort2-count", "Quicksort, dual pivot, counting approach")

} // namespace qsort2v2
