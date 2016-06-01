/******************************************************************************
 * src/tools/papi.h
 *
 * Wrapper class to PAPI library for access to performance counters.
 * http://icl.cs.utk.edu/papi/
 *
 ******************************************************************************
 * Copyright (C) 2014 Timo Bingmann <tb@panthema.net>
 * Copyright (C) 2014 Andreas Beckmann <a.beckmann@fz-juelich.de>
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

#ifndef TOOLS_PAPI_H
#define TOOLS_PAPI_H

#ifdef WITH_PAPI

#include <stdexcept>
#include "debug.h"

extern "C" {
#include <papi.h>
}

class PApiWrapper
{
protected:

    static const bool debug = true;

    static const int max_events = 32;

    bool m_available;

    int EventSet;
    int start_result;
    int stop_result;

    int num_events;
    int event[max_events];
    long long values[max_events];

public:

    //! try to initialize the library
    PApiWrapper()
        : m_available(false),
          EventSet(PAPI_NULL),
          start_result(-1),
          stop_result(-1)
    {
        int retval = -1;

        // initialize papi library
        if ( (retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT ) {
            DBG(debug, "PAPI_library_init(): " << PAPI_strerror(retval));
            return;
        }

        // get the number of counters available
        int num_hwcntrs = 0;

        if ( (num_hwcntrs = PAPI_num_counters()) <= PAPI_OK ) {
            DBG(debug, "PAPI_num_counters(): zero counters");
            return;
        }

        DBG(debug, "This system has " << num_hwcntrs << " available performance counters.");

        // create EventSet
        if ( (retval = PAPI_create_eventset(&EventSet)) != PAPI_OK ) {
            DBG(debug, "PAPI_create_eventset(): " << PAPI_strerror(retval));
            return;
        }

        m_available = true;
    }

    //! true if PAPI counters are available
    bool available() const
    {
        return m_available;
    }

    //! add an event to the event set
    bool add_event(int event_id)
    {
        int retval = PAPI_add_event(EventSet, event_id);

        if ( retval != PAPI_OK ) {
            DBG(1, "PAPI_create_eventset(): " << PAPI_strerror(retval));
            return false;
        }

        num_events++;
        return true;
    }

    //! add an event by name to the event set
    bool add_event(const std::string& event_name)
    {
        int event_id;
        int retval = PAPI_event_name_to_code((char*)event_name.c_str(), &event_id);

        if ( retval != PAPI_OK ) {
            DBG(1, "PAPI_event_name_to_code(" << event_name << "): " << PAPI_strerror(retval));
            return false;
        }

        return add_event(event_id);
    }

    //! add an event list by name to the event set
    bool add_event_list(const std::string& event_list)
    {
        std::string::size_type pos = 0;

        while ( pos < event_list.size() )
        {
            size_t endpos = event_list.find_first_of(",:;", pos);

            std::string event_name = event_list.substr(pos, endpos - pos);

            // prepend PAPI_ if not there
            if (event_name.substr(0, 5) != "PAPI_") {
                event_name = "PAPI_" + event_name;
            }

            add_event(event_name);

            if (endpos == std::string::npos) break;
            pos = endpos+1;
        }

        return true;
    }

    //! Start event counting
    void start()
    {
        if (!m_available) return;

        //! this is required to translate portable event names to hardware!
        PAPI_list_events(EventSet, event, &num_events);

        //! start counting
        start_result = PAPI_start(EventSet);
    }

    //! Stop event counting and retrieve results
    void stop()
    {
        if (!m_available) return;

        stop_result = PAPI_stop(EventSet, values);
    }

    //! Print out status report of all event counters
    void report();

    //! Return numeric values of an event counter
    size_t get_num_counter() const
    {
        return num_events;
    }

    //! Return numeric values of an event counter
    long long get_counter_result(int num) const
    {
        assert(num < max_events);
        return values[num];
    }

    //! Return PAPI code of event counter
    std::string get_counter_name(int num) const
    {
        PAPI_event_info_t info;

        if (PAPI_get_event_info(event[num], &info) == PAPI_OK)
            return info.symbol;
        else
            return "UNKNOWN";
    }
};

void PApiWrapper::report()
{
    if (!m_available) return;

    if (start_result != PAPI_OK)
        OUT(1, "PAPI_start failed: " << PAPI_strerror(start_result));

    if (stop_result != PAPI_OK)
        OUT(1, "PAPI_stop failed: " << PAPI_strerror(stop_result));

    for (int i = 0; i < num_events; ++i)
    {
        PAPI_event_info_t info;

        if (PAPI_get_event_info(event[i], &info) == PAPI_OK)
        {
            OUT(1, "COUNTER(" << info.symbol << ") = " << values[i]);
        }
        else
        {
            OUT(1, "PAPI_get_event_info failed: " << PAPI_strerror(stop_result));
        }
    }
}

#else

struct PApiWrapper
{
    PApiWrapper() { }
    bool available() const { return false; }
    void add_event(int) { }
    void start() { }
    void stop() { }
    void report() { }
    size_t get_num_counter() const { return 0; }
    long long get_counter_result(int) const { return 0; }
    std::string get_counter_name(int) const { return "UNKNOWN"; }
};

#endif

#endif // TOOLS_PAPI_H
