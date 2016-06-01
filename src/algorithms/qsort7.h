/******************************************************************************
 * src/algorithms/qsort7v2.h
 *
 * Sorting using Quicksort, seven pivots, using rotations.
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

namespace qsort7 {

template <typename Iterator>
void seven_pivot(Iterator left, Iterator right)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    if (right - left < 27)
    {
        InsertionSort(left, right - left + 1);
    }
    else
    {
        networks::sort7(*left, *(left + 1), *(left + 2), *(left + 3), *(right - 2),  *(right - 1), *right);

        value_type p1 = *left;
        value_type p2 = *(left + 1);
        value_type p3 = *(left + 2);
	value_type p4 = *(left + 3);
        value_type p5 = *(right - 2);
        value_type p6 = *(right - 1);
        value_type p7 = *right;

        Iterator i1 = left + 4;
        Iterator i2 = left + 4;
        Iterator i3 = left + 4;
	Iterator i4 = left + 4;
        Iterator i5 = right - 3;
        Iterator i6 = right - 3;
        Iterator i7 = right - 3;
        Iterator i8 = right - 3;

        while (i4 <= i5)
        {

            while (*i4 < p4 && i4 <= i5)
            {

		if (*i4 < p2)
		{

		    if (*i4 < p1)
		    {
			rotations::rotate4(*i4, *i3, *i2, *i1);
			i1++;
		    }
		    else
		    {
			rotations::rotate3(*i4, *i3, *i2);
		    }
		    i2++;
		    i3++;
		}
		else 
                {

                    if (*i4 < p3)
		    {
		        rotations::swap(*i4, *i3);
		        i3++;
		    }
                }
		i4++;

            }

            while(*i5 > p4 && i4 <= i5)
            {

                if (*i5 > p6)
                {

		    if (*i5 > p7)
		    {
			rotations::rotate4(*i5, *i6, *i7, *i8);
			i8--;
		    }
		    else
		    {
			rotations::rotate3(*i5, *i6, *i7);
		    }
                    i6--, i7--;
                }
                else 
                {

                    if (*i5 > p5)
                    {
                        rotations::swap(*i5, *i6);
                        i6--;
                    }
                }
                i5--;

            }

            if (i4 <= i5)
            {
                if (*i4 < p6)
                {
                    if (*i4 < p5)
                    {
                        if (*i5 < p2)
                        {
                            if (*i5 < p1)
                            {
                                // a4 and a0
                                rotations::rotate5(*i4, *i3, *i2, *i1, *i5);
                                i1++,i2++,i3++;
                            }
                            else
                            {
                                // a4 and a1
                                rotations::rotate4(*i4, *i3, *i2, *i5);
                                i2++,i3++;
                            }
                        }
                        else
                        {
                            if (*i5 < p3)
                            {
                                // a4 and a2
                                rotations::rotate3(*i4, *i3, *i5);
                                i3++;
                            }
                            else
                            {
                                // a4 and a3
                                rotations::swap(*i4, *i5);
                            }
                        }
                    }
                    else
                    {
                        if (*i5 < p2)
                        {
                            if (*i5 < p1)
                            {
                                // a5 and a0
                                rotations::rotate6(*i4, *i3, *i2, *i1, *i5, *i6);
                                i1++, i2++, i3++;
                                i6--;
                            }
                            else
                            {
                                // a5 and a1
                                rotations::rotate5(*i4, *i3, *i2, *i5, *i6);
                                i2++, i3++;
                                i6--;
                            }
                        }
                        else
                        {
                            if (*i5 < p3)
                            {
                                // a5 and a2
                                rotations::rotate4(*i4, *i3, *i5, *i6);
                                i3++;
                                i6--;
                            }
                            else
                            {
                                // a5 and a3
                                rotations::rotate3(*i4, *i5, *i6);
                                i6--;
                            }
                        }
                    }
                }
                else
                {
                    if (*i4 < p7)
                    {
                        if (*i5 < p2)
                        {
                            if (*i5 < p1)
                            {
                                // a6 and a0
                                rotations::rotate7(*i4, *i3, *i2, *i1, *i5, *i6, *i7);
                                i1++, i2++, i3++;
                                i6--, i7--;
                            }
                            else
                            {
                                // a6 and a1
                                rotations::rotate6(*i4, *i3, *i2, *i5, *i6, *i7);
                                i2++, i3++;
                                i6--, i7--;
                            }
                        }
                        else
                        {
                            if (*i5 < p3)
                            {
                                // a6 and a2
                                rotations::rotate5(*i4, *i3, *i5, *i6, *i7);
                                i3++;
                                i6--, i7--;
                            }
                            else
                            {
                                // a6 and a3
                                rotations::rotate4(*i4, *i5, *i6, *i7);
                                i6--, i7--;
                            }
                        }
                    }
                    else
                    {
                        if (*i5 < p2)
                        {
                            if (*i5 < p1)
                            {
                                // a7 and a0
                                rotations::rotate8(*i4, *i3, *i2, *i1, *i5, *i6, *i7, *i8);
                                i1++, i2++, i3++;
                                i6--, i7--, i8--;
                            }
                            else
                            {
                                // a7 and a1
                                rotations::rotate7(*i4, *i3, *i2, *i5, *i6, *i7, *i8);
                                i2++, i3++;
                                i6--, i7--, i8--;
                            }
                        }
                        else
                        {
                            if (*i5 < p3)
                            {
                                // a7 and a2
                                rotations::rotate6(*i4, *i3, *i5, *i6, *i7, *i8);
                                i3++;
                                i6--, i7--, i8--;
                            }
                            else
                            {
                                // a7 and a3
                                rotations::rotate5(*i4, *i5, *i6, *i7, *i8);
                                i6--, i7--, i8--;
                            }
                        }
                    }
                }
                i4++, i5--;
            }
        }


	rotations::rotate5(*(left + 3), *(i1 - 1), *(i2 - 1), *(i3 - 1), *(i4 - 1));
        rotations::rotate4(*(left + 2), *(i1 - 2), *(i2 - 2), *(i3 - 2));
	rotations::rotate3(*(left + 1), *(i1 - 3), *(i2 - 3));
	rotations::swap(*(left + 0), *(i1 - 4));
        
	rotations::rotate4(*(right - 2), *(i8 + 1), *(i7 + 1), *(i6 + 1));
	rotations::rotate3(*(right - 1), *(i8 + 2), *(i7 + 2));
	rotations::swap(*(right - 0), *(i8 + 3));


        seven_pivot(left,   i1 - 5);
        seven_pivot(i1 - 3, i2 - 4);
        seven_pivot(i2 - 2, i3 - 3);
        seven_pivot(i3 - 1, i4 - 2);
        seven_pivot(i4    , i6    );
        seven_pivot(i6 + 2, i7 + 1);
        seven_pivot(i7 + 3, i8 + 2);
        seven_pivot(i8 + 4, right );
    }
}

template <typename ValueType>
void qsort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    seven_pivot(input, input + n-1);
}

CONTESTANT_REGISTER_ALL(qsort, "qsort7", "Quicksort, seven pivots");

} // namespace qsort7
