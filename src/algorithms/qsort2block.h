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
#include <iostream>

namespace qsort2lomutoblock {

#define QSORT2BLOCKSIZE 1024

template <typename Iterator>
void sort(Iterator lo, Iterator hi)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;
    typedef typename std::iterator_traits<Iterator>::difference_type index;
    
    if (lo + 23 >= hi) {
        return InsertionSort(lo, hi - lo + 1);
    }
    else if (lo + 1000 < hi) {
        pivot_strategies::medians_first_third_of_five(lo, hi);
    }
    else if (lo + 200 <  hi) {
        pivot_strategies::first_third_of_five(lo, hi);
    }
    else {
        pivot_strategies::first_two_of_three(lo, hi);
    }

    index block1[QSORT2BLOCKSIZE];
    index block2[QSORT2BLOCKSIZE];

    const value_type p = *lo;
    const value_type q = *hi;

//    for (index i = 0; i < hi - lo + 1; i++) {
//        std::cout << lo[i] << " ";
//    }
//    std::cout << std::endl;

    Iterator counter = lo + 1, offset1 = lo + 1, offset2 = lo + 1;
    short num1 = 0, num2 = 0;

    while (hi - counter > QSORT2BLOCKSIZE) {
        for (index j = 0; j < QSORT2BLOCKSIZE; ++j) {
            block2[num2] = j;
            num2 += (q >= counter[j]);
        }
        for (index j = 0; j < num2; ++j) {
            std::iter_swap(offset2 + j, counter + block2[j]);
        }

        for (index j = 0; j < num2; ++j) {
            block1[num1] = j;
            num1 += (p > offset2[j]);
        }
        for (index j = 0; j < num1; ++j) {
            std::iter_swap(offset1++, offset2 + block1[j]);
        }
        offset2 += num2;
        num1 = 0;
        num2 = 0;
        counter += QSORT2BLOCKSIZE;
    }

    for (index j = 0; j < hi - counter; ++j) {
        block2[num2] = j;
        num2 += (q >= counter[j]);
    }
    for (index j = 0; j < num2; ++j) {
        std::iter_swap(offset2 + j, counter + block2[j]);
    }

    for (index j = 0; j < num2; ++j) {
        block1[num1] = j;
        num1 += (p > offset2[j]);
    }
    for (index j = 0; j < num1; ++j) {
        std::iter_swap(offset1++, offset2 + block1[j]);
    }
    offset2 += num2;
    
//    for (index i = 0; i < hi - lo + 1; i++) {
//        std::cout << lo[i] << " ";
//    }
//    std::cout << std::endl;

    std::iter_swap(lo, offset1 - 1);
    std::iter_swap(offset2, hi);
    
//    for (index i = 0; i < hi - lo + 1; i++) {
//        std::cout << lo[i] << " ";
//    }
//    std::cout << std::endl;
    sort(lo, offset1 - 2);
    if (p != q) {
        sort(offset1, offset2 - 1);
    }
    sort(offset2 + 1, hi);
}

template <typename ValueType>
void qsort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    sort(input, input + n - 1);
}

CONTESTANT_REGISTER_ALL(qsort, "qsort2lomutoblock", "Quicksort, dual pivot, lomuto, block")

} // namespace qsort1is
