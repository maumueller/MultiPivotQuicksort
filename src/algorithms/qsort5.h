/******************************************************************************
 * src/algorithms/qsort5v4.h
 *
 * Sorting using Quicksort, five pivots, using rotations
 *
 ******************************************************************************
 * Copyright (C) 2014 Martin Aumueller <martin.aumueller@tu-ilmenau.de>
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

namespace qsort5 {


template <typename Iterator>
void five_pivot(Iterator left, Iterator right)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    if (right - left < 25)
    {
        InsertionSort(left, right - left + 1);
    }
    else
    {
        networks::sort5(*left, *(left + 1), *(left + 2), *(right - 1), *right);

        value_type p1 = *left;
        value_type p2 = *(left + 1);
        value_type p3 = *(left + 2);
        value_type p4 = *(right - 1);
        value_type p5 = *right;

        Iterator i1 = left + 3;
        Iterator i2 = left + 3;
        Iterator i3 = left + 3;
        Iterator i4 = right - 2;
        Iterator i5 = right - 2;
        Iterator i6 = right - 2;

        while (i3 <= i4)
        {

            while (*i3 < p3 && i3 <= i4)
            {

                if (*i3 < p1)
                {
		    rotations::rotate3(*i3, *i2, *i1);
                    i1++, i2++;
                }
                else 
                {

                    if (*i3 < p2)
                    {
                        rotations::swap(*i2, *i3);
                        i2++;
                    }
                }
                i3++;

            }

            while(*i4 > p3 && i3 <= i4)
            {

                if (*i4 > p5)
                {
		    rotations::rotate3(*i4, *i5, *i6);
                    i6--, i5--;
                }
                else 
                {

                    if (*i4 > p4)
                    {
                        rotations::swap(*i4, *i5);
                        i5--;
                    }
                }
                i4--;

            }

            if (i3 <= i4)
            {
                if (*i3 < p4)
                {
                    if (*i4 > p2)
                    {
                        // a3 and a2 element
                        rotations::swap(*i3, *i4);
                    }
                    else
                    {
                        if (*i4 > p1)
                        {
                            // a3 and a1
                            rotations::rotate3(*i3, *i2, *i4);
                            i2++;

                        }
                        else
                        {
                            // a3 and a0
                            rotations::rotate4(*i3, *i2, *i1, *i4);
                            i2++, i1++;
                        }
                    }
                }
                else
                {
                    if (*i3 < p5)
                    {
                        if (*i4 > p2)
                        {
                            // a4 and a2 element
                            rotations::rotate3(*i3, *i4, *i5);
                            i5--;
                        }
                        else
                        {
                            if (*i4 > p1)
                            {
                                // a4 and a1
                                rotations::rotate4(*i3, *i2, *i4, *i5);
                                i2++, i5--;
                            }
                            else
                            {
                                // a4 and a0
                                rotations::rotate5(*i3, *i2, *i1, *i4, *i5);
                                i2++, i1++, i5--;
                            }
                        }
                    }
                    else
                    {
                        if (*i4 > p2)
                        {
                            // a5 and a2 element
                            rotations::rotate4(*i3, *i4, *i5, *i6);
                            i5--, i6--;
                        }
                        else
                        {
                            if (*i4 > p1)
                            {
                                // a5 and a1
                                rotations::rotate5(*i3, *i2, *i4, *i5, *i6);
                                i2++, i5--, i6--;
                            }
                            else
                            {
                                // a5 and a0
                                rotations::rotate6(*i3, *i2, *i1, *i4, *i5, *i6);
                                i2++, i1++, i5--, i6--;
                            }
                        }
                    }
                }
                i4--, i3++;
            }
        }

        rotations::rotate4(*(left + 2), *(i1 - 1), *(i2 - 1), *(i3 - 1));
	rotations::rotate3(*(left + 1), *(i1 - 2), *(i2 - 2));
	rotations::swap(*(left + 0), *(i1 - 3));
        
	rotations::rotate3(*(right - 1), *(i6 + 1), *(i5 + 1));
	rotations::swap(*(right - 0), *(i6 + 2));


        five_pivot(left,   i1 - 4);
        five_pivot(i1 - 2, i2 - 3);
        five_pivot(i2 - 1, i3 - 2);
        five_pivot(i3,     i5);
        five_pivot(i5 + 2, i6 + 1);
        five_pivot(i6 + 3, right);
    }
}

template <typename ValueType>
void qsort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    five_pivot(input, input + n-1);
}

CONTESTANT_REGISTER_ALL(qsort, "qsort5", "Quicksort, five pivots")

} // namespace qsort5
