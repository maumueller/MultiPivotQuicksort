/******************************************************************************
 * src/algorithms/qsort9v2.h
 *
 * Sorting using Quicksort, nine pivots, using perfect rotations.
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

namespace qsort9v2 {

template <typename Iterator>
void nine_pivot(Iterator left, Iterator right)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    using rotations::swap;
    using namespace rotations;

    if (right - left < 29)
    {
        InsertionSort(left, right - left + 1);
    }
    else
    {
        networks::sort9(*left, *(left + 1), *(left + 2), *(left + 3), *(left + 4), 
                        *(right - 3), *(right - 2),  *(right - 1), *right);

        value_type p1 = *left;
        value_type p2 = *(left + 1);
        value_type p3 = *(left + 2);
	value_type p4 = *(left + 3);
        value_type p5 = *(left + 4);
        value_type p6 = *(right - 3);
        value_type p7 = *(right - 2);
        value_type p8 = *(right - 1);
        value_type p9 = *right;

        Iterator i = left + 5;
        Iterator j = right  - 4;

        Iterator g1 = i;
        Iterator g2 = i;
	Iterator g3 = i;
        Iterator g4 = i;
        Iterator g5 = j;
        Iterator g6 = j;
        Iterator g7 = j;
        Iterator g8 = j;

        while (i <= j)
        {

            while (*i < p5 && i <= j)
            {

                if (*i < p3)
                {

                    if (*i < p2)
                    {

                        if (*i < p1)
                        {
                            rotate5(*i, *g4, *g3, *g2, *g1);
                            g1++;
                        }
                        else
                        {
                            rotate4(*i, *g4, *g3, *g2);
                        }
                        g2++;
                    }
                    else
                    {
                        rotate3(*i, *g4, *g3);
                    }
                    g3++;
                    g4++;
                }
                else 
                {

                    if (*i < p4)
                    {
                        rotations::swap(*i, *g4);
                        g4++;
                    }
                }
                i++;

            }
            

            while (*j > p5 && i <= j)
            {

                if (*j > p7)
                {

                    if (*j < p8)
                    {
                        rotate3(*j, *g5, *g6);
                    }
                    else
                    {

                        if (*j < p9)
                        {
                            rotate4(*j, *g5, *g6, *g7);
                        }
                        else
                        {
                            rotate5(*j, *g5, *g6, *g7, *g8);
                            g8--;
                        }
                        g7--;
                    }
                    g5--;
                    g6--;
                }
                else 
                {

                    if (*j > p6)
                    {
                        rotations::swap(*j, *g5);
                        g5--;
                    }
                }
                j--;

            }

            if (i < j)
            {
                if (*i < p7)
                {
                    if (*i < p6)
                    {
                        if (*j > p3)
                        {
                            if (*j > p4)
                            {
                                // a5 and a4
                                rotations::swap(*i, *j);

                            }
                            else
                            {
                                // a5 and a3
                                rotations::rotate3(*i, *g4, *j);
                                g4++;
                            }
                        }
                        else
                        {
                            if (*j > p2)
                            {
                                // a5 and a2
                                rotations::rotate4(*i, *g4, *g3, *j);
                                g3++, g4++;
                            }
                            else
                            {
                                if (*j > p1)
                                {
                                    // a5 and a1
                                    rotations::rotate5(*i, *g4, *g3, *g2, *j);
                                    g2++, g3++, g4++;
                                }
                                else
                                {
                                    // a5 and a0
                                    rotations::rotate6(*i, *g4, *g3, *g2, *g1, *j);
                                    g1++, g2++, g3++, g4++;
                                }
                            }
                        }

                    }
                    else
                    {
                        if (*j > p3)
                        {
                            if (*j > p4)
                            {
                                // a6 and a4
                                rotations::rotate3(*i, *j, *g5);
                                g5--;

                            }
                            else
                            {
                                // a6 and a3
                                rotations::rotate4(*i, *g4, *j, *g5);
                                g4++;
                                g5--;
                            }
                        }
                        else
                        {
                            if (*j > p2)
                            {
                                // a6 and a2
                                rotations::rotate5(*i, *g4, *g3, *j, *g5);
                                g3++, g4++;
                                g5--;
                            }
                            else
                            {
                                if (*j > p1)
                                {
                                    // a6 and a1
                                    rotations::rotate6(*i, *g4, *g3, *g2,  *j, *g5);
                                    g2++, g3++, g4++;
                                    g5--;
                                }
                                else
                                {
                                    // a6 and a0
                                    rotations::rotate7(*i, *g4, *g3, *g2, *g1, *j, *g5);
                                    g1++, g2++, g3++, g4++;
                                    g5--;
                                }
                            }
                        }
                    }
                } 
                else
                {
                    if (*i < p8)
                    {
                        if (*j > p3)
                        {
                            if (*j > p4)
                            {
                                // a7 and a4
                                rotations::rotate4(*i, *j, *g5, *g6);
                                g5--, g6--;

                            }
                            else
                            {
                                // a7 and a3
                                rotations::rotate5(*i, *g4, *j, *g5, *g6);
                                g4++;
                                g5--, g6--;
                            }
                        }
                        else
                        {
                            if (*j > p2)
                            {
                                // a7 and a2
                                rotations::rotate6(*i, *g4, *g3, *j, *g5, *g6);
                                g3++, g4++;
                                g5--, g6--;
                            }
                            else
                            {
                                if (*j > p1)
                                {
                                    // a7 and a1
                                    rotations::rotate7(*i, *g4, *g3, *g2, *j, *g5, *g6);
                                    g2++, g3++, g4++;
                                    g5--, g6--;
                                }
                                else
                                {
                                    // a7 and a0
                                    rotations::rotate8(*i, *g4, *g3, *g2, *g1, *j, *g5, *g6);
                                    g1++, g2++, g3++, g4++;
                                    g5--, g6--;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (*i < p9)
                        {
                            if (*j > p3)
                            {
                                if (*j > p4)
                                {
                                    // a8 and a4
                                    rotations::rotate5(*i, *j, *g5, *g6, *g7);
                                    g5--, g6--, g7--;
                                }
                                else
                                {
                                    // a8 and a3
                                    rotations::rotate6(*i, *g4, *j, *g5, *g6, *g7);
                                    g4++;
                                    g5--, g6--, g7--;
                                }
                            }
                            else
                            {
                                if (*j > p2)
                                {
                                    // a8 and a2
                                    rotations::rotate7(*i, *g4, *g3, *j, *g5, *g6, *g7);
                                    g3++, g4++;
                                    g5--, g6--, g7--;
                                }
                                else
                                {
                                    if (*j > p1)
                                    {
                                        // a8 and a1
                                        rotations::rotate8(*i, *g4, *g3, *g2, *j, *g5, *g6, *g7);
                                        g2++, g3++, g4++;
                                        g5--, g6--, g7--;
                                    }
                                    else
                                    {
                                        // a8 and a0
                                        rotations::rotate9(*i, *g4, *g3, *g2, *g1, *j, *g5, *g6, *g7);
                                        g1++, g2++, g3++, g4++;
                                        g5--, g6--, g7--;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (*j > p3)
                            {
                                if (*j > p4)
                                {
                                    // a9 and a4
                                    rotations::rotate6(*i, *j, *g5, *g6, *g7, *g8);
                                    g5--, g6--, g7--, g8--;
                                }
                                else
                                {
                                    // a9 and a3
                                    rotations::rotate7(*i, *g4, *j, *g5, *g6, *g7, *g8);
                                    g4++;
                                    g5--, g6--, g7--, g8--;
                                }
                            }
                            else
                            {
                                if (*j > p2)
                                {
                                    // a9 and a2
                                    rotations::rotate8(*i, *g4, *g3, *j, *g5, *g6, *g7, *g8);
                                    g3++, g4++;
                                    g5--, g6--, g7--, g8--;
                                }
                                else
                                {
                                    if (*j > p1)
                                    {
                                        // a9 and a1
                                        rotations::rotate9(*i, *g4, *g3, *g2, *j, *g5, *g6, *g7, *g8);
                                        g2++, g3++, g4++;
                                        g5--, g6--, g7--, g8--;
                                    }
                                    else
                                    {
                                        // a9 and a0
                                        rotations::rotate10(*i, *g4, *g3, *g2, *g1, *j, *g5, *g6, *g7, *g8);
                                        g1++, g2++, g3++, g4++;
                                        g5--, g6--, g7--, g8--;
                                    }
                                }
                            }
                        }
                    }
                }
                i++, j--;
            }
        }

        rotate6(*(left + 4), *(g1 - 1), *(g2 - 1), *(g3 - 1), *(g4 - 1), *(i - 1));
        rotate5(*(left + 3), *(g1 - 2), *(g2 - 2), *(g3 - 2), *(g4 - 2));
        rotate4(*(left + 2), *(g1 - 3), *(g2 - 3), *(g3 - 3));
        rotate3(*(left + 1), *(g1 - 4), *(g2 - 4));
        rotations::swap(*left, *(g1 - 5));

        rotate5(*(right - 3), *(g8 + 1), *(g7 + 1), *(g6 + 1), *(g5 + 1));
        rotate4(*(right - 2), *(g8 + 2), *(g7 + 2), *(g6 + 2));
        rotate3(*(right - 1), *(g8 + 3), *(g7 + 3));
        rotations::swap(*(right), *(g8 + 4));

       
        nine_pivot(left  , g1 - 6);
        nine_pivot(g1 - 4, g2 - 5);
        nine_pivot(g2 - 3, g3 - 4);
        nine_pivot(g3 - 2, g4 - 3);
        nine_pivot(g4 - 1, i  - 2);
        nine_pivot(i     , g5    );
        nine_pivot(g5 + 2, g6 + 1);
        nine_pivot(g6 + 3, g7 + 2);
        nine_pivot(g7 + 4, g8 + 3);
        nine_pivot(g8 + 5, right );

    }
}


template <typename ValueType>
void qsort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    nine_pivot(input, input + n - 1);
}

CONTESTANT_REGISTER_ALL(qsort, "qsort9", "Quicksort, nine pivots")

} // namespace qsort9
