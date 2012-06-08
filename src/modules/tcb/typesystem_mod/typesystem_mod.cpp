//
// Part of Metta OS. Check http://metta.exquance.com for latest version.
//
// Copyright 2007 - 2012, Stanislav Karchebnyy <berkus@exquance.com>
//
// Distributed under the Boost Software License, Version 1.0.
// (See file LICENSE_1_0.txt or a copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "type_system_v1_interface.h"
#include "type_system_f_v1_interface.h"
#include "type_system_f_v1_impl.h"
#include "type_system_factory_v1_interface.h"
#include "type_system_factory_v1_impl.h"
#include "operation_v1_interface.h"
#include "interface_v1_interface.h"
#include "interface_v1_impl.h"
#include "map_card64_address_factory_v1_interface.h"
#include "map_string_address_factory_v1_interface.h"
#include "heap_new.h"
#include "default_console.h"
#include "exceptions.h"

/**
 * @defgroup typecode Type code
 *
 * Type code is 64 bit entity consisting of 48-bit unique identifier for the interface type plus 16 bit entity for indexing subtypes inside the interface
 * (exceptions, aliases, structures, etc.)
 * 48 bit code is generated by meddler while parsing the interface file. It attempts to maintain some sort of stability for type codes for same interfaces.
 * 16 bit code is just a sequential enumeration of all types inside an interface.
 */

//=====================================================================================================================
// Type system internal data structures.
//=====================================================================================================================

struct type_system_f_v1::state_t
{
    type_system_f_v1::closure_t closure;
    map_card64_address_v1::closure_t* interfaces_by_typecode;
    map_string_address_v1::closure_t* interfaces_by_name;
};

/*
 * Parameter type : used in operations
 */
struct param_t 
{
    operation_v1::parameter rep;
    cstring_t        name;
};

/*
 * Field type : used in Records, Choices, Enumerations and Exceptions
 */
struct field_t {
    types::any   val;
    cstring_t    name;
};

/*
 * Enumeration and record type state
 */
struct EnumRecState_t {
    size_t   num;
    field_t *elems;     /* val = enum. value or record type code */
};


/*
 * Choice type state
 */
struct ChoiceState_t {
    EnumRecState_t ctx;
    types::code    disc;      /* discriminator    */
};

struct ExcRef_t {
    types::code    intf;
    uint32_t       num;
};

/* Exceptions */
struct Exc_t {
    EnumRecState_t params;      /* Parameters - must be first   */
    exception_v1::closure_t cl;            /* Closure for exception        */
    interface_v1::state_t    *intf;           /* Defining interface           */
    cstring_t    name;       /* Name of exception        */
};

/* Operations */
struct operation_t
{
    cstring_t           name;    /* Name of Operation        */
    operation_v1::kind  kind;    /* Type of Operation        */
    param_t*            params;  /* Array of parameters      */
    uint32_t       num_args;    /* How many there are       */
    uint32_t       num_res; /* How many there are       */
    uint32_t       proc_num;    /* Procedure number     */
    ExcRef_t      *exns;        /* Array of exceptions          */       
    uint32_t       num_excepts; /* Number of exceptions         */
    operation_v1::closure_t*  cl;      /* Closure for operation    */
};


/* Type within an interface */
struct type_representation_t
{
    types::any                any;        /* Class-specific stuff     */
    types::any                code;       /* Type code of type, as an any */
    types::name               name;       /* Name of type         */
    interface_v1::state_t    *interface;  /* Pointer to defining interface*/
    memory_v1::size           size;       /* Size of an instance      */
};

/* Interface itself (also a type, obviously) */
struct interface_v1::state_t
{
    type_representation_t rep;
    types::code   const *needs;   /* List of needed interfaces    */
    size_t   num_needs; /* How many of them are there   */
    type_representation_t   * const *types; /* List of defined types    */
    size_t   num_types; /* How many of them are there   */
    bool   local;     /* Is this interface local? */
    types::code    extends;   /* Supertype, if any        */
    operation_t  * const *methods; /* Table of methods      */
    size_t   num_methods;   /* No. of methods we define     */
    Exc_t    * const *exns; /* Table of exceptions      */
    size_t   num_excepts;   /* No. of exceptions we define  */
};

/*
 * Macros for manipulating type codes
 */
#define TCODE_META_NAME     "meta_interface"
#define TCODE_MASK      ((types::code) 0xffffull)
#define TCODE_INTF_CODE(_tc)    ((_tc) & ~TCODE_MASK)
#define TCODE_IS_INTERFACE(_tc) (!((_tc) & TCODE_MASK))

#define TCODE_VALID_TYPE(_tc,_b) \
( ((_tc) & TCODE_MASK) <= (_b)->num_types)

#define TCODE_WHICH_TYPE(_tc,_b) \
    (((_b)->types)[ ((_tc) & TCODE_MASK) - 1 ])

#define TCODE_NONE      0xffffffffffffffffULL
#define MetaInterface__code 0

//=====================================================================================================================
// add, remove, dup and destroy methods from naming_context are all nulls.
//=====================================================================================================================

static void
shared_add(naming_context_v1::closure_t*, const char*, types::any)
{
    OS_RAISE((exception_support_v1::id)"naming_context_v1.denied", NULL);
    return;
}

static void
shared_remove(naming_context_v1::closure_t*, const char*)
{ 
    OS_RAISE((exception_support_v1::id)"naming_context_v1.denied", NULL);
    return; 
}

// dup is not yet defined, but it raises denied as well

static void
shared_destroy(naming_context_v1::closure_t*)
{ 
    return; 
}

//=====================================================================================================================
// Typesystem
//=====================================================================================================================

static naming_context_v1::names
type_system_v1_list(naming_context_v1::closure_t* self)
{
    return naming_context_v1::names();
}

static bool
type_system_v1_get(naming_context_v1::closure_t* self, const char* name, types::any* obj)
{
    return false;
}

static interface_v1::closure_t*
type_system_v1_info(type_system_v1::closure_t* self, type_system_v1::alias tc, types::any* rep)
{
    return 0;
}

static memory_v1::size
type_system_v1_size(type_system_v1::closure_t* self, type_system_v1::alias tc)
{
    return 0;
}

static types::name
type_system_v1_name(type_system_v1::closure_t* self, type_system_v1::alias tc)
{
    return types::name();
}

static bool
type_system_v1_is_type(type_system_v1::closure_t* self, type_system_v1::alias sub, type_system_v1::alias super)
{
    return false;
}

static types::val
type_system_v1_narrow(type_system_v1::closure_t* self, types::any a, type_system_v1::alias tc)
{
    return types::val();
}

static type_system_v1::alias
type_system_v1_unalias(type_system_v1::closure_t* self, type_system_v1::alias tc)
{
    return type_system_v1::alias();
}

static void
type_system_f_v1_register_interface(type_system_f_v1::closure_t* self, type_system_f_v1::interface_info intf)
{
    kconsole << "register_interface" << endl;
}

static type_system_f_v1::ops_t typesystem_ops = 
{
    type_system_v1_list,
    type_system_v1_get,
    shared_add,
    shared_remove,
    shared_destroy,
    type_system_v1_info,
    type_system_v1_size,
    type_system_v1_name,
    type_system_v1_is_type,
    type_system_v1_narrow,
    type_system_v1_unalias,
    type_system_f_v1_register_interface
};

//=====================================================================================================================
// Meta-interface
//=====================================================================================================================

static bool
interface_v1_extends(interface_v1::closure_t* self, interface_v1::closure_t** o)
{
    return false;
}

static bool
interface_v1_info(interface_v1::closure_t* self, interface_v1::needs* need_list, types::name* name, types::code* code)
{
    return false;
}

static interface_v1::ops_t meta_ops =
{
    type_system_v1_list,
    type_system_v1_get,
    shared_add,
    shared_remove,
    shared_destroy,
    interface_v1_extends,
    interface_v1_info
};

interface_v1::closure_t meta_interface_closure =
{
    &meta_ops,
    NULL
};

//=====================================================================================================================
// Definition of types in the 'mythical' interface meta_interface which defines all predefined Meddle types 
// and all interfaces in Metta (including meta_interface itself).
//=====================================================================================================================

extern interface_v1::state_t meta_interface;

#define PREDEFINED_TYPE_REP(name,strname,tag) \
static type_representation_t type_##name##__rep = { \
    {  type_system_v1::predefined_type_code, type_system_v1::predefined_##tag }, \
    {  types::code_type_code, name##__code }, \
    strname, \
    &meta_interface, \
    sizeof(name) \
}

PREDEFINED_TYPE_REP(uint8_t,"octet", Octet);
PREDEFINED_TYPE_REP(int8_t,"int8", Char);
PREDEFINED_TYPE_REP(uint16_t,"card16", ShortCardinal);
PREDEFINED_TYPE_REP(uint32_t,"card32", Cardinal);
PREDEFINED_TYPE_REP(uint64_t,"card64", LongCardinal);
PREDEFINED_TYPE_REP(int16_t,"int16", ShortInteger);
PREDEFINED_TYPE_REP(int32_t,"int32", Integer);
PREDEFINED_TYPE_REP(int64_t,"int64", LongInteger);
PREDEFINED_TYPE_REP(float,"float", Real);
PREDEFINED_TYPE_REP(double,"double", LongReal);
PREDEFINED_TYPE_REP(bool,"boolean", Boolean);
PREDEFINED_TYPE_REP(cstring_t,"string", String);
PREDEFINED_TYPE_REP(voidptr,"opaque", Opaque);

static type_representation_t* const meta_interface__types[] = {
    &type_uint8_t__rep,
    &type_uint16_t__rep,
    &type_uint32_t__rep,
    &type_uint64_t__rep,
    &type_int8_t__rep,
    &type_int16_t__rep,
    &type_int32_t__rep,
    &type_int64_t__rep,
    &type_float__rep,
    &type_double__rep,
    &type_bool__rep,
    &type_cstring_t__rep,
    &type_voidptr__rep,
    (type_representation_t*)0
};

static types::code const meta_interface__needs[] = {
    TCODE_NONE
};

interface_v1::state_t meta_interface =
{
    {
    { type_system_v1::iref_type_code, (types::val)&meta_interface_closure },    // any & cl.ptr
    { types::code_type_code, MetaInterface__code }, // Type Code    
    TCODE_META_NAME,                            // Textual name
    &meta_interface,                            // Scope
    sizeof(interface_v1::closure_t*)            // Size
    },
    meta_interface__needs, // Needs list
    0,                     // No. of needs
    meta_interface__types, // Types list
    14,                    // No. of types
    true,                  // Local flag
    interface_v1::type_code,    // Supertype
    NULL, // Methods
    0,
    NULL, // Exceptions
    0
};

//=====================================================================================================================
// The Factory
//=====================================================================================================================

static type_system_f_v1::closure_t* 
create(type_system_factory_v1::closure_t* self, heap_v1::closure_t* h, 
       map_card64_address_factory_v1::closure_t* cardmap, map_string_address_factory_v1::closure_t* stringmap)
{
    type_system_f_v1::state_t* state = new(h) type_system_f_v1::state_t;
    closure_init(&state->closure, &typesystem_ops, state);

    state->interfaces_by_typecode = cardmap->create(h);
    state->interfaces_by_name = stringmap->create(h);

    state->closure.register_interface(reinterpret_cast<type_system_f_v1::interface_info>(&meta_interface));
    /* The meta-interface closure is of type "interface_v1" but has different methods 
     * since it's state record is type_system state rather than Intf_st. 
     */
    meta_interface_closure.d_state = reinterpret_cast<interface_v1::state_t*>(state);

    return &state->closure;
}

static type_system_factory_v1::ops_t methods = 
{
    create
};

static type_system_factory_v1::closure_t clos =
{
    &methods,
    NULL
};

EXPORT_CLOSURE_TO_ROOTDOM(type_system_factory, v1, clos);
