//
// Part of Metta OS. Check https://atta-metta.net for latest version.
//
// Copyright 2007 - 2017, Stanislav Karchebnyy <berkus@atta-metta.net>
//
// Distributed under the Boost Software License, Version 1.0.
// (See file LICENSE_1_0.txt or a copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "multiboot.h"

multiboot_t mb;
address_t multiboot_info;
address_t multiboot_flags;

multiboot_t* multiboot_t::prepare()
{
    if (multiboot_flags == 0x2BADB002)
    {
        mb.set_header(reinterpret_cast<multiboot_t::header_t*>(multiboot_info));
        return &mb;
    }
    return 0;
}

