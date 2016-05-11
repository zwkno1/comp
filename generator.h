#ifndef GENERATOR_H
#define GENERATOR_H

#include "treenode.h"
#include <iostream>

class Generator
{
public:
    Generator(TreeNode * t, std::ostream & os = std::cout);
    void gen_code();
private:
    void gen_stmt_seq(TreeNode * t);
    void gen_stmt(TreeNode * t);
    void gen_expr(TreeNode * t);

    TreeNode * root_;

    std::ostream & os_;
};

#endif // GENERATOR_H
