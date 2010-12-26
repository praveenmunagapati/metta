#pragma once

#include <vector>
#include <string>

namespace AST
{

class var_decl_t;
class exception_t;
class alias_t;
class method_t;

class node_t
{
public:
    virtual void emit_impl_h(std::ostringstream& s) = 0;
    virtual void emit_interface_h(std::ostringstream& s) = 0;
    virtual void emit_interface_cpp(std::ostringstream& s) = 0;

    virtual void dump(std::string indent_prefix) = 0;
    virtual bool add_field(var_decl_t*) { return false; }
    virtual bool add_exception(exception_t*) { return false; }
    virtual bool add_type(alias_t*) { return false; }
    virtual bool add_method(method_t*) { return false; }
};

// Type, variable or parameter declaration as "type-name" pair.
// subtree of alias defines concrete variants: set_alias, array_alias, record_alias
class alias_t : public node_t
{
public:
    alias_t() : node_t(), type(), name() {}
    alias_t(std::string nm) : node_t(), type(), name(nm) {}
    alias_t(std::string tp, std::string nm) : node_t(), type(tp), name(nm) {}
    virtual void dump(std::string indent_prefix);

    std::string type; // use known types! check LLVM's Type/TypeBuilder
    std::string name;
};

// Variable or parameter declaration as "type-name" pair.
class var_decl_t : public alias_t
{
public:
    var_decl_t() : alias_t(), reference(false) {}
    void set_reference() { reference = true; }
    virtual void dump(std::string indent_prefix);

    virtual void emit_impl_h(std::ostringstream& s);
    virtual void emit_interface_h(std::ostringstream& s);
    virtual void emit_interface_cpp(std::ostringstream& s);

    bool reference;
};

class type_alias_t : public alias_t
{
public:
    type_alias_t() : alias_t() {}

    virtual void emit_impl_h(std::ostringstream& s);
    virtual void emit_interface_h(std::ostringstream& s);
    virtual void emit_interface_cpp(std::ostringstream& s);
};

class sequence_alias_t : public alias_t
{
public:
    // type - base type
    // name - sequence type name
    sequence_alias_t(std::string type, std::string base_type) : alias_t(base_type, type) {}
    virtual void dump(std::string indent_prefix);

    virtual void emit_impl_h(std::ostringstream& s);
    virtual void emit_interface_h(std::ostringstream& s);
    virtual void emit_interface_cpp(std::ostringstream& s);
};

class array_alias_t : public alias_t
{
public:
    // type - base type
    // name - sequence type name
    array_alias_t(std::string type, std::string base_type, int c) : alias_t(base_type, type), count(c) {}
    virtual void dump(std::string indent_prefix);

    virtual void emit_impl_h(std::ostringstream& s);
    virtual void emit_interface_h(std::ostringstream& s);
    virtual void emit_interface_cpp(std::ostringstream& s);

    int count;
};

class set_alias_t : public alias_t
{
public:
    // type - base type
    // name - set type name
    set_alias_t(std::string type, std::string base_type) : alias_t(base_type, type) {}
    virtual void dump(std::string indent_prefix);

    virtual void emit_impl_h(std::ostringstream& s);
    virtual void emit_interface_h(std::ostringstream& s);
    virtual void emit_interface_cpp(std::ostringstream& s);
};

class record_alias_t : public alias_t
{
public:
    std::vector<var_decl_t*> fields;

    record_alias_t(std::string nm) : alias_t() { name = nm; }
    virtual bool add_field(var_decl_t* field);
    virtual void dump(std::string indent_prefix);

    virtual void emit_impl_h(std::ostringstream& s);
    virtual void emit_interface_h(std::ostringstream& s);
    virtual void emit_interface_cpp(std::ostringstream& s);
};

class enum_alias_t : public alias_t
{
public:
    std::vector<std::string> fields;

    enum_alias_t() : alias_t() { }
    virtual bool add_field(var_decl_t* field);
    virtual void dump(std::string indent_prefix);

    virtual void emit_impl_h(std::ostringstream& s);
    virtual void emit_interface_h(std::ostringstream& s);
    virtual void emit_interface_cpp(std::ostringstream& s);
};

class range_alias_t : public alias_t
{
public:
    std::string start, end;

    range_alias_t(std::string nm, std::string s, std::string e) : alias_t(nm), start(s), end(e) { }
    virtual void dump(std::string indent_prefix);

    virtual void emit_impl_h(std::ostringstream& s);
    virtual void emit_interface_h(std::ostringstream& s);
    virtual void emit_interface_cpp(std::ostringstream& s);
};

// Represents both method arguments and returns.
class parameter_t : public var_decl_t
{
public:
    enum direction_e { in, out, inout } direction;
    virtual void dump(std::string indent_prefix);
};

class exception_t : public node_t
{
public:
    exception_t(std::string nm) : name(nm) {}
    virtual bool add_field(var_decl_t* field);
    virtual void dump(std::string indent_prefix);

    virtual void emit_impl_h(std::ostringstream& s);
    virtual void emit_interface_h(std::ostringstream& s);
    virtual void emit_interface_cpp(std::ostringstream& s);

    std::string name;
    std::vector<var_decl_t*> fields;
};

class method_t : public node_t
{
public:
    method_t() : node_t(), idempotent(false), never_returns(false) {}

    virtual void dump(std::string indent_prefix);
    virtual bool add_parameter(parameter_t*);
    virtual bool add_return(parameter_t*);
    virtual bool add_exception(exception_t*);

    virtual void emit_impl_h(std::ostringstream& s);
    virtual void emit_interface_h(std::ostringstream& s);
    virtual void emit_interface_cpp(std::ostringstream& s);

    std::string name;
    std::vector<parameter_t*> params;
    std::vector<parameter_t*> returns;
    std::vector<exception_t*> raises;
    std::vector<std::string>  raises_ids;
    bool idempotent;
    bool never_returns; // oneway
};

class interface_t : public node_t
{
public:
    interface_t(std::string nm, bool is_local, bool is_final) : local(is_local), final(is_final), name(nm) {}
    virtual bool add_method(method_t*);
    virtual bool add_exception(exception_t*);
    virtual bool add_type(alias_t*);
    virtual void dump(std::string indent_prefix);
    void set_parent(std::string p) { base = p; }

    virtual void emit_impl_h(std::ostringstream& s);
    virtual void emit_interface_h(std::ostringstream& s);
    virtual void emit_interface_cpp(std::ostringstream& s);

    bool local;
    bool final;
    std::string name;
    std::string base;
    std::vector<interface_t*> imports;//implicitly derived from 'types' contents.
    std::vector<alias_t*>     types;
    std::vector<exception_t*> exceptions;
    std::vector<method_t*>    methods;
};

}
