#ifndef GENERATOR_H
#define GENERATOR_H

#include <iostream>

#include "treenode.h"
#include "analyzer.h"

class Generator
{
public:
    Generator(Analyzer & analyzer, std::ostream & os = std::cout);
    void genCode();
private:
    void genCode(TreeNode * t);
    void genStmtSeq(TreeNode * t);
    void genStmt(TreeNode * t);
    void genExpr(TreeNode * t);

    size_t skipLocation(size_t size);
    void backupLocation(size_t loc);
    void restoreLocation();
    void increaseLocation();

    void genRO(const char *op, int r, int s, int t);
    void genRM(const char * op, int r, int d, int s);
    void genRMAbs(const char *op, int r, int a);

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
