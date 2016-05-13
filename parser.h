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
    TreeNode * stmtSequence();
    TreeNode * statement();
    TreeNode * ifStmt();
    TreeNode * repeatStmt();
    TreeNode * assignStmt();
    TreeNode * readStmt();
    TreeNode * writeStmt();

    TreeNode * expr();
    TreeNode * simpleExpr();
    TreeNode * term();
    TreeNode * factor();

    TreeNode * allocStmtSeq();
    TreeNode * allocStmt(StmtKind k);
    TreeNode * allocExpr(ExprKind k);

    void getToken();
    const std::string & tokenId() { return scanner_.id(); }
    int tokenNum() { return scanner_.num(); }
    size_t tokenLineNo() { return scanner_.line(); }


    void match(TokenType token);

    void syntaxError(const char * err);

    TreeNode * root_;
    Scanner & scanner_;
    TokenType token_;
};

#endif // PARSER_H
