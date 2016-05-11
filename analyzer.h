#ifndef ANALYZER_H
#define ANALYZER_H

#include <unordered_map>
#include <list>

struct TreeNode;


struct Symbol
{
    size_t line_no;
    std::string name;
    size_t location;
};

class Analyzer
{
public:
    Analyzer(TreeNode * t);
    void type_check();
    void build_symbol_table();
    size_t find_symbol(const std::string & name);
    void print_symbol_table();

private:
    void traverse(TreeNode * t, auto pre, auto post);
    void check_node(TreeNode * t);
    void type_check_error(TreeNode * t, const char *);

    void type_check(TreeNode * t);
    void build_symbol_table(TreeNode * t);

    void insert_symbol(TreeNode * t);

    std::unordered_map<std::string, std::list<Symbol> > symbol_table_;
    size_t location_;

    TreeNode * root_;
};

#endif // ANALYZER_H
