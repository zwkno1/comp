#include "lexer.h"

#include <map>
#include <stdio.h>

namespace
{
	std::map<std::string, TokenType> reserve_word =
	{
		{ "if",     IF },
		{ "then",   THEN },
		{ "else",   ELSE },
		{ "end",    END },
		{ "repeat", REPEAT },
		{ "until",  UNTIL },
		{ "read",   READ },
		{ "write",  WRITE },
	};

}

Lexer::Lexer(std::istream & in)
    : in_(in)
    , line_(0)
{
}

TokenType Lexer::getToken()
{
	while(true)
	{
        next();
        switch(current_)
        {
        case EOF:
            return ENDFILE;
        case ' ' : case '\f' : case '\t' : case '\v' :
            break;
        case '\n' :
            increase_line();
            break;
        case '=' :
            if(check_next('='))
                return EQ;
            return ASSIGN;
        case '<' :
            if(check_next('='))
                return LE;
            return LT;
        case '+' :
            return ADD;
        case '-' :
            return SUB;
        case '*' :
            return MUL;
        case '(' :
            return LP;
        case ')' :
            return RP;
        case ';' :
            return SEMI;
        case '/' :
            if(check_next('/'))
            {
                if(skip_comment() == false)
                    return ERROR;
                break;
            }

            if(check_next('*'))
            {
                if(skip_comment2() == false)
                    return ERROR;
                break;
            }

            return DIV;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            read_num();
            return NUM;
        default :
            if(read_id())
            {
                auto iter = reserve_word.find(id_);
                if(iter != reserve_word.end())
                    return iter->second;
                return ID;
            }
            else
                return ERROR;
        }
    }
}

void Lexer::next()
{
    current_ = in_.get();
}

void Lexer::putback()
{
    in_.putback(current_);
}

bool Lexer::check_next(char ch)
{
    next();
    if(current_ != ch)
    {
        putback();
        return false;
    }

    return true;
}

void Lexer::increase_line()
{
    ++line_;
}

bool Lexer::read_num()
{
    num_ = current_ - '0';
    while(true)
    {
        next();
        if(current_ == EOF || !std::isdigit(current_))
        {
            putback();
            return true;
        }
        else
        {
            num_ = num_ * 10 + (current_ - '0');
        }
    }
}

bool Lexer::read_id()
{
    id_.clear();
    if(current_ == EOF || ((!std::isalpha(current_)) && (current_ != '_')))
        return false;
    while(true)
    {
        if(current_ == EOF || (!std::isalnum(current_) && current_ != '_'))
        {
            putback();
            return true;
        }
        else
        {
            id_.push_back(current_);
            next();
        }
    }
}

bool Lexer::skip_comment()
{
    while (true)
    {
        next();
        if(current_ == EOF)
            return false;
        else if(current_ == '\n')
        {
            increase_line();
            return true;
        }
    }
}

bool Lexer::skip_comment2()
{
    while (true)
    {
        next();
        if(current_ == EOF)
            return false;
        else if(current_ == '*')
        {
asterisk:
            next();
            switch (current_)
            {
            case EOF:
                return false;
            case '/':
                return true;
                break;
            case '*':
                goto asterisk;
            case '\n':
                increase_line();
                break;
            default:
                break;
            }
        }
        else if(current_ == '\n')
        {
            increase_line();
        }
    }
}
