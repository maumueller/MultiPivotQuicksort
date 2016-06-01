/******************************************************************************
 * src/tools/contest.h
 *
 * Class to register contestants for speed test.
 *
 ******************************************************************************
 * Copyright (C) 2012-2014 Timo Bingmann <tb@panthema.net>
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

#ifndef TOOLS_CONTEST_H
#define TOOLS_CONTEST_H

#include <stdlib.h>
#include <string.h>
#include <vector>

#include "stats_writer.h"

extern stats_writer g_stats;

class Contestant;

class Contest
{
public:
    typedef std::vector<Contestant*>    list_type;

    list_type           m_list;

    void register_contestant(Contestant* c)
    {
        m_list.push_back(c);
    }

    void run_contest(const char* path); // implemented in main.cc

    bool exist_contestant(const char* algoname); // implemented in main.cc

    void list_contentants(); // implemented in main.cc
};

extern Contest* getContestSingleton();

class Contestant
{
public:
    const char*         m_algoname;
    const char*         m_description;
    unsigned            m_typesize;

    Contestant(const char* algoname,
               const char* description,
               unsigned typesize)
        : m_algoname(algoname),
          m_description(description),
          m_typesize(typesize)
    {
        getContestSingleton()->register_contestant(this);
    }

    virtual void run() = 0; // depends on individual sorter's interface

    inline bool operator< (const Contestant &b) const
    {
        if (strcmp(m_algoname, b.m_algoname) == 0)
            return m_typesize < b.m_typesize;

        return (strcmp(m_algoname, b.m_algoname) < 0);
    }
};

static inline bool sort_contestants(const Contestant *a, const Contestant *b) {
    return (*a < *b);
}

class Contestant_Array : public Contestant
{
public:
    typedef void (*func_type)();

    func_type           m_run_func;

    Contestant_Array(func_type run_func,
                     const char* algoname,
                     const char* description,
                     unsigned typesize)
        : Contestant(algoname, description, typesize),
          m_run_func(run_func)
    {
    }

    virtual void run();         // implemented in main.cc
    void         run_forked();  // implemented in main.cc
    void         real_run();    // implemented in main.cc
};

#define CONTESTANT_REGISTER_ONE(call, algoname, desc, func, type)              \
    static const class Contestant* _Contestant_##func##type##_register =       \
        new Contestant_Array(call, algoname, desc, sizeof(type));

#define CONTESTANT_REGISTER_ALL(func, algoname, desc)                          \
    CONTESTANT_REGISTER_ONE(func<unsigned>, algoname, desc, func, unsigned)    \
    CONTESTANT_REGISTER_ONE(func<uint64_t>, algoname, desc, func, uint64_t)    \

#endif // TOOLS_CONTEST_H
