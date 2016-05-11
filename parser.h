#ifndef PARSER_H
#define PARSER_H

#include "treenode.h"
#include "scanner.h"

class Parser
{
public:
    Parser(Scanner & s);
    void parse();
    TreeNode * root() { return root_; }

private:
    TreeNode * stmt_sequence();
    TreeNode * statement();
    TreeNode * if_stmt();
    TreeNode * repeat_stmt();
    TreeNode * assign_stmt();
    TreeNode * read_stmt();
    TreeNode * write_stmt();

    TreeNode * expr();
    TreeNode * simple_expr();
    TreeNode * term();
    TreeNode * factor();

    TreeNode * alloc_stmtseq();
    TreeNode * alloc_stmt(StmtKind k);
    TreeNode * alloc_expr(ExprKind k);

    void get_token();
    const std::string & token_id() { return scanner_.id(); }
    int token_num() { return scanner_.num(); }
    size_t token_line_no() { return scanner_.line(); }


    void match(TokenType token);

    void syntax_error(const char * err);

    TreeNode * root_;
    Scanner & scanner_;
    TokenType token_;
};

#endif // PARSER_H
