/******************************************************************************
 * src/algorithms/qsort3sample.h
 *
 * Sorting using Quicksort, three pivots. Sample first, then using the best comparison tree.
 *
 ******************************************************************************
 * Copyright (C) 2016 Martin Aumueller <martin.aumueller@tu-ilmenau.de>
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
#include <array>

namespace qsort3sample {

using ushort = unsigned short;

template <typename ValueType>
inline ushort l1(ValueType& elem, ValueType& p1, ValueType& p2, ValueType& p3)
{
    if (elem < p1)
        return 0;
    if (elem < p2)
        return 1;
    if (elem < p3)
        return 2;
    return 3;
}
inline ushort l1(unsigned int& elem, unsigned int& p1, unsigned int& p2, unsigned int& p3)
{
    if (elem < p1)
        return 0;
    if (elem < p2)
        return 1;
    if (elem < p3)
        return 2;
    return 3;
}

template <typename ValueType>
inline ushort l2(ValueType& elem, ValueType& p1, ValueType& p2, ValueType& p3)
{
    if (elem < p1)
        return 0;
    if (elem > p3)
        return 3;
    if (elem < p2)
        return 1;
    return 2;

}

template <typename ValueType>
inline ushort l3(ValueType& elem, ValueType& p1, ValueType& p2, ValueType& p3)
{
    if (elem < p2)
    {
        if (elem < p1)
            return 0;
        return 1;
    }
    else
    {
        if (elem < p3)
        {
            return 2;
        }
        return 3;
    }
}

template <typename ValueType>
inline ushort l4(ValueType& elem, ValueType& p1, ValueType& p2, ValueType& p3)
{
    if (elem > p3)
        return 3;
    if (elem < p1)
        return 0;
    if (elem < p2)
        return 1;
    return 2;
}

template <typename ValueType>
inline ushort l5(ValueType& elem, ValueType& p1, ValueType& p2, ValueType& p3)
{
    if (elem > p3)
        return 3;
    if (elem > p2)
        return 2;
    if (elem > p1)
        return 1;
    return 0;
}

template <typename ValueType>
inline ushort choosetree(ValueType&& a0, ValueType&& a1, ValueType&& a2, ValueType&& a3)
{
    if (a3 > a1)
    {
        if (a0 > a2)
        {
            if (a3 > a0 && a1 + a2 < a3)
            {
                return 4;
            }
            else if (a3 < a0 && a2 + a3 < a0)
            {
                return 2;
            }
        }
        else if (a0 + a1 < a3)
        {
            return 5;
        }
    }
    else if (a3 + a2 < a0)
    {
        return 1;
    }
    return 3;
}

template <typename ValueType>
inline ushort getclass(ValueType& elem, ValueType& p1, ValueType& p2, 
        ValueType& p3, size_t& a0, size_t& a1, size_t& a2, size_t& a3)
{
        return getclassfortree(choosetree(a0, a1, a2, a3), elem, p1, p2, p3);
}

template <typename ValueType>
inline ushort getclassfortree(ushort tree, ValueType& elem, ValueType& p1,
        ValueType& p2, ValueType& p3)
{
        switch (tree)
        {
            case 1:
                return l1(elem, p1, p2, p3);
            case 2:
                return l2(elem, p1, p2, p3);
            case 3:
                return l3(elem, p1, p2, p3);
            case 4:
                return l4(elem, p1, p2, p3);
            case 5:
                return l5(elem, p1, p2, p3);
        }
        return 0;

}

template <typename Iterator>
void three_pivot(Iterator left, Iterator right)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    using rotations::swap;

//    if (left >= right) return;
    if (right - left < 1024)
    {
        qsort3::three_pivot(left, right);
        return;
    }

    Iterator i = left + 2;
    Iterator j = i;

    Iterator k = right - 1;
    Iterator l = k;

    if (*left > *(left+1))
        swap(*left, *(left+1));

    if (*left > *right)
    {
        swap(*left, *(left+1));
        swap(*left, *right);
    }
    else if (*(left + 1) > *right)
    {
        swap(*(left+1), *right);
    }

    value_type p = *left;
    value_type q = *(left+1);
    value_type r = *right;

#if 0
    std::cout << "Before partitioning" << std::endl;
    for (auto it = left; it <= right; it++)
        std::cout << *it << " ";
    std::cout << std::endl;
#endif 

    auto max_sample = static_cast<size_t>((right - left) >> 5);
    size_t se = 0;

    ushort cj = 0;
    ushort ck = 0;

    while (se < max_sample)
    {
        if (*j < q)
        {
            if (*j < p)
            {
                swap(*i, *j);
                i++;
            }
        }
        else
        {
            while (*k > q && se < max_sample)
            {
                if (*k > r)
                {
                    swap(*k, *l);
                    l--;
                }
                k--;
                se++;
            }
            if (se < max_sample)
            {
                if (*j < r)
                {
                    if (*k < p)
                    {
                        rotations::rotate3(*j, *i, *k);
                        i++;
                    }
                    else
                    {
                        swap(*j, *k);
                    }
                }
                else
                {
                    if (*k < p)
                    {
                        rotations::rotate4(*j, *i, *k, *l);
                        i++;
                    }
                    else
                    {
                        rotations::rotate3(*j, *k, *l);
                    }
                    l--;
                }
            }
            else
                break;
            k--;
        }
        j++;
        se++;
    }
    auto t = choosetree(i - left, j - i, l - k, right - l);

    while (j <= k)
    {
        cj = getclassfortree(t, *j, p, q, r);
        if (cj == 0)
        {
            swap(*i, *j);
            i++;
        }
        else if (cj > 1)
        {
            while ((ck = getclassfortree(t,*k, p, q, r)) > 1)
            {
                if (ck == 3)
                {
                    swap(*k, *l);
                    l--;
                }
                k--;
            }
            if (j < k)
            {
                if (cj == 2)
                {
                    if (ck == 0)
                    {
                        rotations::rotate3(*j, *i, *k);
                        i++;
                    }
                    else
                    {
                        swap(*j, *k);
                    }
                }
                else
                {
                    // cj is 3
                    if (ck == 0)
                    {
                        rotations::rotate4(*j, *i, *k, *l);
                        i++;
                    }
                    else
                    {
                        rotations::rotate3(*j, *k, *l);
                    }
                    l--;
                }
            }
            else
            {
                break;
            }
            k--;
        }
        j++;
    }

#if 0
    std::cout << "Before swapping in" << std::endl;
    std::cout << "i: " << (i - left) << " j: " << (j - left) <<
        " k: " << (k - left) << std::endl;
    for (auto it = left; it <= right; it++)
        std::cout << *it << " ";
    std::cout << std::endl;
#endif

    rotations::rotate3(*(left + 1), *(i - 1), *(j - 1));

    swap(*left, *(i - 2));
    swap(*right, *(l + 1));

#if 0    
    std::cout << "After partitioning" << std::endl;
    for (auto it = left; it <= right; it++)
        std::cout << *it << " ";
    std::cout << std::endl;
#endif

    three_pivot(left, i - 3);
    three_pivot(i - 1, j - 2);
    three_pivot(j, l);
    three_pivot(l + 2, right);
}

template <typename ValueType>
void qsort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    three_pivot(input, input + n-1);
}

CONTESTANT_REGISTER_ALL(qsort, "qsort3sample", "Comparison-optimal Quicksort, sampling, three pivots")

} // namespace qsort3
