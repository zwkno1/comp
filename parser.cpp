#include "parser.h"

#include <iostream>
#include <sstream>

#include "lexer.h"

Parser::Parser(Lexer & s)
    : scanner_(s)
{
}

void Parser::getToken()
{
    token_ = scanner_.getToken();
}

void Parser::parse()
{
    getToken();
    root_ = stmtSequence();
    if(token_ != ENDFILE)
        syntaxError(std::string("expect token: ") + getTokenStr(ENDFILE));
}

TreeNode * Parser::stmtSequence()
{
    TreeNode *sq = allocStmtSeq();
    TreeNode *t = 0;
    while(true)
    {
        switch (token_)
        {
        case ELSE :
        case UNTIL :
        case END :
        case ENDFILE :
            return sq;
        default :
        {
            TreeNode * n = statement();
            if(t)
            {
                t->sibling = n;
            }
            else
            {
                sq->child = n;
            }
            t = n;
            match(SEMI);
        }
            break;
        }
    }
}

TreeNode * Parser::statement()
{
    switch (token_)
    {
    case IF:
        return ifStmt();
    case REPEAT:
        return repeatStmt();
    case ID:
        return assignStmt();
    case READ:
        return readStmt();
    case WRITE:
        return writeStmt();
    default:
        syntaxError(std::string("unexpected token: ") + getTokenStr(token_));
        return 0;
    }
}

TreeNode * Parser::ifStmt()
{
    TreeNode * t = allocStmt(IfK);
    match(IF);
    t->child = expr();
    match(THEN);
    t->child->sibling = stmtSequence();
    if(token_ == ELSE)
    {
        match(ELSE);
        t->child->sibling->sibling = stmtSequence();
    }
    match(END);
    return t;
}

TreeNode * Parser::repeatStmt()
{
    TreeNode * t = allocStmt(RepeatK);
    match(REPEAT);
    t->child = stmtSequence();
    match(UNTIL);
    t->child->sibling = expr();
    return t;
}

TreeNode * Parser::assignStmt()
{
    TreeNode * t = allocStmt(AssignK);
    if(token_ == ID)
    {
        t->name = tokenId();
    }
    match(ID);
    match(ASSIGN);
    t->child = expr();
    return t;
}

TreeNode * Parser::readStmt()
{
    TreeNode * t = allocStmt(ReadK);
    match(READ);
    if(token_ == ID)
    {
        t->name = tokenId();
    }
    match(ID);
    return t;
}

TreeNode * Parser::writeStmt()
{
    TreeNode * t = allocStmt(WriteK);
    match(WRITE);
    t->child = expr();
    return t;
}

TreeNode * Parser::expr()
{
    TreeNode * t = simpleExpr();
    switch (token_)
    {
    case EQ:
    case LT:
    {
        TreeNode * p = allocExpr(OpK);
        p->child = t;
        p->op = token_;
        t = p;
        match(token_);
        t->child->sibling = simpleExpr();
    }
        break;
    default:
        break;
    }

    return t;
}

TreeNode * Parser::simpleExpr()
{
    TreeNode * t = term();
    while(true)
    {
        switch (token_)
        {
        case ADD:
        case SUB:
        {
            TreeNode * p = allocExpr(OpK);
            p->op =  token_;
            p->child = t;
            t = p;
            match(token_);
            p->child->sibling = term();
            break;
        }
        default:
            return t;
            break;
        }
    }
}

TreeNode * Parser::term()
{
    TreeNode * t = factor();
    while(true)
    {
        switch (token_)
        {
        case MUL:
        case DIV:
        {
            TreeNode * p = allocExpr(OpK);
            p->op =  token_;
            p->child = t;
            t = p;
            match(token_);
            p->child->sibling = factor();
            break;
        }
        default:
            return t;
            break;
        }
    }
}

TreeNode * Parser::factor()
{
    TreeNode * t = 0;
    switch (token_)
    {
    case NUM:
        t = allocExpr(ConstK);
        t->value = tokenNum();
        match(NUM);
        break;
    case ID:
        t = allocExpr(IdK);
        t->name = tokenId();
        match(ID);
        break;
    case LP:
        match(LP);
        t = expr();
        match(RP);
        break;
    default:
        syntaxError(std::string("unexpected token: ") + getTokenStr(token_));
        getToken();
        break;
    }
    return t;
}

TreeNode * Parser::allocStmtSeq()
{
    TreeNode * t = new TreeNode;
    t->kind = StmtSeqK;
    return t;
}

TreeNode * Parser::allocStmt(StmtKind k)
{
    TreeNode * t = new TreeNode;
    t->kind = StmtK;
    t->stmt_kind = k;
    t->line_no = tokenLineNo();
    return t;
}

TreeNode * Parser::allocExpr(ExprKind k)
{
    TreeNode * t = new TreeNode;
    t->kind = ExprK;
    t->expr_kind = k;
    t->line_no = tokenLineNo();
    return t;
}

void Parser::match(TokenType token)
{
    if(token_ == token)
        getToken();
    else
        syntaxError(std::string("expect token: ") + getTokenStr(token) + ", current token: " + getTokenStr(token_));
}

void Parser::syntaxError(const std::string & err)
{
    std::stringstream ss;
    ss << "line: " << scanner_.line() << " >>>>>>>>>> " << err;
    throw  ss.str();
}
