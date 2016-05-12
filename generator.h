#ifndef GENERATOR_H
#define GENERATOR_H

#include <iostream>

#include "treenode.h"
#include "analyzer.h"

class Generator
{
public:
    Generator(Analyzer & analyzer, std::ostream & os = std::cout);
    void gen_code();
private:
    void gen_code(TreeNode * t);
    void gen_stmt_seq(TreeNode * t);
    void gen_stmt(TreeNode * t);
    void gen_expr(TreeNode * t);

    size_t skip_location(size_t size);
    void backup_location(size_t loc);
    void restore_location();
    void increase_location();

    void gen_ro(const char *op, int r, int s, int t);
    void gen_rm(const char * op, int r, int d, int s);
    void gen_rm_abs(const char *op, int r, int a);

    Analyzer & analyzer_;

    std::ostream & os_;

    size_t current_location_;
    size_t highest_location_;

    /* tmpOffset is the memory offset for temps
     * It is decremented each time a temp is
     * stored, and incremeted when loaded again
     */
    size_t tmp_offset_;
};

#endif // GENERATOR_H
