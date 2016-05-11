#include "parser.h"

#include <iostream>

#include "scanner.h"

Parser::Parser(Scanner & s)
    : scanner_(s)
{
}

void Parser::get_token()
{
    token_ = scanner_.getToken();
}

void Parser::parse()
{
    get_token();
    root_ = stmt_sequence();
    if(token_ != ENDFILE)
        syntax_error("unexpected token");
}

TreeNode * Parser::stmt_sequence()
{
    TreeNode *sq = alloc_stmtseq();
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
        return if_stmt();
        break;
    case REPEAT:
        return repeat_stmt();
        break;
    case ID:
        return assign_stmt();
        break;
    case READ:
        return read_stmt();
        break;
    case WRITE:
        return write_stmt();
        break;
    default:
        get_token();
        syntax_error("unexpected token");
        return 0;
        break;
    }
}

TreeNode * Parser::if_stmt()
{
    TreeNode * t = alloc_stmt(IfK);
    match(IF);
    t->child = expr();
    match(THEN);
    t->child->sibling = stmt_sequence();
    if(token_ == ELSE)
    {
        match(ELSE);
        t->child->sibling->sibling = stmt_sequence();
    }
    match(END);
    return t;
}

TreeNode * Parser::repeat_stmt()
{
    TreeNode * t = alloc_stmt(RepeatK);
    match(REPEAT);
    t->child = stmt_sequence();
    match(UNTIL);
    t->child->sibling = expr();
    return t;
}

TreeNode * Parser::assign_stmt()
{
    TreeNode * t = alloc_stmt(AssignK);
    if(token_ == ID)
    {
        t->name = token_id();
    }
    match(ID);
    match(ASSIGN);
    t->child = expr();
    return t;
}

TreeNode * Parser::read_stmt()
{
    TreeNode * t = alloc_stmt(ReadK);
    match(READ);
    if(token_ == ID)
    {
        t->name = token_id();
    }
    match(ID);
    return t;
}

TreeNode * Parser::write_stmt()
{
    TreeNode * t = alloc_stmt(WriteK);
    match(WRITE);
    t->child = expr();
    return t;
}

TreeNode * Parser::expr()
{
    TreeNode * t = simple_expr();
    switch (token_)
    {
    case EQ:
    case LT:
    {
        TreeNode * p = alloc_expr(OpK);
        p->child = t;
        p->op = token_;
        t = p;
        match(token_);
        t->child->sibling = simple_expr();
    }
        break;
    default:
        break;
    }

    return t;
}

TreeNode * Parser::simple_expr()
{
    TreeNode * t = term();
    while(true)
    {
        switch (token_)
        {
        case ADD:
        case SUB:
        {
            TreeNode * p = alloc_expr(OpK);
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
            TreeNode * p = alloc_expr(OpK);
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
        t = alloc_expr(ConstK);
        t->value = token_num();
        match(NUM);
        break;
    case ID:
        t = alloc_expr(IdK);
        t->name = token_id();
        match(ID);
        break;
    case LP:
        match(LP);
        t = expr();
        match(RP);
        break;
    default:
        syntax_error("unexpected token");
        get_token();
        break;
    }
    return t;
}

TreeNode * Parser::alloc_stmtseq()
{
    TreeNode * t = new TreeNode;
    t->kind = StmtSeqK;
    return t;
}

TreeNode * Parser::alloc_stmt(StmtKind k)
{
    TreeNode * t = new TreeNode;
    t->kind = StmtK;
    t->stmt_kind = k;
    t->line_no = token_line_no();
    return t;
}

TreeNode * Parser::alloc_expr(ExprKind k)
{
    TreeNode * t = new TreeNode;
    t->kind = ExprK;
    t->expr_kind = k;
    t->line_no = token_line_no();
    return t;
}

void Parser::match(TokenType token)
{
    if(token_ == token)
        get_token();
    else
        syntax_error("unexpected token");
}

void Parser::syntax_error(const char *err)
{
    std::cout << "line: " << scanner_.line() << " >>>>>>>>>> " << err << std::endl;
    throw;
}
