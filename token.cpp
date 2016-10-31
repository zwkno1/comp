#include "token.h"

namespace
{

const char * token_str[] =
{
    "IF",
    "THEN",
    "ELSE",
    "END",
    "REPEAT",
    "UNTIL",

    "READ",
    "WRITE",
    "ASSIGN",

    "EQ",
    "NE",
    "LT",
    "LE",
    "GT",
    "GE",

    "ADD",
    "SUB",
    "MUL",
    "DIV",

    "LP",
    "RP",
    "SEMI",
    "NUM",
    "ID",
    "ERROR",
    "ENDFILE",
};

} // namespace

const char * getTokenStr(TokenType t)
{
    return token_str[t];
}
