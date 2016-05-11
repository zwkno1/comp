#ifndef TREENODE_H
#define TREENODE_H

#include <stddef.h>
#include <string>

#include "token.h"

enum NodeKind
{
    StmtSeqK, StmtK, ExprK
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
    TreeNode()
        : child(0)
        , sibling(0)
        , line_no(0)
    {
    }

    TreeNode * child;
    TreeNode * sibling;
    size_t line_no;
    NodeKind kind;

    StmtKind stmt_kind;
    ExprKind expr_kind;

    TokenType op;
    int value;
    std::string name;

    //type check
    ExprType expr_type;
};


//struct StmtNode : public TreeNode
//{
//    StmtNode() : kind(StmtK) {}
//
//    StmtKind stmt_kind;
//};
//
//struct ExprNode : public TreeNode
//{
//    ExprNode() : kind(ExprK) {}
//
//    ExprKind expr_kind;
//};
//
//struct IdNode : public ExprNode
//{
//    IdNode() : expr_kind(IdK) {}
//
//    std::string name;
//};
//
//struct ConstNode : public ExprNode
//{
//    ConstNode() : expr_kind(ConstK) {}
//
//    int value;
//};
//
//struct OpNode : public ExprNode
//{
//    OpNode() : expr_kind(OpK) {}
//
//    TokenType op;
//};



#endif // TREENODE_H
