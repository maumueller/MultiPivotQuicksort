/******************************************************************************
 * src/algorithms/qsort2branchfree.h
 *
 * Sorting using Quicksort, two pivots, branch free inspired by Lomuto. 
 *
 ******************************************************************************
 * Copyright (C) 2016 Martin Aumüller <maau@itu.dk>
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

namespace qsort2branchfree {

template <typename Iterator>
void dualpivot(Iterator lo, Iterator hi)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    if (lo + 20 > hi)
    {
        InsertionSort(lo, hi - lo + 1);
        return;
    }
    
    if (*lo > *hi)
        rotations::swap(*lo, *hi);

    const value_type p = *lo;
    const value_type q = *hi;

    Iterator i = lo;
    Iterator j = lo;
    Iterator k = lo + 1;

#if 0
    std::cout << "Before partitioning" << std::endl;
    for (auto it = lo; it <= hi ; it++)
        std::cout << *it << " ";
    std::cout << std::endl;
#endif 

    while (k < hi)
    {
        auto tmp = *k;
        auto smaller = (tmp < p);
        auto notlarger = (tmp < q);
        i += smaller;
        j += notlarger;
        auto delta1 = (k - j) * notlarger;
        auto delta2 = (j - i) * smaller;
        auto l1 = k - delta1 - delta2;
        auto l2 = k - delta1;
        *k = *l2;
        *l2 = *l1;
        *l1 = tmp;
        k++;
    }
    rotations::swap(*lo, *i );
    rotations::swap(*hi, *(j + 1) );

#if 0    
    std::cout << "After partitioning" << std::endl;
    for (auto it = lo; it <= hi; it++)
        std::cout << *it << " ";
    std::cout << std::endl;
#endif

    dualpivot(lo, i - 1);
    dualpivot(i + 1, j);
    dualpivot(j + 2, hi);
}

template <typename ValueType>
void sort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    dualpivot(input, input + n-1);
}

CONTESTANT_REGISTER_ALL(sort, "qsort2branchfree", "Quicksort, two pivots, branch free")

} // namespace qsort1is
