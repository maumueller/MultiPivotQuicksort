/******************************************************************************
 * src/algorithms/qsort2v3.h
 *
 * Sorting using Dual Pivot Quicksort Sampling approach
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

namespace qsort2v3 {

template <typename Iterator>
void ptrDualPivotSampling(Iterator lo, Iterator hi)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    //    if (lo >= hi) return;
    if (lo + 1024 > hi)
    {
        qsort2v5::ptrDualPivotLargerFirst(lo, hi);
        return;
    }

    if (*lo > *hi)
        std::swap(*lo, *hi);

    const value_type p = *lo;
    const value_type q = *hi;

    Iterator i = lo + 1;
    Iterator k = hi - 1;
    Iterator j = i;

    int maxCnt = (hi - lo) >> 10;
    int cnt = 0;

    while (cnt < maxCnt)
    {
        while (q < *k)
        {
            k--;
            cnt++;
        }

        while (*j < q)
        {
            if (*j < p)
            {
                std::swap(*j, *i);
                i++;
            }
            j++;
            cnt++;
        }

        if (j < k)
        {
            if (*k < p)
            {
                rotations::rotate3(*k, *j, *i);
                i++;
            }
            else
            {
                std::swap(*j, *k);
            }
            k--;
        }
        j++;
    }

    if ((i - lo) >= (hi - k))
    {
        while (j <= k)
        {
            if (*j < p)
            {
                std::swap(*i, *j);
                i++;
                j++;
            }
            else
            {
                if (*j < q)
                    j++;
                else
                {
                    std::swap(*j, *k);
                    k--;
                }
            }
        }
    }
    else
    {
        while (j <= k)
        {
            while (q < *k)
            {
                k--;
            }

            while (*j < q)
            {
                if (*j < p)
                {
                    std::swap(*j, *i);
                    i++;
                }
                j++;
            }

            if (j < k)
            {
                if (*k < p)
                {
                    rotations::rotate3(*k, *j, *i);
                    i++;
                }
                else
                {
                    std::swap(*j, *k);
                }
                k--;
            }
            j++;
        }

    }

    std::swap(*lo, *(i - 1));
    std::swap(*hi, *(k + 1));

    ptrDualPivotSampling(lo, i - 2);
    ptrDualPivotSampling(i, k);
    ptrDualPivotSampling(k + 2, hi);
}

template <typename ValueType>
void qsort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    ptrDualPivotSampling(input, input + n-1);
}

CONTESTANT_REGISTER_ALL(qsort, "qsort2-sample", "Quicksort, dual pivot, sampling approach")

} // namespace qsort2v3
