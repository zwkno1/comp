#ifndef SCANNER_H
#define SCANNER_H

#include <istream>

#include "token.h"

class Lexer
{
public:
    Lexer(std::istream & in);
	TokenType getToken();
    const std::string & id() const { return id_; }
	int num() const { return num_; }
    size_t line() const { return line_; }

private:
    void next();

    void putback();

    bool check_next(char ch);

    void increase_line();

    bool skip_comment();

    bool skip_comment2();

    bool read_num();

    bool read_id();

    std::istream & in_;

    std::string id_;

    int num_;

    size_t line_;

    int current_;
};

#endif // SCANNER_H
