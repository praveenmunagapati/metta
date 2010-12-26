#pragma once

#include "lexer.h"
#include "ast.h"
#include "symbol_table.h"
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>

class parser_t
{
    bool is_local, is_final, is_idempotent; // TODO: move to private class parser_state_t
    lexer_t lex;
    symbol_table_t symbols;
    AST::node_t* parse_tree; friend class Meddler;
    llvm::SourceMgr& source_mgr;

    void populate_symbol_table();
    bool parse_top_level_entities();
    bool parse_interface();
    bool parse_interface_body();
    bool parse_exception();
    bool parse_method();
    bool parse_method_returns(AST::method_t& m);
    bool parse_method_raises(AST::method_t& m);
    bool parse_var_decl(AST::var_decl_t& to_get);
    bool parse_field_list(AST::node_t* parent);
    bool parse_field(AST::node_t* parent);
    bool parse_argument_list(std::vector<AST::parameter_t*>& args, AST::parameter_t::direction_e default_dir);
    bool parse_argument(std::vector<AST::parameter_t*>& args, AST::parameter_t::direction_e default_dir);
    bool parse_id_list(std::vector<std::string>& ids, token::kind delim);

    bool parse_enum_type_alias();
    bool parse_array_type_alias();
    bool parse_range_type_alias();
    bool parse_sequence_type_alias();
    bool parse_set_type_alias();
    bool parse_record_type_alias();
    bool parse_type_alias();

    void reportError(std::string msg);

public:
    parser_t(llvm::SourceMgr& sm);
    void init(const llvm::MemoryBuffer *F);
    bool run();
};
