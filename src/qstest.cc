/******************************************************************************
 * src/qstest.cc
 *
 * Quicksort experiments program
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

#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <string>
#include <vector>
#include <stack>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <sys/wait.h>

#include <boost/random.hpp>
#include <getopt.h>

#include "tools/stats_writer.h"
#include "tools/papi.h"

#ifdef MALLOC_COUNT
#include "tools/malloc_count.h"
#include "tools/stack_count.h"
#include "tools/memprofile.h"

static const char* memprofile_path = "memprofile.txt";
#endif

#define LIKELY(c)   __builtin_expect((c), 1)
#define UNLIKELY(c)   __builtin_expect((c), 0)

// *** Global Command Line Parameters ***

size_t          gopt_inputsize_minlimit = 0;    // lower limit to multi-size runs
size_t          gopt_inputsize_maxlimit = 0;    // upper limit to multi-size runs
size_t          gopt_repeats_outer = 1;         // argument -R outer loop repeats
size_t          gopt_repeats_inner = 1;         // argument -r inner loop repeats
size_t          gopt_repeats_divider = 1;       // current power of two of inputsize
std::vector<const char*> gopt_algorithm_substr; // argument -a
std::vector<const char*> gopt_algorithm_exact;  // argument -A
int             gopt_timeout = 0;               // argument -T, --timeout

const char*     gopt_input_write = NULL;        // argument -i, --input
const char*     gopt_output_write = NULL;       // argument -o, --output

bool            gopt_no_check = false;          // argument --no-check

unsigned        gopt_typesize = 8;              // argument -t <type>

bool            gopt_forkrun = false;
bool            gopt_forkdataload = false;

const char*     gopt_papi_events = NULL;        // argument -P, --papi

std::vector<size_t> gopt_seeds;                  // argument  -z, --seed

// *** Global Data Structures ***

std::string     g_input_name;           // description of input
void*           g_input = NULL;         // pointer to first item
size_t          g_input_size = 0;       // total size of data
size_t          g_seed;                 // current seed for rng
size_t		g_id = 0;
size_t          g_pivot_switch = 0; 
size_t          g_block_size = 128; 

long long       g_assignments = 0;
long long       scanned_element_cnt = 0;

stats_writer    g_stats;

// *** Type Switch Template ***

template <template <typename ValueType> class Functional>
void type_switch()
{
    if (gopt_typesize == 4)
        Functional<unsigned>()();
    else if (gopt_typesize == 8)
        Functional<uint64_t>()();
    else
        abort();
}

// *** Tools and Algorithms ***

#include "tools/debug.h"
#include "tools/lcgrandom.h"
#include "tools/contest.h"
#include "tools/input.h"
#include "tools/logfloor.h"

// *** Algorithms ***

#include "algorithms/networks.h"
#include "algorithms/rotations.h"
#include "algorithms/inssort.h"

#include "algorithms/included_algorithms.h"


// *** Main Experimental Framework ***

template <typename ValueType>
bool check_sorted_order_type()
{
    ValueType* input = (ValueType*)g_input;
    size_t n = g_input_size / sizeof(ValueType);

    for (size_t i = 1; i < n; ++i)
    {
        if (input[i-1] > input[i]) return false;
    }

    return true;
}

bool check_sorted_order()
{
    if (gopt_typesize == 4)      return check_sorted_order_type<unsigned>();
    else if (gopt_typesize == 8) return check_sorted_order_type<uint64_t>();
    else abort();
}

Contest* getContestSingleton()
{
    static Contest* c = NULL;
    if (!c) c = new Contest;
    return c;
}

static inline bool gopt_algorithm_select(const Contestant* c)
{
    if (c->m_typesize != gopt_typesize) return false;

    if (gopt_algorithm_substr.size() || gopt_algorithm_exact.size())
    {
        // iterate over gopt_algorithm_substr list as a filter
        for (size_t ai = 0; ai < gopt_algorithm_substr.size(); ++ai) {
            if (strstr(c->m_algoname, gopt_algorithm_substr[ai]) != NULL)
                return true;
        }

        // iterate over gopt_algorithm_exact list as a exact match filter
        for (size_t ai = 0; ai < gopt_algorithm_exact.size(); ++ai) {
            if (strcmp(c->m_algoname, gopt_algorithm_exact[ai]) == 0)
                return true;
        }

        return false;
    }

    return true;
}

template <typename ValueType>
struct write_input {
    void operator()() const
    {
        std::cout << "Writing unsorted input to " << gopt_input_write << std::endl;
        std::ofstream f(gopt_input_write);

        ValueType* input = (ValueType*)g_input;
        size_t n = g_input_size / sizeof(ValueType);

        for (size_t i = 0; i < n; ++i)
            f << input[i] << '\n';
    }
};

static inline void maybe_inputwrite()
{
    if (!gopt_input_write) return;
    type_switch<write_input>();
}

template <typename ValueType>
struct write_output {
    void operator()() const
    {
        std::cout << "Writing sorted output to " << gopt_output_write << std::endl;
        std::ofstream f(gopt_output_write);

        ValueType* input = (ValueType*)g_input;
        size_t n = g_input_size / sizeof(ValueType);

        for (size_t i = 0; i < n; ++i)
            f << input[i] << '\n';
    }
};

void Contest::run_contest(const char* dataname)
{
    g_input_name = dataname;

//    if (!gopt_forkdataload)
//    {
//        // read input datafile
//        if (!input::load(g_input_name))
//            return;
//
//        maybe_inputwrite();
//
//        std::cout << "Sorting " << g_input_size << " bytes = "
//                  << g_input_size / gopt_typesize << " items of size "
//                  << gopt_typesize << "" << std::endl;
//    }

    std::stable_sort(m_list.begin(), m_list.end(), sort_contestants);

    for (size_t r = 0; r < gopt_repeats_outer; ++r)
    {
	if (!gopt_forkdataload)
	{
	    if (!input::load(g_input_name))
		return;
	}
	// iterate over all contestants
	for (list_type::iterator c = m_list.begin(); c != m_list.end(); ++c)
	{
	    if (gopt_algorithm_select(*c))
	    {
		(*c)->run();
	    }
	}
        type_switch<input::free_data>();
	g_id++;
    }
}

bool Contest::exist_contestant(const char* algoname)
{
    for (size_t i = 0; i < m_list.size(); ++i) {
        if (strcmp(m_list[i]->m_algoname, algoname) == 0)
            return true;
    }

    return false;
}

void Contest::list_contentants()
{
    std::cout << "Available algorithms:" << std::endl;

    std::stable_sort(m_list.begin(), m_list.end(), sort_contestants);

    size_t w_algoname = 0;
    for (list_type::iterator c = m_list.begin(); c != m_list.end(); ++c)
    {
        if (!gopt_algorithm_select(*c)) continue;
        w_algoname = std::max(w_algoname, strlen( (*c)->m_algoname ));
    }

    // iterate over all contestants
    std::string prev;
    for (list_type::iterator c = m_list.begin(); c != m_list.end(); ++c)
    {
        if (!gopt_algorithm_select(*c)) continue;
        if ((*c)->m_algoname == prev) continue;

        std::cout << std::left << std::setw(w_algoname) << (*c)->m_algoname
                  << "  " << (*c)->m_description << std::endl;

        prev = (*c)->m_algoname;
    }

    if (w_algoname == 0)
        std::cout << "Selected algorithm set is empty." << std::endl;
}

void Contestant_Array::real_run()
{
    std::cout << "Running " << m_algoname << " - " << m_description << std::endl;

    g_stats >> "algo" << m_algoname
            >> "id" << g_id
	    >> "seed" << g_seed
            >> "input" << g_input_name
            >> "size" << g_input_size
            >> "typesize" << gopt_typesize
            >> "items" << g_input_size / gopt_typesize;

#ifdef MALLOC_COUNT
    //MemProfile memprofile( m_algoname, memprofile_path );
    size_t memuse = malloc_count_current();
    void* stack = stack_count_clear();
    malloc_count_reset_peak();
#endif

    PApiWrapper papi;

    char* input_shadow = new char[g_input_size];
    memcpy(input_shadow, g_input, g_input_size);

#if WITH_PAPI
    // see output of "papi_avail"

    if (gopt_papi_events)
    {
        papi.add_event_list(gopt_papi_events);
    }
    else if (papi.available())
    {
        // add a few default events

        papi.add_event(PAPI_TOT_INS); // Total Instructions
        papi.add_event(PAPI_TOT_CYC); // Total Cycles

        papi.add_event(PAPI_L2_TCM); // L2 Total Cache Misses

        //papi.add_event(PAPI_BR_CN); // Conditional branch instructions executed
        //papi.add_event(PAPI_BR_TKN); // Conditional branch instructions taken
        //papi.add_event(PAPI_BR_NTK); // Conditional branch instructions not taken
        papi.add_event(PAPI_BR_MSP); // Conditional branch instructions mispred
        //papi.add_event(PAPI_BR_PRC); // Conditional branch instructions correctly predicted
    }
#else
    if (gopt_papi_events)
        DBG(1, "PAPI performance monitoring is not available!");
#endif



    size_t repeats_inner = gopt_repeats_inner / gopt_repeats_divider;
    if (repeats_inner == 0) repeats_inner = 1;
	
		auto start = std::chrono::high_resolution_clock::now();
		auto end = std::chrono::high_resolution_clock::now();

    if (repeats_inner == 1)
    {
		start = std::chrono::high_resolution_clock::now();
        papi.start();
        {
            m_run_func();
        }
		end = std::chrono::high_resolution_clock::now();
        papi.stop();
    }
    else
    {
        // copy unsorted array
        char* input_copy = new char[g_input_size];
        memcpy(input_copy, g_input, g_input_size);

        papi.start(); 
		start = std::chrono::high_resolution_clock::now();
        for (size_t rep = 0; rep < repeats_inner; ++rep)
        {
            // restore array
            if (rep != 0)
                memcpy(g_input, input_copy, g_input_size);

            m_run_func();
        }
		end = std::chrono::high_resolution_clock::now();
        papi.stop();

        delete [] input_copy;
    }

#ifdef MALLOC_COUNT
    std::cout << "Max stack usage: " << stack_count_usage(stack) << std::endl;
    std::cout << "Max heap usage: " << malloc_count_peak() - memuse << std::endl;
    //memprofile.finish();

    g_stats >> "heapuse" << (malloc_count_peak() - memuse)
            >> "stackuse" << stack_count_usage(stack);

    if (memuse < malloc_count_current())
    {
        std::cout << "MEMORY LEAKED: " << (malloc_count_current() - memuse) << " B" << std::endl;
    }
#endif

    g_stats >> "time" <<  std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
            >> "repeats_inner" << repeats_inner
            >> "pivot_switch" << g_pivot_switch 
            >> "block_size" << g_block_size
            >> "assignments" << g_assignments
            >> "scanned-elements" << scanned_element_cnt;


    g_assignments = 0;
    scanned_element_cnt = 0;

    if (papi.available())
    {
        papi.report();

        for (size_t i = 0 ; i < papi.get_num_counter(); ++i)
        {
            g_stats >> papi.get_counter_name(i) << papi.get_counter_result(i);
        }
    }

    if (!gopt_no_check)
    {
        if (check_sorted_order()) {
            std::cout << "ok" << std::endl;
            g_stats >> "status" << "ok";
        }
        else {
            std::cout << "FAILED" << std::endl;
            g_stats >> "status" << "failed";
        }
    }
    else
    {
        std::cout << "skipped" << std::endl;
    }

    // print timing data out to stdout
    std::cout << g_stats << std::endl;
    g_stats.clear();

    memcpy(g_input, input_shadow, g_input_size);
    delete [] input_shadow;

    if (gopt_output_write)
    {
        type_switch<write_output>();
        exit(0);
    }
}

void Contestant_Array::run_forked()
{
    if (!gopt_forkrun) return real_run();

    pid_t p = fork();
    if (p == 0)
    {
        std::cout << "fork() ------------------------------------------------------------" << std::endl;

        if (gopt_forkdataload)
        {
            if (!input::load(g_input_name)) return;

            maybe_inputwrite();

            std::cout << "Sorting " << g_input_size << " bytes = "
                      << g_input_size / gopt_typesize << " items of size "
                      << gopt_typesize << "" << std::endl;
        }

        // terminate child program after gopt_timeout seconds
        if (gopt_timeout) alarm(gopt_timeout);

        real_run();

        type_switch<input::free_data>();
        exit(0);
    }

    int status = 0;
    wait(&status);

    if (WIFEXITED(status)) {
        //std::cout << "Child normally with return code " << WEXITSTATUS(status) << std::endl;
    }
    else if (WIFSIGNALED(status)) {
        std::cout << "Child terminated abnormally with signal " << WTERMSIG(status) << std::endl;

        // write out exit status information to results file

        g_stats >> "algo" << m_algoname
                >> "input" << g_input_name
                >> "size" << g_input_size;

        if (WTERMSIG(status) == SIGALRM)
        {
            g_stats >> "status" << "timeout"
                    >> "timeout" << gopt_timeout;
        }
        else if (WTERMSIG(status) == SIGSEGV)
        {
            g_stats >> "status" << "segfault";
        }
        else if (WTERMSIG(status) == SIGABRT)
        {
            g_stats >> "status" << "aborted";
        }
        else
        {
            g_stats >> "status" << "SIG" << WTERMSIG(status);
        }

        std::cout << g_stats << std::endl;
    }
    else {
        std::cout << "Child wait returned with status " << status << std::endl;

        g_stats >> "algo" << m_algoname
                >> "input" << g_input_name
                >> "size" << g_input_size
                >> "status" << "weird";

        std::cout << g_stats << std::endl;
    }

    if (gopt_output_write) exit(0);
    g_stats.clear();
}

void Contestant_Array::run()
{
    run_forked();
}

void print_usage(const char* prog)
{
    std::cout << "Usage: " << prog << " [options] <input type>" << std::endl
              << "Options:" << std::endl
              << "  -a, --algo <match>     Run only algorithms containing this substring, can be used multile times. Try \"list\"." << std::endl
              << "  -A, --algoname <name>  Run only algorithms fully matching this string, can be used multile times. Try \"list\"." << std::endl
              << "  -D, --datafork         Fork before running algorithm and load data within fork!" << std::endl
              << "  -F, --fork             Fork before running algorithm, but load data before fork!" << std::endl
              << "  -i, --input <path>     Write unsorted input strings to file, usually for checking." << std::endl
              << "  -N, --no-check         Skip checking of sorted order." << std::endl
              << "  -o, --output <path>    Write sorted strings to output file, terminate after first algorithm run." << std::endl
              << "  -p, --pivot-switch     Input size on which pivot strategies are switched." << std::endl
              << "  -P, --papi <events>    Use PAPI library to monitor these events." << std::endl
              << "  -r, --repeat-inner <n> Repeat inner experiment loop a number of times and divide by repetition count." << std::endl
              << "  -R, --repeat-outer <n> Repeat experiment a number of times." << std::endl
              << "  -s, --size <size>      Limit the input size to this number of characters." << std::endl
              << "  -S, --maxsize <size>   Run through powers of two for input size limit." << std::endl
              << "  -t, --type <w>         Run algorithms with type of size w." << std::endl
              << "  -T, --timeout <sec>    Abort algorithms after this timeout (default: disabled)." << std::endl
              << "  -z, --seed             Set random seed, can be used multiple times." << std::endl
        ;
}

int main(int argc, char* argv[])
{
    static const struct option longopts[] = {
        { "help",    no_argument,        0, 'h' },
        { "algo",    required_argument,  0, 'a' },
        { "algoname", required_argument, 0, 'A' },
        { "fork",    no_argument,        0, 'F' },
        { "datafork", no_argument,       0, 'D' },
        { "input",   required_argument,  0, 'i' },
        { "no-check", no_argument,       0, 'N' },
        { "output",  required_argument,  0, 'o' },
        { "pivot-switch",    required_argument,  0, 'p' },
        { "papi",    required_argument,  0, 'P' },
        { "repeat-inner", required_argument, 0, 'r' },
        { "repeat-outer", required_argument, 0, 'R' },
        { "size",    required_argument,  0, 's' },
        { "maxsize", required_argument,  0, 'S' },
        { "type",    required_argument,  0, 't' },
        { "timeout", required_argument,  0, 'T' },
        { "seed",    required_argument,  0, 'z' },
        { 0,0,0,0 },
    };

#ifndef GIT_VERSION_SHA1
#define GIT_VERSION_SHA1 "unknown"
#endif

    {
        char hostname[128];
        gethostname(hostname, sizeof(hostname));
        std::cout << "Running quicksort test " << GIT_VERSION_SHA1
                  << " on " << hostname << std::endl;

        std::cout << "Called as";
        for (int i = 0; i < argc; ++i)
            std::cout << " " << argv[i];
        std::cout << std::endl;
    }

#ifdef MALLOC_COUNT
    if (truncate(memprofile_path, 0)) {
        perror("Cannot truncate memprofile datafile");
    }
#endif

    while (1)
    {
        int index;
        int argi = getopt_long(argc, argv, "ha:A:b:FDi:No:p:P:r:R:s:S:t:T:z:", longopts, &index);

        if (argi < 0) break;

        switch (argi)
        {
        case 'h':
            print_usage(argv[0]);
            return 0;

        case 'a':
            if (strcmp(optarg,"list") == 0)
            {
                getContestSingleton()->list_contentants();
                return 0;
            }
            gopt_algorithm_substr.push_back(optarg);
            std::cout << "Option -a: selecting algorithms containing " << optarg << std::endl;
            break;

        case 'A':
            if (strcmp(optarg,"list") == 0)
            {
                getContestSingleton()->list_contentants();
                return 0;
            }
            if (!getContestSingleton()->exist_contestant(optarg))
            {
                std::cout << "Option -A: unknown algorithm " << optarg << std::endl;
                return 0;
            }
            gopt_algorithm_exact.push_back(optarg);
            std::cout << "Option -A: selecting algorithm " << optarg << std::endl;
            break;

        case 'b':
            g_block_size = atoi(optarg);
            std::cout << "Option -b: setting block size to " << optarg << std::endl;
            break;

        case 'D':
            gopt_forkrun = gopt_forkdataload = true;
            std::cout << "Option -D: forking before each algorithm run and loading data after fork." << std::endl;
            break;

        case 'F':
            gopt_forkrun = true;
            std::cout << "Option -F: forking before each algorithm run, but load data before fork." << std::endl;
            break;

        case 'i':
            gopt_input_write = optarg;
            std::cout << "Option -i: will write input items to \"" << gopt_input_write << "\"." << std::endl;
            break;

        case 'N':
            gopt_no_check = true;
            std::cout << "Option --no-check: skipping checking of sorted order and distinguishing prefix calculation." << std::endl;
            break;

        case 'o':
            gopt_output_write = optarg;
            std::cout << "Option -o: will write output strings to \"" << gopt_output_write << "\"" << std::endl;
            break;

        case 'P':
            gopt_papi_events = optarg;
            std::cout << "Option --papi: monitoring events " << gopt_papi_events << "." << std::endl;
            break;

        case 'r':
            if (!input::parse_filesize(optarg, gopt_repeats_inner)) {
                std::cout << "Option -s: invalid repeats-inner parameter: " << optarg << std::endl;
                exit(EXIT_FAILURE);
            }
            std::cout << "Option -r: repeat inner loop " << gopt_repeats_inner << " times." << std::endl;
            break;

        case 'R':
            gopt_repeats_outer = atoi(optarg);
            std::cout << "Option -R: repeat algorithms " << gopt_repeats_outer << " times." << std::endl;
            break;

        case 's':
            if (!input::parse_filesize(optarg, gopt_inputsize_minlimit)) {
                std::cout << "Option -s: invalid size parameter: " << optarg << std::endl;
                exit(EXIT_FAILURE);
            }
            std::cout << "Option -s: limiting input size to " << gopt_inputsize_minlimit << std::endl;
            break;

        case 'S':
            if (!input::parse_filesize(optarg, gopt_inputsize_maxlimit)) {
                std::cout << "Option -S: invalid maxsize parameter: " << optarg << std::endl;
                exit(EXIT_FAILURE);
            }
            std::cout << "Option -S: limiting maximum input size to " << gopt_inputsize_maxlimit << std::endl;
            break;

        case 't':
            gopt_typesize = atoi(optarg);
            std::cout << "Option -t: running with types of size " << gopt_typesize << "." << std::endl;
            break;

        case 'T':
            gopt_timeout = atoi(optarg);
            std::cout << "Option -T: aborting algorithms after " << gopt_timeout << " seconds timeout." << std::endl;
            break;

        case 'z':
			gopt_seeds.push_back(atoi(optarg));
			std::cout << "Option -z: Using random seed " << optarg << " for inputs." << std::endl;
			break;

        case 'p':
            g_pivot_switch = atoi(optarg);
            std::cout << "Option -p: Using " << optarg << " to switch pivot strategies." << std::endl;
            break;

        default:
            std::cout << "Invalid parameter: " << argi << std::endl;
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind == argc) { // no input data parameter given
        print_usage(argv[0]);
        return 0;
    }

    if (gopt_inputsize_maxlimit < gopt_inputsize_minlimit)
        gopt_inputsize_maxlimit = gopt_inputsize_minlimit;

    if (gopt_seeds.size() == 0)
        gopt_seeds.push_back(time(NULL));


    for (; optind < argc; ++optind)
    {
        // iterate over input size range
        for (g_input_size = gopt_inputsize_minlimit;
             g_input_size <= gopt_inputsize_maxlimit;
             g_input_size *= 2, gopt_repeats_divider *= 2)
        {
            // iterate over seeds
            for (size_t si = 0; si < gopt_seeds.size(); ++si)
            {
                g_seed = gopt_seeds[si];
                input::gen.seed(g_seed);

                getContestSingleton()->run_contest(argv[optind]);
            }
        }
    }
    return 0;
}
