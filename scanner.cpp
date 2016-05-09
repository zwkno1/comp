#include "scanner.h"

#include <map>
#include <stdio.h>

namespace
{
	enum
	{
		START, INSLASH, INCOMMENT, INNUM, INID, INCOLON,
	};

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

Scanner::Scanner(std::istream & in)
: in_(in)
	, line_(0)
{
}

TokenType Scanner::getToken()
{
	int state = START;

	while(true)
	{
		get();
		switch(state)
		{
			case START:
				{
					switch(c_)
					{
						case '=' :
							return EQ;
						case '<' :
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
							state = INSLASH;
							break;
						case ':':
							state = INCOLON;
							break;
						case ' ':
						case '\t':
							break;
						case '\n':
							++line_;
							break;
						case EOF:
							return ENDFILE;
							break;
						default :
							{
								if(std::isdigit(c_))
								{
									num_ = c_ - '0';
									state = INNUM;
								}
								else if(std::isalpha(c_) || c_ == '_')
								{
									id_.clear();
									id_.push_back(c_);
									state = INID;
								}
								else
								{
									return ERROR;
								}
							}
							break;
					}
				}
				break;
			case INSLASH:
				{
					if(c_ == '/')
					{
						state = INCOMMENT;
					}
					else
					{
						putback();
						return DIV;
					}
				}
				break;
			case INCOMMENT:
				{
					if(c_ == '\n')
					{
						++line_;
						state = START;
					}
				}
				break;
			case INNUM:
				{
					if(std::isdigit(c_))
					{
						num_ += num_ *10;
					}
					else
					{
						putback();
						return NUM;
					}
				}
				break;
			case INID:
				{
					if(std::isalnum(c_) || c_ == '_')
					{
						id_.push_back(c_);
					}
					else
					{
						putback();
						auto iter = reserve_word.find(id_);
						if(iter != reserve_word.end())
						  return iter->second;
						else
						  return ID;
					}
				}
				break;
			case INCOLON:
				{
					if(c_ == '=')
					{
						return ASSIGN;
					}
					else
					{
						return ERROR;
					}
				}
				break;
			default:
				return ERROR;
		}
	}

}

void Scanner::get()
{
	c_ = in_.get();
}

void Scanner::putback()
{
	in_.putback(c_);
}
