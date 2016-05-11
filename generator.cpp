#include "generator.h"

Generator::Generator(TreeNode * t, std::ostream & os)
    : root_(t)
    , os_(os)
{
}


void Generator::gen_code()
{
    gen_stmt_seq(root_);
}

void Generator::gen_stmt_seq(TreeNode * t)
{
    TreeNode *child = root_->child;
    while(child)
    {
        gen_stmt(child);
    }
}


void Generator::gen_stmt(TreeNode * t)
{
    switch (t->stmt_kind)
    {
    case IfK :
    {

    }
        break;
    case RepeatK :
        break;
    case AssignK :
        break;
    case ReadK :
        break;
    case WriteK :
        break;
    default:
        break;
    }
}

void Generator::gen_expr(TreeNode * t)
{
    switch (t->expr_kind)
    {
    default:
        break;
    }
}
