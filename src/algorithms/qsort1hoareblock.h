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

//taken from https://github.com/maumueller/LomutoBlockQuicksort/blob/6b72a8e8a33a26dabc1f709be0c6896f097366ad/partition.h

namespace qsort1hoareblock {

template <typename value_type>
bool less(value_type v1, value_type v2) {
    return v1 < v2;
}

template <typename iter>
void sort(iter begin, iter end)
{
	typedef typename std::iterator_traits<iter>::difference_type index;
	typedef typename std::iterator_traits<iter>::value_type value_type;

	index indexL[BLOCKSIZE], indexR[BLOCKSIZE];
    iter pivot_position; 

	if (begin + 16 >= end) {
        return InsertionSort(begin, end - begin + 1);
    }
	else if (begin + 100 < end) {
	    pivot_position = pivot_strategies::median_of_5_medians_of_5(begin, end);
    }
    else {
        pivot_position = pivot_strategies::median_of_3(begin, end);
    }
    std::iter_swap(pivot_position, end);

    iter start = begin;
	iter last = end;
	const typename std::iterator_traits<iter>::value_type & pivot = *last;
	last--;

	int num_left = 0;
	int num_right = 0;
	int start_left = 0;
	int start_right = 0;
	int num;
	//main loop
//    std::cout << "Input: ";
//    for (index i = 0; i < end - begin + 1; i++) {
//        std::cout << begin[i] << " ";
//    }
//    std::cout << std::endl;
	while (last - begin + 1 > 2 * BLOCKSIZE)
	{
		//Compare and store in buffers
		if (num_left == 0) {
			start_left = 0;
			for (index j = 0; j < BLOCKSIZE; j++) {
				indexL[num_left] = j;
				num_left += (!(less(begin[j], pivot)));                
			}
		}
		if (num_right == 0) {
			start_right = 0;
			for (index j = 0; j < BLOCKSIZE; j++) {
				indexR[num_right] = j;
				num_right += !(less(pivot, *(last - j)));                
			}
		}
		//rearrange elements
		num = std::min(num_left, num_right);
		for (int j = 0; j < num; j++)
			std::iter_swap(begin + indexL[start_left + j], last - indexR[start_right + j]);

		num_left -= num;
		num_right -= num;
		start_left += num;
		start_right += num;
		begin += (num_left == 0) ? BLOCKSIZE : 0;
		last -= (num_right == 0) ? BLOCKSIZE : 0;

	}//end main loop

	//Compare and store in buffers final iteration
	index shiftR = 0, shiftL = 0;
	if (num_right == 0 && num_left == 0) {    //for small arrays or in the unlikely case that both buffers are empty
		shiftL = ((last - begin) + 1) / 2;
		shiftR = (last - begin) + 1 - shiftL;
		assert(shiftL >= 0); assert(shiftL <= BLOCKSIZE);
		assert(shiftR >= 0); assert(shiftR <= BLOCKSIZE);
		start_left = 0; start_right = 0;
		for (index j = 0; j < shiftL; j++) {
			indexL[num_left] = j;
			num_left += (!less(begin[j], pivot));
			indexR[num_right] = j;
			num_right += !less(pivot, *(last - j));
		}
		if (shiftL < shiftR)
		{
			assert(shiftL + 1 == shiftR);
			indexR[num_right] = shiftR - 1;
			num_right += !less(pivot, *(last - shiftR + 1));
		}
	}
	else if (num_right != 0) {
		shiftL = (last - begin) - BLOCKSIZE + 1;
		shiftR = BLOCKSIZE;
		assert(shiftL >= 0); assert(shiftL <= BLOCKSIZE); assert(num_left == 0);
		start_left = 0;
		for (index j = 0; j < shiftL; j++) {
			indexL[num_left] = j;
			num_left += (!less(begin[j], pivot));
		}
	}
	else {
		shiftL = BLOCKSIZE;
		shiftR = (last - begin) - BLOCKSIZE + 1;
		assert(shiftR >= 0); assert(shiftR <= BLOCKSIZE); assert(num_right == 0);
		start_right = 0;
		for (index j = 0; j < shiftR; j++) {
			indexR[num_right] = j;
			num_right += !(less(pivot, *(last - j)));
		}
	}

	//rearrange final iteration
	num = std::min(num_left, num_right);
	for (int j = 0; j < num; j++)
		std::iter_swap(begin + indexL[start_left + j], last - indexR[start_right + j]);

	num_left -= num;
	num_right -= num;
	start_left += num;
	start_right += num;
	begin += (num_left == 0) ? shiftL : 0;
	last -= (num_right == 0) ? shiftR : 0;            
	//end final iteration
    
//    std::cout << "After rearrangment: ";
//
//    for (index i = 0; i < end - begin + 1; i++) {
//        std::cout << begin[i] << " ";
//    }
//    std::cout << std::endl;

	//rearrange elements remaining in buffer
	if (num_left != 0)
	{
		
		assert(num_right == 0);
		int lowerI = start_left + num_left - 1;
		index upper = last - begin;
		//search first element to be swapped
		while (lowerI >= start_left && indexL[lowerI] == upper) {
			upper--; lowerI--;
		}
		while (lowerI >= start_left)
			std::iter_swap(begin + upper--, begin + indexL[lowerI--]);

		std::iter_swap(end, begin + upper + 1); // fetch the pivot 
        sort(start, begin + upper);
        sort(begin + upper + 2, end);
	}
	else if (num_right != 0) {
		assert(num_left == 0);
		int lowerI = start_right + num_right - 1;
		index upper = last - begin;
		//search first element to be swapped
		while (lowerI >= start_right && indexR[lowerI] == upper) {
			upper--; lowerI--;
		}
		
		while (lowerI >= start_right)
			std::iter_swap(last - upper--, last - indexR[lowerI--]);

		std::iter_swap(end, last - upper);// fetch the pivot 
        sort(start, last - upper - 1);
        sort(last - upper + 1, end);
	}
	else { //no remaining elements
		assert(last + 1 == begin);
		std::iter_swap(end, begin);// fetch the pivot 
        sort(start, begin - 1);
        sort(begin + 1, end);
	}
//    std::cout << "After local sorting: ";
//    for (index i = 0; i < end - start + 1; i++) {
//        std::cout << start[i] << " ";
//    }
//    std::cout << std::endl;
}

template <typename ValueType>
void qsort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    sort(input, input + n - 1);
}

CONTESTANT_REGISTER_ALL(qsort, "qsort1hoareblock", "Quicksort, one pivot, hoare, block")

} // namespace qsort1is
