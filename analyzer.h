#ifndef ANALYZER_H
#define ANALYZER_H

#include <unordered_map>
#include <list>

#include "parser.h"

struct TreeNode;


struct Symbol
{
    std::string name;
    size_t location;
    std::list<size_t> lines;
};

class Analyzer
{
public:
    Analyzer(Parser & parser);
    void analysis();

    TreeNode * root() { return parser_.root(); }

    size_t findSymbol(const std::string & name);

    void printSymbolTable();

private:

    void typeCheck();
    void buildSymbolTable();

    void traverse(TreeNode * t, auto pre, auto post);
    void checkNode(TreeNode * t);
    void typeCheckError(TreeNode * t, const char *);

    void typeCheck(TreeNode * t);
    void buildSymbolTable(TreeNode * t);

    void insertSymbol(TreeNode * t);

    Parser & parser_;

    std::unordered_map<std::string, Symbol> symbol_table_;
    size_t location_;
};

#endif // ANALYZER_H
