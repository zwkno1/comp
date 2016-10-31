#ifndef TOKEN_H
#define TOKEN_H

enum TokenType
{
    IF,
    THEN,
    ELSE,
    END,
    REPEAT,
    UNTIL,

    READ,
    WRITE,
    ASSIGN,

    EQ,
    NE,
    LT,
    LE,
    GT,
    GE,

    ADD,
    SUB,
    MUL,
    DIV,

    LP,
    RP,
    SEMI,
    NUM,
    ID,
    ERROR,
    ENDFILE,
};

const char * getTokenStr(TokenType t);

#endif // TOKEN_H
