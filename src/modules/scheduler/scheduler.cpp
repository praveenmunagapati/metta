//
// Part of Metta OS. Check http://metta.exquance.com for latest version.
//
// Copyright 2007 - 2010, Stanislav Karchebnyy <berkus@exquance.com>
//
// Distributed under the Boost Software License, Version 1.0.
// (See file LICENSE_1_0.txt or a copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "scheduler.h"
#include "c++ctors.h"
#include "debugger.h"

extern "C" void init()
{
    run_global_ctors();
    bochs_console_print_str("scheduler: init\n");
}

namespace scheduler
{

scheduler_t::scheduler_t()
{
}

} // namespace scheduler

// kate: indent-width 4; replace-tabs on;
// vim: set et sw=4 ts=4 sts=4 cino=(4 :
