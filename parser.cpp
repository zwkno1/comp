#include "parser.h"

#include "scanner.h"

Parser::Parser(Scanner & s)
    : scanner_(s)
{
}

void Parser::parse()
{
    token_ = scanner_.getToken();
    root_ = stmt_sequence();
    if(token_ != ENDFILE)
        syntax_error("Code ends before file");
}

TreeNode * Parser::stmt_sequence()
{
    TreeNode * s = statement();
    while(true)
    {
        switch (token_)
        {
        case ELSE :
        case UNTIL :
        case END :
        case ENDFILE :
            return s;
        default :
        {
            match(SEMI);
            TreeNode * t = statement();
            if(t)
            {
                s->sibling = t;
                s = t;
            }
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
    default:
        break;
    }
}
