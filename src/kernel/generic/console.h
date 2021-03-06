//
// Part of Metta OS. Check https://atta-metta.net for latest version.
//
// Copyright 2007 - 2017, Stanislav Karchebnyy <berkus@atta-metta.net>
//
// Distributed under the Boost Software License, Version 1.0.
// (See file LICENSE_1_0.txt or a copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "types.h"
#include "panic.h"
#include "cstring.h"

#ifndef endl
#define endl console_t::eol
#endif

enum Color {
    BLACK = 0,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    DARKGRAY=8,
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE,
    // Special colors
    WARNING = YELLOW,
    ERROR = LIGHTRED
};

/**
 * Abstract base class for console output.
 */
class console_t
{
public:
    static const char eol;

    virtual void set_color(Color col) = 0;
    virtual void set_background(Color col) = 0;
    virtual void set_attr(Color fore, Color back) = 0;// e.g. GREEN on BLACK

    virtual void clear() = 0;
    virtual void locate(int row, int col) = 0;
    virtual void scroll_up() = 0;
    virtual void newline() = 0;

    virtual void print_int(int n) = 0;
    virtual void print_char(char ch) = 0;
    virtual void print_unprintable(char ch) = 0;
    virtual void print_byte(unsigned char n) = 0;
    virtual void print_hex(uint32_t n) = 0;
    virtual void print_hex2(uint16_t n) = 0;
    virtual void print_hex8(uint64_t n) = 0;
    virtual void print_str(const char *s) = 0;

    // Wrappers for template version
    inline void print(int32_t n) { print_int(n); }
    inline void print(char ch) { print_char(ch); }
    inline void print(unsigned char n) { print_byte(n); }
    inline void print(uint32_t n) { print_hex(n); }
    inline void print(uint16_t n) { print_hex2(n); }
    inline void print(void *p) { print((uint32_t)p); }
    inline void print(uint64_t n) { print_hex8(n); }
    inline void print(const char* str) { print_str(str); }
    template<typename T, typename... Args>
    void print(const char* str, T value, Args... args);

    virtual void wait_ack() = 0;

    virtual void debug_log(const char *str, ...) = 0;

protected:
    console_t();
    virtual ~console_t();
};

// Define stream io on console.
inline console_t& operator << (console_t& con, Color data)
{
    con.set_color(data);
    return con;
}

inline console_t& operator << (console_t& con, const char* data)
{
    con.print_str(data);
    return con;
}

// inline console_t& operator << (console_t& con, int data)
// {
//     con.print_int(data);
//     return con;
// }

inline console_t& operator << (console_t& con, int32_t data)
{
    con.print_int(data);
    return con;
}

// inline console_t& operator << (console_t& con, unsigned int data)
// {
//     con.print_hex(data);
//     return con;
// }

inline console_t& operator << (console_t& con, uint32_t data)
{
    con.print_hex(data);
    return con;
}

// inline console_t& operator << (console_t& con, size_t data)
// {
//     con.print_hex(data);
//     return con;
// }

inline console_t& operator << (console_t& con, uint16_t data)
{
    con.print_hex2(data);
    return con;
}

inline console_t& operator << (console_t& con, uint64_t data)
{
    con.print_hex8(data);
    return con;
}

inline console_t& operator << (console_t& con, char data)
{
    con.print_char(data);
    return con;
}

inline console_t& operator << (console_t& con, unsigned char data)
{
    con.print_byte(data);
    return con;
}

inline console_t& operator << (console_t& con, const void* data)
{
    con.print_hex((uint32_t)data);
    return con;
}

inline console_t& operator << (console_t& con, const cstring_t& data)
{
    con.print_str(data.c_str());
    return con;
}

template<typename T, typename... Args>
void console_t::print(const char* str, T value, Args... args)
{
    while (*str)
    {
        if (*str == '%' && *(++str) != '%')
        {
            print(value);
            print(*str ? ++str : str, args...); // call even when *s == 0 to detect extra arguments
            return;
        }
        print(*str++);
    }
    PANIC("console: extra arguments provided to print");
}
