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

namespace qsort1lomutoblock_optimized {

#define BLOCKSIZE 512

template <typename Iterator>
void sort(Iterator lo, Iterator hi)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;
    typedef typename std::iterator_traits<Iterator>::difference_type index;

    index block[BLOCKSIZE];

    const value_type pivot = *hi;

    if (lo >= hi) {
        return InsertionSort(lo, hi - lo + 1);
    }

    Iterator counter = lo, offset = lo;
    int num_left = 0;
    value_type tmp;

    while (hi - counter > BLOCKSIZE) {
        for (index j = 0; j < BLOCKSIZE; ++j) {
            block[num_left] = j;
            num_left += (pivot > counter[j]);
        }
        if (num_left > 0) {
            tmp = *offset;
            for (index j = 1; j < num_left; j++) {
                *offset = *(counter + block[j]);
                offset++;
                *(counter + block[j]) = *offset;
            }
            *offset = *(counter + block[0]);
            *(counter + block[0]) = tmp;
            num_left = 0;
            offset++;
        }
        counter += BLOCKSIZE;
    }
//  
//    for (index i = 0; i < hi - lo + 1; i++) {
//        std::cout << lo[i] << " ";
//    }
//    std::cout << std::endl;
    
    for (index j = 0; j < hi - counter; ++j) {
        block[num_left] = j;
        num_left += (pivot > counter[j]);
    }
    if (num_left > 0) {
        tmp = *offset;
        for (index j = 1; j < num_left; j++) {
            *offset = *(counter + block[j]);
            offset++;
            *(counter + block[j]) = *offset;
        }
        *offset = *(counter + block[0]);
        offset++;
        *(counter + block[0]) = tmp;
    }
 
//    for (index i = 0; i < hi - lo + 1; i++) {
//        std::cout << lo[i] << " ";
//    }
//    std::cout << std::endl;

    std::iter_swap(offset, hi);
    
//    for (index i = 0; i < hi - lo + 1; i++) {
//        std::cout << lo[i] << " ";
//    }
//    std::cout << std::endl;
    
    sort(lo, offset - 1);
    sort(offset + 1, hi);
    
}

template <typename ValueType>
void qsort1()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    sort(input, input + n - 1);
}

CONTESTANT_REGISTER_ALL(qsort1, "qsort1lomutoblockoptimized", "Quicksort, single pivot, lomuto, block, optimized movement")

} // namespace qsort1is
