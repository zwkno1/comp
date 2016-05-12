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

    size_t find_symbol(const std::string & name);

    void print_symbol_table();

private:

    void type_check();
    void build_symbol_table();

    void traverse(TreeNode * t, auto pre, auto post);
    void check_node(TreeNode * t);
    void type_check_error(TreeNode * t, const char *);

    void type_check(TreeNode * t);
    void build_symbol_table(TreeNode * t);

    void insert_symbol(TreeNode * t);

    Parser & parser_;

    std::unordered_map<std::string, Symbol> symbol_table_;
    size_t location_;
};

#endif // ANALYZER_H
