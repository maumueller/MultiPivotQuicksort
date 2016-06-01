/******************************************************************************
 * src/tools/input.h
 *
 * Tools to generate input data
 *
 ******************************************************************************
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
#include <limits>

namespace input {

/// Parse a string like "343KB" or "44g" into the corresponding size in bytes
bool parse_filesize(const char* str, size_t& outsize)
{
    char* endptr;
    outsize = strtoul(str,&endptr,10);
    if (!endptr) return false;

    if ( *endptr == 0 || ( (*endptr == 'b' || *endptr == 'B') && *(endptr+1) == 0) )
        outsize *= 1;
    else if ( (*endptr == 'k' || *endptr == 'K') &&
              (*(endptr+1) == 0 || ( (*(endptr+1) == 'b' || *(endptr+1) == 'B') && *(endptr+2) == 0) ) )
        outsize *= 1024;
    else if ( (*endptr == 'm' || *endptr == 'M') &&
              (*(endptr+1) == 0 || ( (*(endptr+1) == 'b' || *(endptr+1) == 'B') && *(endptr+2) == 0) ) )
        outsize *= 1024*1024;
    else if ( (*endptr == 'g' || *endptr == 'G') &&
              (*(endptr+1) == 0 || ( (*(endptr+1) == 'b' || *(endptr+1) == 'B') && *(endptr+2) == 0) ) )
        outsize *= 1024*1024*1024;
    else
        return false;

    return true;
}

template <typename ValueType>
struct free_data {
    void operator()() const
    {
        if (g_input)
        {
            delete [] (ValueType*)g_input;
            g_input = NULL;
        }
    }
};

static boost::mt19937_64 gen;

inline size_t rand_int(int bound)
{
    boost::uniform_int<size_t> dis(0, bound - 1);
    return dis(gen);
}


template <typename ValueType>
struct generate_random {
    void operator()() const
    {
        // free previous data file
        free_data<ValueType>();
        
        if (g_input_size % sizeof(ValueType) != 0) {
            std::cout << "Input size not divisable by typesize." << std::endl;
            abort();
        }

        size_t n = g_input_size / sizeof(ValueType);

        ValueType* input = new ValueType[n];
        g_input = (void*)input;
        
        boost::uniform_int<ValueType> dis(0, std::numeric_limits<ValueType>::max());
        boost::variate_generator<boost::mt19937_64&,boost::uniform_int<ValueType> > rand (gen, dis);

        for (size_t i = 0; i < n; ++i)
        {
            input[i] = ValueType(rand());
        }
    }
};

template <typename ValueType>
struct generate_permutation {
    void operator()() const
    {
        // free previous data file
        free_data<ValueType>();

        if (g_input_size % sizeof(ValueType) != 0) {
            std::cout << "Input size not divisable by typesize." << std::endl;
            abort();
        }

        size_t n = g_input_size / sizeof(ValueType);

        ValueType* input = new ValueType[n];
        g_input = (void*)input;

        for (size_t i = 0; i < n; ++i)
        {
            input[i] = i;
        }

        std::random_shuffle(input, input + n, rand_int);
    }
};

template <typename ValueType>
struct generate_permutation_sentinel {
    void operator()() const
    {
        // free previous data file
        free_data<ValueType>();

        if (g_input_size % sizeof(ValueType) != 0) {
            std::cout << "Input size not divisable by typesize." << std::endl;
            abort();
        }

        size_t n = g_input_size / sizeof(ValueType);

        ValueType* input = new ValueType[n+1];
        g_input = (void*)(input);

        for (size_t i = 1; i <= n; ++i)
        {
            input[i] = i;
        }

        std::random_shuffle(input + 1, input + n + 1, rand_int);

        input[0] = std::numeric_limits<ValueType>::min();
    }
};


bool load_artifical(const std::string& path)
{
    if (path == "random") {
        type_switch<generate_random>();
        return true;
    }
    else if (path == "permutation") {
        type_switch<generate_permutation>();
        return true;
    }
    else if (path == "sentinel-permutation") {
        type_switch<generate_permutation_sentinel>();
        return true;
    }

    return false;
}

/// Load an input set into memory
bool load(const std::string& path)
{

    if (load_artifical(path)) {
        g_input_name = path;
    }
    else {
        std::cout << "Unknown input " << path << "\n";
        return false;
    }

    return true;
}

} // namespace input
