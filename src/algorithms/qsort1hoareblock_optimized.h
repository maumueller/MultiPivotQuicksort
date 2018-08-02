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

namespace qsort1hoareblock_optimized {

template <typename value_type>
bool less(value_type v1, value_type v2) {
    return v1 < v2;
}

template <typename iter>
void sort(iter begin, iter end, iter input_start, iter input_end)
{
	typedef typename std::iterator_traits<iter>::difference_type index;
	typedef typename std::iterator_traits<iter>::value_type t;

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
    iter start = begin;
    iter temp_begin = begin;
	iter last = end;
    bool double_pivot_check = (end < input_end && (*pivot_position >= *end)) || (begin > input_start && !(*(begin - 1) < *pivot_position));
    int pivot_length = 1;
	t pivot = std::move(*pivot_position);
	*pivot_position = std::move(*last);
	iter hole = last;
	t temp;
	last--;

	int num_left = 0;
	int num_right = 0;
	int start_left = 0;
	int start_right = 0;
	int j;
	int num;

	bool small_array = (last - begin + 1 <= 2 * BLOCKSIZE) && ((last - begin) > 48);
	//main loop
	while (last - begin + 1 > 2 * BLOCKSIZE)
	{
		//Compare and store in buffers
		if (num_left == 0) {
			start_left = 0;
			for (j = 0; j < BLOCKSIZE; ) {
				indexL[num_left] = j;
				num_left += (!(less(begin[j], pivot)));
				j++;
				indexL[num_left] = j;
				num_left += (!(less(begin[j], pivot)));
				j++;
				indexL[num_left] = j;
				num_left += (!(less(begin[j], pivot)));
				j++;
				indexL[num_left] = j;
				num_left += (!(less(begin[j], pivot)));
				j++;
			}
		}
		if (num_right == 0) {
			start_right = 0;
			for (j = 0; j < BLOCKSIZE; ) {
				indexR[num_right] = j;
				num_right += !(less(pivot, *(last - j)));
				j++;
				indexR[num_right] = j;
				num_right += !(less(pivot, *(last - j)));
				j++;
				indexR[num_right] = j;
				num_right += !(less(pivot, *(last - j)));
				j++;
				indexR[num_right] = j;
				num_right += !(less(pivot, *(last - j)));
				j++;
			}
		}
		//rearrange elements
		num = std::min(num_left, num_right);
		if (num != 0)
		{
			*hole = std::move(*(begin + indexL[start_left]));
			*(begin + indexL[start_left]) = std::move(*(last - indexR[start_right]));
			for (j = 1; j < num; j++)
			{
				*(last - indexR[start_right + j - 1]) = std::move(*(begin + indexL[start_left + j]));
				*(begin + indexL[start_left + j]) = std::move(*(last - indexR[start_right + j]));
			}
			hole = (last - indexR[start_right + num - 1]);
		}
		num_left -= num;
		num_right -= num;
		start_left += num;
		start_right += num;
		begin += (num_left == 0) ? BLOCKSIZE : 0;
		last -= (num_right == 0) ? BLOCKSIZE : 0;
	}//end main loop

	if (num_left == 0) start_left = 0;
	if (num_right == 0) start_right = 0;

	//Compare and store in buffers final iteration
	int shiftR = 0, shiftL = 0;
	if (num_right == 0 && num_left == 0) {    //for small arrays or in the unlikely case that both buffers are empty
		shiftL = (int)((last - begin) + 1) / 2;
		shiftR = (int)(last - begin) + 1 - shiftL;
		assert(shiftL >= 0); assert(shiftL <= BLOCKSIZE);
		assert(shiftR >= 0); assert(shiftR <= BLOCKSIZE);
		start_left = 0; start_right = 0;
		for (j = 0; j < shiftL; j++) {
			indexL[num_left] = j;
			num_left += (!less(begin[j], pivot));
			indexR[num_right] = j;
			num_right += !less(pivot, *(last - j));
		}
		if (shiftL < shiftR)
		{
			assert(shiftL + 1 == shiftR);
			indexR[num_right] = j;
			num_right += !less(pivot, *(last - j));
		}
	}
	else if (num_right != 0) {
		shiftL = (int)(last - begin) - BLOCKSIZE + 1;
		shiftR = BLOCKSIZE;
		assert(shiftL >= 0); assert(shiftL <= BLOCKSIZE); assert(num_left == 0);
		start_left = 0;
		for (j = 0; j < shiftL; j++) {
			indexL[num_left] = j;
			num_left += (!less(begin[j], pivot));
		}
	}
	else {
		shiftL = BLOCKSIZE;
		shiftR = (int)(last - begin) - BLOCKSIZE + 1;
		assert(shiftR >= 0); assert(shiftR <= BLOCKSIZE); assert(num_right == 0);
		start_right = 0;
		for (j = 0; j < shiftR; j++) {
			indexR[num_right] = j;
			num_right += !(less(pivot, *(last - j)));
		}
	}

	//rearrange final iteration
	num = std::min(num_left, num_right);
	if (num != 0)
	{
		*hole = std::move(*(begin + indexL[start_left]));
		*(begin + indexL[start_left]) = std::move(*(last - indexR[start_right]));
		for (j = 1; j < num; j++)
		{
			*(last - indexR[start_right + j - 1]) = std::move(*(begin + indexL[start_left + j]));
			*(begin + indexL[start_left + j]) = std::move(*(last - indexR[start_right + j]));
		}
		hole = (last - indexR[start_right + num - 1]);
	}
	num_left -= num;
	num_right -= num;
	start_left += num;
	start_right += num;

	if (num_left == 0)
		begin += shiftL;
	if (num_right == 0)
		last -= shiftR;

	/*    std::cout << "Partition check" << std::endl;
		  for (iter it = bbegin; it != begin;  it++)
		  if(*it > pivot)
		  std::cout << "vorne" << begin - it << ", " << it->value() << std::endl;
		  for (iter it = last + 1; it != eend ;  it++)
		  if (*it < pivot)
		  std::cout <<"hinten" << it-last << ", " << it->value() << std::endl;
		  ;*/

	//rearrange remaining elements
	if (num_left != 0)
	{
		assert(num_right == 0);
		int lowerI = start_left + num_left - 1;
		int upper = (int)(last - begin);
		while (lowerI >= start_left && indexL[lowerI] == upper)
		{
			upper--; lowerI--;
		}
		temp = std::move(*(begin + upper));
		while (lowerI >= start_left)
		{
			*(begin + upper) = std::move(*(begin + indexL[lowerI]));
			*(begin + indexL[lowerI]) = std::move(*(begin + (--upper)));
			lowerI--;
		}
		*(begin + upper) = std::move(temp);
		*hole = std::move(*(begin + upper + 1));

		//check for double elements if the pivot sample has repetitions or a small array is partitioned very unequal
		if (double_pivot_check || (small_array && num_left >= (15 * shiftL) / 16)) {
			iter begin_lomuto = begin + upper + 1;
			iter q = begin_lomuto + 1;

			//check at least 4 elements whether they are equal to the pivot using Elmasry, Katajainen and Stenmark's Lomuto partitioner
			unsigned int count_swaps = 1;
			unsigned int count_steps = 0;
			while (q < end && (count_swaps << 2) > count_steps) { //continue as long as there are many elements equal to pivot
				typename std::iterator_traits<iter>::value_type x = std::move(*q);
				bool smaller = !less(pivot, x);
				begin_lomuto += smaller; // smaller = 1 ? begin++ : begin
				count_swaps += smaller;
				index delta = smaller * (q - begin_lomuto);
				iter s = begin_lomuto + delta; // smaller = 1 => s = q : s = begin
				iter y = q - delta; // smaller = 1 => y = begin : y = q
				*s = std::move(*begin_lomuto);
				*y = std::move(x);
				++q;
				count_steps++;
			}

			pivot_length = begin_lomuto + 1 - (begin + upper + 1);

			//    std::cout << "check for double elements left" << pivot_length << " of " << num_left << " array size " << end - temp_begin << std::endl;
		}
		*(begin + upper + 1) = std::move(pivot); // fetch the pivot 
		sort(start, begin + upper, input_start, input_end);
		sort(begin + upper + 1 + pivot_length, end, input_start, input_end);

	}
	else if (num_right != 0) {
		assert(num_left == 0);
		int lowerI = start_right + num_right - 1;
		int upper = (int)(last - begin);
		while (lowerI >= start_right && indexR[lowerI] == upper)
		{
			upper--; lowerI--;
		}
		*hole = std::move(*(last - upper));
		while (lowerI >= start_right)
		{
			*(last - upper) = std::move(*(last - indexR[lowerI]));
			*(last - indexR[lowerI--]) = std::move(*(last - (--upper)));
		}

		//check for double elements if the pivot sample has repetitions or a small array is partitioned very unequal
		if (double_pivot_check || (small_array && num_right >= (15 * shiftR) / 16)) {
			iter begin_lomuto = last - upper;
			iter q = begin_lomuto - 1;

			//check at least 4 elements whether they are equal to the pivot using Elmasry, Katajainen and Stenmark's Lomuto partitioner
			unsigned int count_swaps = 1;
			unsigned int count_steps = 0;
			while (q > temp_begin && (count_swaps << 2) > count_steps) { //continue as long as there are many elements equal to pivot
				typename std::iterator_traits<iter>::value_type x = std::move(*q);
				bool smaller = !less(x, pivot);
				begin_lomuto -= smaller; // smaller = 1 ? begin++ : begin
				count_swaps += smaller;
				index delta = smaller * (q - begin_lomuto);
				iter s = begin_lomuto + delta; // smaller = 1 => s = q : s = begin
				iter y = q - delta; // smaller = 1 => y = begin : y = q
				*s = std::move(*begin_lomuto);
				*y = std::move(x);
				--q;
				count_steps++;
			}

			pivot_length = (last - upper) + 1 - begin_lomuto;
			*(last - upper) = std::move(pivot); // fetch the pivot 
			sort(start, last - upper - pivot_length, input_start, input_end);
		    sort(last - upper + 1, end, input_start, input_end);
		}
		else
		{
			*(last - upper) = std::move(pivot); // fetch the pivot 
			sort(start, last - upper - 1, input_start, input_end);
  			sort(last - upper + 1, end, input_start, input_end);
		}


	}
	else { //no remaining elements
		assert(last + 1 == begin);
		*hole = std::move(*begin);
		*begin = std::move(pivot); // fetch the pivot 
		sort(start, begin - 1, input_start, input_end);
	    sort(begin + 1, end, input_start, input_end);	
	}

}

template <typename ValueType>
void qsort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    sort(input, input + n - 1, input, input + n  - 1);
}

CONTESTANT_REGISTER_ALL(qsort, "qsort1hoareblockoptimized", "Quicksort, one pivot, hoare, block, all optimizations")

} // namespace qsort1is
