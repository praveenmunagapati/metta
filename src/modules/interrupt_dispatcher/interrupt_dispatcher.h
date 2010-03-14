//
// Part of Metta OS. Check http://metta.exquance.com for latest version.
//
// Copyright 2007 - 2010, Stanislav Karchebnyy <berkus@exquance.com>
//
// Distributed under the Boost Software License, Version 1.0.
// (See file LICENSE_1_0.txt or a copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

/*!
Interrupt dispatcher abstracts interrupt support of a particular platform
by turning interrupts into threads with help of the scheduler.
*/
class interrupt_dispatcher_t
{
public:
    interrupt_dispatcher_t();
private:
};

// kate: indent-width 4; replace-tabs on;
// vim: set et sw=4 ts=4 sts=4 cino=(4 :
