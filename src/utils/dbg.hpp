/// dbg.hpp -- debug utilities

#pragma once

#include <cassert>

#include <mutex>
static std::mutex cout_mtx;

#define DBGOUTPUT  // Uncomment to enable debug output

#ifdef DEBUG

#ifdef DBGOUTPUT
// Debug output
#include <string.h>
#include <iostream>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DBGOUT(p)                                                                                 \
    {                                                                                             \
        std::lock_guard<std::mutex> lock(cout_mtx);                                               \
        std::cerr << "[" << __FILENAME__ << ":" << __LINE__ << " <" << __FUNCTION__ << ">] " << p \
                  << "\n";                                                                        \
    }

#else

#define DBGOUT(p) ;

#endif

#define DBG(b)       b
#define DBGASSERT(p) assert(p)

#else

#define DBGOUT(p)    ;
#define DBGASSERT(p) ;

#define DBG(b) ;

#endif
