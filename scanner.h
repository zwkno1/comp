#ifndef SCANNER_H
#define SCANNER_H

#include <istream>

#include "token.h"

class Scanner
{
public:
	Scanner(std::istream & in);
	TokenType getToken();
    const std::string & id() const { return id_; }
	int num() const { return num_; }
    size_t line() const { return line_; }

private:
	void get();
	void putback();

    std::istream & in_;

    std::string id_;
    int num_;

    size_t line_;
	char c_;
};

#endif // SCANNER_H
