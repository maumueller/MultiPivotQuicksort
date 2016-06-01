/******************************************************************************
 * src/algorithms/ssortv1.h
 *
 * Super Scalar Sample Sort by Peter Sanders and Sebastian Winkel (2004)
 * using in-place permutation algorithm and storing classifications.
 *
 ******************************************************************************
 * Copyright (C) 2014 Timo Bingmann <tb@panthema.net>
 * Copyright (C) 2014 Martin Aumüller <martin.aumueller@tu-ilmenau.de>
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

namespace qsortpermutestore{

typedef size_t bktsize_t;
typedef unsigned int bktnum_t;

static const bool debug_splitter = false;
static const bool debug_classify = false;
static const bool debug_bktsize = false;
static const bool debug_permute = false;
static const bool debug_permute_in_loop = false;

//! Recursive TreeBuilder for full-descent and unrolled variants, constructs a
//! binary tree of splitters.
template <typename Iterator, size_t numsplitters>
struct TreeBuilder
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    value_type* m_tree;
    value_type* m_samples;
    unsigned int m_index;

    TreeBuilder(value_type* splitter_tree,
                value_type* samples, size_t samplesize)
        : m_tree( splitter_tree ),
          m_samples( samples ),
          m_index( 0 )
    {
        recurse(samples, samples + samplesize, 1);
    }

    ssize_t snum(value_type* s) const
    {
        return (ssize_t)(s - m_samples);
    }

    value_type recurse(value_type* lo, value_type* hi, unsigned int treeidx)
    {
        DBG(debug_splitter, "rec_buildtree(" << snum(lo) << "," << snum(hi)
            << ", treeidx=" << treeidx << ")");

        // pick middle element as splitter
        value_type* mid = lo + (ssize_t)(hi - lo) / 2;

        DBG(debug_splitter, "tree[" << treeidx << "] = samples[" << snum(mid) << "] = "
            << *mid);

        value_type mykey = m_tree[treeidx] = *mid;
#if 0
        value_type* midlo = mid;
        while (lo < midlo && *(midlo-1) == mykey) midlo--;

        value_type* midhi = mid;
        while (midhi+1 < hi && *midhi == mykey) midhi++;

        if (midhi - midlo > 1)
            DBG(0, "key range = [" << snum(midlo) << "," << snum(midhi) << ")");
#else
        value_type *midlo = mid, *midhi = mid+1;
#endif
        if (2 * treeidx < numsplitters)
        {
            recurse(lo, midlo, 2 * treeidx + 0);

            DBG(debug_splitter, "splitter[" << m_index++ << "] = " << mykey);

            return recurse(midhi, hi, 2 * treeidx + 1);
        }
        else
        {
            DBG(debug_splitter, "splitter[" << m_index++ << "] = " << mykey);

            return mykey;
        }
    }
};

template <typename Iterator, size_t treebits>
struct ClassifySimple
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    static const size_t numsplitters = (1 << treebits) - 1;

    value_type splitter_tree[numsplitters+1];

    /// build tree and splitter array from sample
    inline void
    build(value_type* samples, size_t samplesize)
    {
        TreeBuilder<Iterator, numsplitters>(splitter_tree, samples, samplesize);
    }

    void step(unsigned int& i, const value_type& key) const
    {
#if 0
        // in gcc-4.6.3 this produces a SETA, LEA sequence
        i = 2 * i + (key <= this->splitter_tree[i] ? 0 : 1);
#else
        // in gcc-4.6.3 this produces two LEA and a CMOV sequence, which is
        // slightly faster
        if (key <= this->splitter_tree[i])
            i = 2*i + 0;
        else // (key > splitter_tree[i])
            i = 2*i + 1;
#endif
    }

    /// search in splitter tree for bucket number
    inline unsigned int
    find_bkt_tree(const value_type& key) const
    {
        unsigned int i = 1;

        while ( i <= numsplitters )
        {
            step(i, key);
        }

        i -= numsplitters + 1;

        return i;
    }

    /// classify all strings in area by walking tree and saving bucket id
    inline void
    classify(Iterator begin, Iterator end, uint16_t* bktout) const
    {
        for (Iterator iter = begin; iter != end; )
        {
            value_type key = *iter++;
            uint16_t b = find_bkt_tree(key);
            DBG(debug_classify, "key[" << key << "] -> " << b);
            *bktout++ = b;
        }
    }

    /// search in splitter tree for bucket number, unrolled for U keys at once.
    template <int U>
    __attribute__((optimize("unroll-all-loops")))
    inline void
    find_bkt_tree_unroll(const value_type key[U], uint16_t obkt[U]) const
    {
        // binary tree traversal without left branch

        static const size_t numsplitters = this->numsplitters;

        unsigned int i[U];
        std::fill(i+0, i+U, 1);

        for (size_t l = 0; l < treebits; ++l)
        {
            for (int u = 0; u < U; ++u)
            {
                step(i[u], key[u]);
            }
        }

        for (int u = 0; u < U; ++u)
        {
            obkt[u] = i[u] - (numsplitters + 1);
        }
    }

    /// classify all strings in area by walking tree and saving bucket id,
    /// unrolled loops
    __attribute__((optimize("unroll-all-loops")))
    inline void
    classify_unroll(Iterator begin, Iterator end, uint16_t* bktout) const
    {
        static const int U = 2;

        for (Iterator iter = begin; iter != end; )
        {
            if (iter + U < end)
            {
                value_type key[U];

                for (int u = 0; u < U; ++u)
                    key[u] = *iter++;

                find_bkt_tree_unroll<U>(key, bktout);

                bktout += U;
            }
            else
            {
                // binary search in splitter with equal check
                *bktout++ = this->find_bkt_tree(*iter++);
            }
        }
    }
};


template <typename Iterator>
void sample_sort(Iterator begin, Iterator end)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    static const size_t numsplitters2 = 128;
    static const size_t treebits = logfloor_<numsplitters2>::value;
    static const size_t numsplitters = (1 << treebits) - 1;
    static const size_t bktnum = numsplitters + 1;

    size_t n = end - begin;

    // step 1: select splitters with oversampling

    static const size_t oversample = 2;
    static const size_t samplesize = oversample * numsplitters + (oversample-1);

    value_type samples[ samplesize ];

    for (unsigned int i = 0; i < samplesize; ++i)
    {
        samples[i] = *(begin + i);
    }

    std::sort(samples, samples + samplesize);

    for (size_t i = 0; i < samplesize; ++i)
    {
        DBG(debug_permute, "samples[" << i << "] = " << samples[i]);
    }


    // build classification tree

    ClassifySimple<Iterator, treebits> classifier;
    classifier.build(samples, samplesize);

    // step 2: count bucket sizes
    uint16_t* bktcache = new uint16_t[n];
    classifier.classify_unroll(begin, end, bktcache);

    bktsize_t bktcnt[bktnum+1];
    memset(bktcnt, 0, (bktnum+1) * sizeof(bktsize_t));
    
    for (size_t i = 0; i < n; ++i)
        ++bktcnt[ bktcache[i] ];

    //classifier.bucket_sizes_unroll(begin, end, bkts + 1);

    //g_assignments += n - 127;
    
    for (bktnum_t i = 0; i < bktnum; ++i)
    {
        DBG(debug_permute, "bktsize[" << i << "] = " << bktcnt[i]);
    }
    
    
    // step 3: exclusive prefix sum
    
    bktsize_t bktnext[bktnum];
    bktnext[0] = 0 ;
    for (unsigned int i = 1; i < bktnum; ++i) {
        bktnext[i] = bktnext[i-1] + bktcnt[i - 1];
    }

    for (bktnum_t i = 0; i < bktnum; ++i)
    {
        DBG(debug_permute, "bktstart[" << i << "] = " << bktnext[i]);
    }

    for (Iterator iter = begin; iter != end; ++iter)
    {
        DBG(debug_permute, "before loop:  A[" << (iter-begin) << "] = " << *iter);
        DBG(debug_permute, "before loop:  bktcache[" << (iter-begin) << "] = " << bktcache[iter - begin]);
    }

    // step 4: permute in-place

    // bktnext[i] == position next to be filled with an element for bucket i,
    //               initially equal to bkts
    
    size_t bkts[bktnum + 1]; 
    memcpy(bkts, bktnext, bktnum * sizeof(size_t));
    bkts[bktnum] = n;

    value_type* hole;
    value_type* from;
    value_type* to;
    value_type key;
    size_t bkt;
    size_t to_bkt;
    

    for (size_t b = 0; b < bktnum - 1; b++)
    {
        while (bktcnt[b] > 0)
        {
            while (bktcnt[b] > 0 && (bkt = bktcache[bktnext[b]]) == b)
            {
                DBG(debug_permute_in_loop, "Skipping " << bktnext[b]);
                bktnext[b]++;
                bktcnt[b]--;
            }
            if (bktcnt[b] == 0)
                break;
            hole = begin + bktnext[b];
            from = hole;
            key = *hole;
            do
        {
                while ((to_bkt = bktcache[bktnext[bkt]]) == bkt)
            {
                    bktnext[bkt]++;
                    bktcnt[bkt]--;
            }
                to = begin + bktnext[bkt]++;
                bktcnt[to_bkt]--;
                std::swap(key, *to);
            //g_assignments += 2;
                hole = to;
                bkt = to_bkt;
            } while (from != hole);
        }
    }

    for (Iterator iter = begin; iter != end; ++iter)
    {
        DBG(debug_permute, "after loop:  A[" << (iter-begin) << "] = " << *iter);
    }
    

    // step 5: recurse

    for (bktnum_t i = 0; i < bktnum; ++i)
    {
        // go into recursive subproblems
        if (bkts[i+1] - bkts[i] <= 1024) // samplesize)
        {
            DBG(debug_permute, "rec call on A[" << bkts[i] << "] to A[" << bkts[i + 1] - 1 << "]");
            qsort3::three_pivot(begin + bkts[i], begin + bkts[i+1]-1); // inclusive end!
        }
        else
            sample_sort(begin + bkts[i], begin + bkts[i+1]);
    }

    for (Iterator iter = begin; iter != end; ++iter)
    {
        DBG(debug_permute, "after rec calls:  A[" << (iter-begin) << "] = " << *iter);
    }
#undef bktsize

}

template <typename ValueType>
void issort()
{
    assert( g_input_size % sizeof(ValueType) == 0 );

    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    sample_sort(input, input + n);
}

CONTESTANT_REGISTER_ALL(issort, "qsort-permute-store", "In-place Super Scalar Sample Sort storing classifications")

} 
