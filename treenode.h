#ifndef TREENODE_H
#define TREENODE_H

#include <stddef.h>
#include <string>

#include "token.h"

enum NodeKind
{
    StmtK, ExpK
};

enum StmtKind
{
    IfK, RepeatK, AssignK, ReadK, WriteK
};

enum ExprKind
{
    OpK, ConstK, IdK
};

/* ExprType is used for type checking */
enum ExprType
{
    Void, Integer, Boolean
};

struct TreeNode
{
    enum { child_size = 3 };

    TreeNode * child[child_size];
    TreeNode * sibling;
    size_t line_no;
    NodeKind kind;

    union
    {
        StmtKind stmt;
        ExprKind expr;
    } sub_kind;

    union
    {
        TokenType op;
        int val;
        std::string name;
    } attr;

    ExprType type;
};

#endif // TREENODE_H
