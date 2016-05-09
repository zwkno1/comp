#ifndef PARSER_H
#define PARSER_H

#include "treenode.h"

class Scanner;
class Parser
{
public:
    Parser(Scanner & s);
    void parse();

private:
    TreeNode * stmt_sequence();
    TreeNode * statement();
    void match(TokenType token);
    void syntax_error(const char * err);

    TreeNode * root_;
    Scanner & scanner_;
    TokenType token_;
};

#endif // PARSER_H
